/*************************************************************************************************\
*                                                                                                 *
* "IndCrkDB.cpp" -                                                                                *
*                                                                                                 *
*         Author - Tom McDonnell 2005                                                             *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "IndCrkDB.h"
#include "IndCrkRecords.h"

#include <TomsLibrary/misc.h>

#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

// MEMBER FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

// file I/O functions //

/*
 * Read match data from file "fileName" into list 'l'.
 * PROBLEM: maybe don't want to output to screen directly.
 */
bool indCrkDB::readFromFile(void)
{
   using std::cout;
   using std::endl;

   matchRecord *mPtr;
   
   cout << "Reading data from file \"" << fileName << "\"." << endl;
 
   // open file
   std::ifstream file(fileName.c_str());

   if (file == NULL)
   {
      cout << "File \"" << fileName << "\" could not be read." << endl;

      return false;
   }

   // read file
   int lines = 1; // to enable file format error to be located
   while (file.peek() != EOF)
   {
      mPtr = new matchRecord;

      if (mPtr == NULL)
      {
         std::cerr << "Out of memory." << endl;
         exit(0);
      }

      mPtr->readFromFile(file, lines);

      cout << "."; // progress indicator

      db.insert(db.end(), *mPtr);
   }

   // close file
   file.close();

   cout << endl
        << db.size() << " match records read successfully." << endl;

   return true;
}

/*
 *
 */
bool indCrkDB::writeToFile(void) const
{
   std::ofstream file(fileName.c_str(), std::ios_base::trunc);

   std::list<matchRecord>::const_iterator p;
   for (p = db.begin(); p != db.end(); ++p)
     p->writeToFile(file);

   return true;
}

/*
 * integrity check function
 */
void indCrkDB::checkDBintegrityAndPrintResults(void) const
{
   using std::cout;
   using std::endl;

   std::list<matchRecord>::const_iterator p;
   for (p = db.begin(); p != db.end(); ++p)
   {
      cout << endl
           << p->getDate() << ": " << p->getTeamName() << " Vs " << p->getOppTeamName() << endl;

      p->checkDataIntegrityAndPrintResults();
   }
}

// print functions //

/*
 *
 */
void indCrkDB::printMatchRecord(const titleRecord &teamName,
                                const dateRecord  &date, std::ostream &out) const
{
   std::list<matchRecord>::const_iterator p = constFirstMatchWithDate(date);

   if (p->getDate() == date && p->getTeamName() == teamName)
     p->printRecord(out);
}


// database manipulation functions //

/*
 * Insert matchRecord into database.
 */
void indCrkDB::insertMatchRecord(const matchRecord &m)
{
   insertSorted(m);

   // PROBLEM: must handle failure case
}

/*
 *
 */
void deleteMatchRecord(const dateRecord &, const titleRecord &teamName)
{
}

/*
 * Note: Does not update file.
 */
bool indCrkDB::getUserToEditMatchRecord(const dateRecord &date, const titleRecord &teamName)
{
   // find match with date matching or after that specified
   std::list<matchRecord>::iterator p = firstMatchWithDate(date);

   while (p->getDate() == date)
   {
      if (p->getTeamName() == teamName)
      {
         p->getUserToEditData();

         // sort match record list (date may have changed)
         sortByDate();

         return true;
      }
      ++p;
   }
   return false;
}

/*
 *
 */
void indCrkDB::compilePlayerRecords(const nameRecord          &playerName,
                                    const titleRecord         &teamName,
                                    const titleRecord         &oppTeamName,
                                    const dateBeginEnd        &period,
                                          playerRecordsStruct &r           ) const
{
   // initialise
   r.playerName = playerName;
   r.teamName   = teamName;
   r.period     = period;

   // compile player totals and best/worst performance records
   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(r.period.begin);
        p != db.end() && p->getDate() <= r.period.end; ++p) // order important
   {
      if (p->getTeamName() == r.teamName && p->playerPlayed(r.playerName)
          && (oppTeamName == titleRecord() || p->getOppTeamName() == oppTeamName))
      {
         r.matchesPlayed++;

         p->getPlayerStats(r.playerName, r);
      }
   }

   // calculate player's averages
   r.avgRunsScrdPerInns  =   static_cast<double>(r.runsScored)
                           / static_cast<double>(r.innsBatted);
   r.avgWktsTakenPerOver =   static_cast<double>(r.wicketsTaken)
                           / static_cast<double>(r.oversBowled);
   r.avgRunsConcPerOver  =   static_cast<double>(r.runsConceded)
                           / static_cast<double>(r.oversBowled);
   r.avgMatchContr = matchContribution(r.innsBatted, r.runsScored, r.oversBowled, r.runsConceded);
}

/*
 * The list compiled here may be sorted by
 * any criteria to form player ranking tables
 */
void indCrkDB::compilePlayerRecords(const titleRecord              &teamName,
                                    const titleRecord              &oppTeamName,
                                    const dateBeginEnd             &period,
                                    std::list<playerRecordsStruct> &playerRecordsList) const
{
   // clear player records list
   playerRecordsList.clear();

   // compile player names list
   std::vector<nameRecord> uNames; compilePlayersList(period, teamName, uNames);

   playerRecordsStruct tempRecordsStruct;
   std::vector<nameRecord>::const_iterator namePtr;
   for (namePtr = uNames.begin(); namePtr != uNames.end(); ++namePtr)
   {
      // initialise tempRecordsStruct
      tempRecordsStruct = playerRecordsStruct();

      compilePlayerRecords(*namePtr, teamName, oppTeamName, period, tempRecordsStruct);

      playerRecordsList.push_back(tempRecordsStruct);
   }
}

/*
 *
 */
void indCrkDB::compilePlayerHistory(const nameRecord          &playerName,
                                    const titleRecord         &teamName,
                                    const titleRecord         &oppTeamName,
                                    const dateBeginEnd        &period,
                                          playerHistoryStruct &h          ) const
{
   // initialise
   h.period     = period;
   h.playerName = playerName;
   h.teamName   = teamName;

   // compile player history
   playerMatchStats matchStats;
   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getTeamName() == teamName && p->playerPlayed(playerName)
         && (oppTeamName == titleRecord() || p->getOppTeamName() == oppTeamName))
     {
        p->getPlayerStats(playerName, matchStats);

        h.matchStatsVect.push_back(matchStats);
     }
}

/*
 *
 */
void indCrkDB::compileTeamRecords(const titleRecord  &teamName,
                                  const titleRecord  &oppTeamName,
                                  const dateBeginEnd &period,
                                  teamRecordsStruct  &r        ) const
{
   using teamRecordsStruct::overall;
   using teamRecordsStruct::batting1st;
   using teamRecordsStruct::batting2nd;
   using teamRecordsStruct::withFullTeam;
   using teamRecordsStruct::missing1Player;
   using teamRecordsStruct::missing2Players;

   std::vector<nameRecord> uNames;

   // initialise
   r.teamName       = teamName;
   r.period         = period;
   r.longestWstreak = 0;
   r.longestLstreak = 0;

   int          currentWstreak = 0,
                currentLstreak = 0;
   dateBeginEnd currentWstreakPeriod,
                currentLstreakPeriod;

   // compile team best/worst performances
   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(r.period.begin);
        p != db.end() && p->getDate() <= r.period.end; ++p) // order important
   {
      if (p->getTeamName() == r.teamName
          && (oppTeamName == titleRecord() || p->getOppTeamName() == oppTeamName))

      {
         // tally results in 'overall' category
         r.category[overall].matchesPlayed++;
              if (p->getTeamScore() > p->getOppTeamScore()) r.category[overall].matchesWon++;
         else if (p->getTeamScore() < p->getOppTeamScore()) r.category[overall].matchesLost++;
         else                                               r.category[overall].matchesDrawn++;
         r.category[overall].runsScored   += p->getTeamScore();       // note: includes penalties
         r.category[overall].runsConceded += p->getOppTeamScore();    // note: includes penalties
         r.category[overall].wicketsTaken += p->getTeamWicketTally();

         // tally results in 'batting1st' category
         if (p->battedFirst())
         {
            r.category[batting1st].matchesPlayed++;

                 if (p->getTeamScore() > p->getOppTeamScore()) r.category[batting1st].matchesWon++;
            else if (p->getTeamScore() < p->getOppTeamScore())
              r.category[batting1st].matchesLost++;
            else
              r.category[batting1st].matchesDrawn++;

            r.category[batting1st].runsScored   += p->getTeamScore();    // note:includes pens
            r.category[batting1st].runsConceded += p->getOppTeamScore(); // note:includes pens
            r.category[batting1st].wicketsTaken += p->getTeamWicketTally();
         }

         // tally results in 'batting2nd' category
         else {
            r.category[batting2nd].matchesPlayed++;

                 if (p->getTeamScore() > p->getOppTeamScore()) r.category[batting2nd].matchesWon++;
            else if (p->getTeamScore() < p->getOppTeamScore())
              r.category[batting2nd].matchesLost++;
            else
              r.category[batting2nd].matchesDrawn++;

            r.category[batting2nd].runsScored   += p->getTeamScore();    // note:includes pens
            r.category[batting2nd].runsConceded += p->getOppTeamScore(); // note:includes pens
            r.category[batting2nd].wicketsTaken += p->getTeamWicketTally();
         }

         // count players in team
         uNames.clear();
         p->findUniquePlayerNames(uNames);

         // tally results in 'withFullTeam' category
         if (uNames.size() == 8)
         {
            r.category[withFullTeam].matchesPlayed++;

            if (p->getTeamScore() > p->getOppTeamScore())
              r.category[withFullTeam].matchesWon++;
            else if (p->getTeamScore() < p->getOppTeamScore())
              r.category[withFullTeam].matchesLost++;
            else
              r.category[withFullTeam].matchesDrawn++;
            r.category[withFullTeam].runsScored   += p->getTeamScore();    // note: includes pens.
            r.category[withFullTeam].runsConceded += p->getOppTeamScore(); // note: includes pens.
            r.category[withFullTeam].wicketsTaken += p->getTeamWicketTally();
         }

         // tally results in 'missing1Player' category
         else if (uNames.size() == 7)
         {
            r.category[missing1Player].matchesPlayed++;

                 if (p->getTeamScore() > p->getOppTeamScore())
              r.category[missing1Player].matchesWon++;
            else if (p->getTeamScore() < p->getOppTeamScore())
              r.category[missing1Player].matchesLost++;
            else
              r.category[missing1Player].matchesDrawn++;

            r.category[missing1Player].runsScored   += p->getTeamScore();    // note: includes pens
            r.category[missing1Player].runsConceded += p->getOppTeamScore(); // note: includes pens
            r.category[missing1Player].wicketsTaken += p->getTeamWicketTally();
         }

         // tally results in 'missing2Players' category
         else if (uNames.size() == 6)
         {
            r.category[missing2Players].matchesPlayed++;

                 if (p->getTeamScore() > p->getOppTeamScore())
              r.category[missing2Players].matchesWon++;
            else if (p->getTeamScore() < p->getOppTeamScore())
              r.category[missing2Players].matchesLost++;
            else
              r.category[missing2Players].matchesDrawn++;

            r.category[missing2Players].runsScored   += p->getTeamScore();    // note:includes pens
            r.category[missing2Players].runsConceded += p->getOppTeamScore(); // note:includes pens
            r.category[missing2Players].wicketsTaken += p->getTeamWicketTally();
         }

         // keep track of streaks
         if (p->getTeamScore() > p->getOppTeamScore())
         {
            // win
            if (currentWstreak++ == 0)
            {
               currentWstreakPeriod.begin = p->getDate();
               currentWstreakPeriod.end   = p->getDate();
            }
            else
              currentWstreakPeriod.end = p->getDate();

            // PROBLEM: need to account for when there are equal longest streaks
            if (currentWstreak > r.longestWstreak)
            {
               r.longestWstreak       = currentWstreak;
               r.longestWstreakPeriod = currentWstreakPeriod;
            }

            currentLstreak = 0;
         }
         else if (p->getTeamScore() < p->getOppTeamScore())
         {
            // loss
            if (currentLstreak++ == 0)
            {
               currentLstreakPeriod.begin = p->getDate();
               currentLstreakPeriod.end   = p->getDate();
            }
            else
              currentLstreakPeriod.end = p->getDate();

            // PROBLEM: need to account for when there are equal longest streaks
            if (currentLstreak > r.longestLstreak)
            {
               r.longestLstreak       = currentLstreak;
               r.longestLstreakPeriod = currentLstreakPeriod;
            }

            currentWstreak = 0;
         }

         p->getTeamStats(r); // keep track of highest/lowest scores etc.
      }
   }

   for (int i = 0; i < 6; ++i)
   {
      if (r.category[i].matchesPlayed != 0) // avoid dividing by zero
      {
         // calculate team's win rate
         r.category[i].winRate = 100 * (  static_cast<double>(r.category[i].matchesWon)
                                        / static_cast<double>(r.category[i].matchesPlayed));

         // calculate team's averages
         r.category[i].avgTeamScore    =   static_cast<double>(r.category[i].runsScored)
                                         / static_cast<double>(r.category[i].matchesPlayed);
         r.category[i].avgOppTeamScore =   static_cast<double>(r.category[i].runsConceded)
                                         / static_cast<double>(r.category[i].matchesPlayed);
         r.category[i].avgWicketTally  =   static_cast<double>(r.category[i].wicketsTaken)
                                         / static_cast<double>(r.category[i].matchesPlayed);
      }
      else
      {
         r.category[i].winRate         = 0.0;
         r.category[i].avgTeamScore    = 0.0;
         r.category[i].avgOppTeamScore = 0.0;
         r.category[i].avgWicketTally  = 0.0;
      }

      r.category[i].avgMargin = r.category[i].avgTeamScore - r.category[i].avgOppTeamScore;
   }
}

/*
 *
 */
void indCrkDB::compileTeamHistory(const titleRecord       &teamName,
                                  const titleRecord       &oppTeamName,
                                  const dateBeginEnd      &period,
                                        teamHistoryStruct &h        ) const
{
   // initialise
   h.period   = period;
   h.teamName = teamName;

   // compile team history
   teamMatchStats matchStats;
   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getTeamName() == teamName
         && (oppTeamName == titleRecord() || p->getOppTeamName() == oppTeamName))
     {
        p->getTeamStats(matchStats);

        h.matchStatsVect.push_back(matchStats);
     }
}

/*
 *
 */
void indCrkDB::compileTeamIndividualRecords(const titleRecord  &teamName,
                                            const titleRecord  &oppTeamName,
                                            const dateBeginEnd &period,
                                            teamIndividualRecordsStruct  &r) const
{
   // initialise
   r.teamName = teamName;
   r.period   = period;

   // compile player names list
   std::vector<nameRecord> uNames;
   compilePlayersList(period, teamName, uNames);

   // compile best/worst individual performances within team
   std::vector<nameRecord>::const_iterator namePtr;
   for (namePtr = uNames.begin(); namePtr != uNames.end(); ++namePtr)
     // note: since not initializing namePtr,
     //       overwriting players records with better/worse ones when they are found
     compilePlayerRecords(*namePtr, teamName, oppTeamName, period, r.bestPlayerRecords);

   // compile best/worst batting partnerships
   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(r.period.begin);
        p != db.end() && p->getDate() <= r.period.end; ++p) // order important
   {
      if (p->getTeamName() == r.teamName
          && (oppTeamName == titleRecord() || p->getOppTeamName() == oppTeamName))
      {
         int i;

         // Find best batting partnership(s)
         for (i = 0; i < p->nPlayersPerTeam ; i += 2)
         {
            // test whether this batting pship is better than the best seen so far
            if (  p->getBattingRec(i).runsScored      + p->getBattingRec(i + 1).runsScored
                > r.bestBattPship1.front().runsScored + r.bestBattPship2.front().runsScored)
            {
               // remove all but one record from both lists
               while (r.bestBattPship1.size() > 1) r.bestBattPship1.pop_back();
               while (r.bestBattPship2.size() > 1) r.bestBattPship2.pop_back();

               // update remaining records
               r.bestBattPship1.front() = playerInnsRecord(p->getBattingRec(i).runsScored,
                                                           p->getBattingRec(i).batsmansName,
                                                           p->getOppTeamName(), p->getDate());
               r.bestBattPship2.front() = playerInnsRecord(p->getBattingRec(i + 1).runsScored,
                                                           p->getBattingRec(i + 1).batsmansName,
                                                           p->getOppTeamName(), p->getDate()    );
            }
            else
              // test whether this batting pship is equal to the best seen so far
              if (   p->getBattingRec(i).runsScored      + p->getBattingRec(i + 1).runsScored
                  == r.bestBattPship1.front().runsScored + r.bestBattPship2.front().runsScored)
              {
                 // add record to list
                 r.bestBattPship1.push_back(playerInnsRecord(p->getBattingRec(i).runsScored,
                                                             p->getBattingRec(i).batsmansName,
                                                             p->getOppTeamName(), p->getDate()  ));
                 r.bestBattPship2.push_back(playerInnsRecord(p->getBattingRec(i + 1).runsScored,
                                                             p->getBattingRec(i + 1).batsmansName,
                                                             p->getOppTeamName(), p->getDate()  ));
              }
         }

         // Find worst batting partnership(s)
         for (i = 0; i < p->nPlayersPerTeam ; i += 2)
         {
            // test whether this batting pship is worse than the worst seen so far
            if (  p->getBattingRec(i).runsScored      + p->getBattingRec(i + 1).runsScored
                < r.wrstBattPship1.front().runsScored + r.wrstBattPship2.front().runsScored)
            {
               // remove all but one record from both lists
               while (r.wrstBattPship1.size() > 1) r.wrstBattPship1.pop_back();
               while (r.wrstBattPship2.size() > 1) r.wrstBattPship2.pop_back();

               // update remaining records
               r.wrstBattPship1.front() = playerInnsRecord(p->getBattingRec(i).runsScored,
                                                           p->getBattingRec(i).batsmansName,
                                                           p->getOppTeamName(), p->getDate());
               r.wrstBattPship2.front() = playerInnsRecord(p->getBattingRec(i + 1).runsScored,
                                                           p->getBattingRec(i + 1).batsmansName,
                                                           p->getOppTeamName(), p->getDate()    );
            }
            else
              // test whether this batting pship is equal to the worst seen so far
              if (   p->getBattingRec(i).runsScored      + p->getBattingRec(i + 1).runsScored
                  == r.wrstBattPship1.front().runsScored + r.wrstBattPship2.front().runsScored)
              {
                 // add record to list
                 r.wrstBattPship1.push_back(playerInnsRecord(p->getBattingRec(i).runsScored,
                                                             p->getBattingRec(i).batsmansName,
                                                             p->getOppTeamName(), p->getDate()  ));
                 r.wrstBattPship2.push_back(playerInnsRecord(p->getBattingRec(i + 1).runsScored,
                                                             p->getBattingRec(i + 1).batsmansName,
                                                             p->getOppTeamName(), p->getDate()  ));
              }
         }
      }
   }
}

// simple queries //

/*
 * Test whether player "playerName" played for team "teamName"
 * in any match on the date specified.
 */
bool indCrkDB::playerPlayed(const nameRecord   &playerName,
                            const titleRecord  &teamName,
                            const dateRecord   &date       ) const
{
   std::list<matchRecord>::const_iterator p = constFirstMatchWithDate(date);

   if (p->getDate() == date && p->getTeamName() == teamName && p->playerPlayed(playerName))
     return true;
   else
     return false;
}

/*
 * Test whether player "playerName" played for team "teamName"
 * in any match in the period specified.
 */
bool indCrkDB::playerPlayed(const nameRecord   &playerName,
                            const titleRecord  &teamName,
                            const dateBeginEnd &period     ) const
{
   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (   p->getTeamName() == teamName
         && p->playerPlayed(playerName) )
       return true;

   return false;
}

/*
 * Test whether player "playerName" played for team "teamName" in any match.
 */
bool indCrkDB::playerPlayed(const nameRecord &playerName, const titleRecord &teamName) const
{
   std::list<matchRecord>::const_iterator p;
   for (p = db.begin(); p != db.end(); ++p)
     if (   p->getTeamName() == teamName
         && p->playerPlayed(playerName) )
       return true;

   return false;
}

/*
 * Test whether team "teamName" played in any match in the period specified.
 */
bool indCrkDB::teamPlayed(const titleRecord &teamName, const dateRecord &date) const
{
   std::list<matchRecord>::const_iterator p = constFirstMatchWithDate(date);

   if (p->getDate() == date && p->getTeamName() == teamName)
     return true;
   else
     return false;
}

/*
 * Test whether team "teamName" played in any match in the period specified.
 */
bool indCrkDB::teamPlayed(const titleRecord &teamName, const dateBeginEnd &period) const
{
   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getTeamName() == teamName)
       return true;

   return false;
}

/*
 * Test whether team "teamName" played in any match.
 */
bool indCrkDB::teamPlayed(const titleRecord &teamName) const
{
   std::list<matchRecord>::const_iterator p;
   for (p = db.begin(); p != db.end(); ++p)
     if (p->getTeamName() == teamName)
       return true;

   return false;
}

/*
 * Test whether team "teamName" played in any match in the period specified.
 */
bool indCrkDB::teamPlayedOppTeam(const titleRecord &teamName,
                                 const titleRecord &oppTeamName,
                                 const dateRecord  &date        ) const
{
   std::list<matchRecord>::const_iterator p = constFirstMatchWithDate(date);

   if (p->getDate() == date && p->getTeamName() == teamName && p->getOppTeamName() == oppTeamName)
     return true;
   else
     return false;
}

/*
 * Test whether team "teamName" played in any match in the period specified.
 */
bool indCrkDB::teamPlayedOppTeam(const titleRecord  &teamName,
                                 const titleRecord  &oppTeamName,
                                 const dateBeginEnd &period      ) const
{
   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getTeamName() == teamName && p->getOppTeamName() == oppTeamName)
       return true;

   return false;
}

/*
 * Test whether team "teamName" played in any match.
 */
bool indCrkDB::teamPlayedOppTeam(const titleRecord &teamName,
                                 const titleRecord &oppTeamName) const
{
   std::list<matchRecord>::const_iterator p;
   for (p = db.begin(); p != db.end(); ++p)
     if (p->getTeamName() == teamName && p->getOppTeamName() == oppTeamName)
       return true;

   return false;
}

/*
 * Test whether opposition team "oppTeamName" played in any match in the period specified.
 */
bool indCrkDB::oppTeamPlayed(const titleRecord &oppTeamName, const dateRecord &date) const
{
   std::list<matchRecord>::const_iterator p = constFirstMatchWithDate(date);

   if (p->getDate() == date && p->getOppTeamName() == oppTeamName)
     return true;
   else
     return false;
}

/*
 * Test whether opposition team "oppTeamName" played in any match in the period specified.
 */
bool indCrkDB::oppTeamPlayed(const titleRecord &oppTeamName, const dateBeginEnd &period) const
{
   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getOppTeamName() == oppTeamName)
       return true;

   return false;
}

/*
 * Test whether opposition team "oppTeamName" played in any match.
 */
bool indCrkDB::oppTeamPlayed(const titleRecord &oppTeamName) const
{
   std::list<matchRecord>::const_iterator p;
   for (p = db.begin(); p != db.end(); ++p)
     if (p->getOppTeamName() == oppTeamName)
       return true;

   return false;
}

// database maintenance functions //

/*
 *
 */
void indCrkDB::compileTeamsList(const dateBeginEnd &period, std::vector<titleRecord> &v) const
{
   v.clear();

   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p)     // order important
     if (std::find(v.begin(), v.end(), p->getTeamName()) == v.end())
       v.insert(v.end(), p->getTeamName());
}

/*
 *
 */
void indCrkDB::compilePlayersList(const dateBeginEnd &period, std::vector<nameRecord> &v) const
{
   v.clear();

   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     p->findUniquePlayerNames(v);
}

/*
 *
 */
void indCrkDB::compilePlayersList(const dateBeginEnd &period,
                                  const titleRecord &teamName, std::vector<nameRecord> &v) const
{
   v.clear();

   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getTeamName() == teamName)
       p->findUniquePlayerNames(v);
}

/*
 *
 */
void indCrkDB::compilePlayersList(const dateBeginEnd      &period,
                                  const titleRecord       &teamName,
                                  const titleRecord       &oppTeamName,
                                  std::vector<nameRecord> &v           ) const
{
   v.clear();

   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getTeamName() == teamName && p->getOppTeamName() == oppTeamName)
       p->findUniquePlayerNames(v);
}

/*
 * Compile list of all opposition teams in database in given period.
 */
void indCrkDB::compileOppTeamsList(const dateBeginEnd &period, std::vector<titleRecord> &) const
{
}

/*
 * Compile list of all opposition teams who have played in
 * matches Vs team "teamName" in database in given period.
 */
void indCrkDB::compileOppTeamsList(const dateBeginEnd &period,
                                   const titleRecord &teamName, std::vector<titleRecord> &v) const
{
   v.clear();

   std::list<matchRecord>::const_iterator p;
   for (p = constFirstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getTeamName() == teamName)
       if (std::find(v.begin(), v.end(), p->getOppTeamName()) == v.end())
         v.insert(v.end(), p->getOppTeamName());
}

/*
 * Replace old player name with new in matches within specified period.
 * Note: does not update file.
 */
void indCrkDB::updatePlayerNames(const dateBeginEnd &period,  const titleRecord &teamName,
                                 const nameRecord   &oldName, const nameRecord  &newName  )
{
   std::list<matchRecord>::iterator p;
   for (p = firstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getTeamName() == teamName && p->playerPlayed(oldName))
       p->replacePlayerName(oldName, newName);
}

/*
 * Note: does not update file.
 */
void indCrkDB::updateTeamNames(const dateBeginEnd &period,
                               const titleRecord &oldTeamName, const titleRecord &newTeamName)
{
   std::list<matchRecord>::iterator p;
   for (p = firstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getTeamName() == oldTeamName)
       p->replaceTeamName(newTeamName);
}

/*
 * Note: does not update file.
 */
void indCrkDB::updateOppTeamNames(const dateBeginEnd &period, const titleRecord &teamName,
                                  const titleRecord &oldOppTeamName,
                                  const titleRecord &newOppTeamName)
{
   std::list<matchRecord>::iterator p;
   for (p = firstMatchWithDate(period.begin);
        p != db.end() && p->getDate() <= period.end; ++p) // order important
     if (p->getOppTeamName() == oldOppTeamName)
       p->replaceOppTeamName(newOppTeamName);
}

// PRIVATE MEMBER FUNCTIONS ///////////////////////////////////////////////////////////////////////

/*
 * Returns a const iterator pointing to the first match
 * with the date equal to or after the date specified.
 * Assumes that the list is sorted.
 */
std::list<matchRecord>::const_iterator
indCrkDB::constFirstMatchWithDate(const dateRecord &date) const
{
   std::list<matchRecord>::const_iterator p = db.begin();
   while (p != db.end() && p->getDate() < date)           // order important
     ++p;

   return p;
}

/*
 * Same as above but not const.
 */
std::list<matchRecord>::iterator
indCrkDB::firstMatchWithDate(const dateRecord &date)
{
   std::list<matchRecord>::iterator p = db.begin();
   while (p != db.end() && p->getDate() < date)     // order important
     ++p;

   return p;
}

/*
 *
 */
void indCrkDB::insertSorted(const matchRecord &m)
{
   db.insert(db.end(), m); // PROBLEM: should be able to insert into correct position

   // sort match record list
   db.sort(std::greater<matchRecord>());
}

/*****************************************END*OF*FILE*********************************************/
