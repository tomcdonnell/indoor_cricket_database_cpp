/*************************************************************************************************\
*                                                                                                 *
* "IndCrk.cpp" -                                                                                  *
*                                                                                                 *
*       Author - Tom McDonnell 2005                                                               *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "IndCrk.h"

#include <TomsLibrary/misc.h> // needed for max() which ought to be in <algorithm>

#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <fstream>

// STATIC MEMBER CONSTANT DEFINITIONS /////////////////////////////////////////////////////////////

const int matchRecord::nPlayersPerTeam  =  8,
          matchRecord::nOversPerInnings = 16;

// MEMBER FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

// member functions of class matchRecord //

/*
 * Constructor.
 */
matchRecord::matchRecord(void)
: date(), teamName(), oppTeamName(), teamPenalty(0, 50, 0), oppTeamPenalty(0, 50, 0),
  teamScore(indCrkRecords::minPossibleRunsScoredPerInns * nPlayersPerTeam,
            indCrkRecords::maxPossibleRunsScoredPerInns * nPlayersPerTeam, 0),
  oppTeamScore(indCrkRecords::minPossibleRunsScoredPerInns * nPlayersPerTeam,
               indCrkRecords::maxPossibleRunsScoredPerInns * nPlayersPerTeam, 0)
{
   battingRec = new battingRecord[nPlayersPerTeam];
   bowlingRec = new bowlingRecord[nOversPerInnings];
}

/*
 * Print summary of match record to ostream 'o'.
 */
void matchRecord::printResult(std::ostream &o) const
{
   using std::endl;
   using std::setw;

   o << date                                              << "  "
     << setw(titleRecord::titleMaxLength) << teamName     << " "
     << setw(3)                           << teamScore    << "  "
     << setw(titleRecord::titleMaxLength) << oppTeamName  << " "
     << setw(3)                           << oppTeamScore << "  ";

   if      (teamScore > oppTeamScore) o << setw(6) << "W";
   else if (teamScore < oppTeamScore) o << setw(6) << "L";
   else                               o << setw(6) << "D";

   o << endl;
}

/*
 * Print complete match record to ostream 'o'.
 */
void matchRecord::printRecord(std::ostream &o) const
{
   using std::endl;
   using std::setw;
   using std::string;

   const int scrW = 79; // screen width (no. characters)

   char h  = (char)196, v  = (char)179, // horizontal,  vertical     line
        tl = (char)218, tr = (char)191, // top-left,    top-right    border corner
        bl = (char)192, br = (char)217; // bottom-left, bottom-right border corner

   int i,      // counter
       w1, w2; // widths

   string fill; // for filling rest of line with a character (eg. '-')

   string blankLine     = v + string(77, ' ') + v;
   string separatorLine = v + string(77,   h) + v;

   // print title and top border
   fill = string(scrW - 19, h);
   o << tl << h << char(17) << " Match Record " << char(16)
     << fill << tr << endl
     << blankLine  << endl;

   // print team names and date
   fill = string(scrW - 26 - teamName.get().length() - oppTeamName.get().length(), ' ');
   o << v << " " << teamName << " Vs " << oppTeamName << fill
     << "  Date: " << date << " " << v << endl
     << separatorLine << endl
     << blankLine     << endl;

   // print match result
   o << v << " Match result: ";
        if (teamScore > oppTeamScore) o << "Win ";
   else if (teamScore < oppTeamScore) o << "Loss";
   else                               o << "Draw";
   fill = string(scrW - 22, ' ');
   o << fill << " " << v << endl
     << blankLine        << endl;

   // print team scores heading
   fill = string(scrW - 15, ' ');
   o << v << " Final Scores" << fill << v << endl
     << separatorLine                     << endl;

   // print team scores
   w1 = max(teamName.get().length(), oppTeamName.get().length());
   fill = string(scrW - w1 - 8, ' ');
   o << v << " " << setw(w1) <<    teamName
     << fill     << setw(4)  <<    teamScore << " " << v << endl;
   o << v << " " << setw(w1) << oppTeamName
     << fill     << setw(4)  << oppTeamScore << " " << v << endl;
   o << blankLine                                        << endl;

   // find width of longest first and last names
   // NOTE: assuming only nPlayersPerTeam played - ie. no substitutes
   w1 = w2 = 0;
   for (i = 0; i < matchRecord::nPlayersPerTeam; ++i)
   {
      w1 = max(w1, static_cast<int>(battingRec[i].batsmansName.getFirstName().length()));
      w2 = max(w2, static_cast<int>(battingRec[i].batsmansName.getLastName().length()));
   }

   // print batting scores heading
   fill = string(scrW - 30, ' ');
   o << v << " Batting Scores";

   // print whether team batted first or second
   switch (batted1st)
   {
    case true:  o << " (batted 1st)"; break;
    case false: o << " (batted 2nd)"; break;
   }

   o << fill << v     << endl
     << separatorLine << endl;

   // print batting scores
   fill = string(scrW - w1 - w2 - 8, ' ');
   for (i = 0; i < matchRecord::nPlayersPerTeam; ++i)
   {
      o << v << " "; battingRec[i].batsmansName.printFormatted(o, w1, w2);
      o << fill;
      o << setw(3)  << battingRec[i].runsScored << " "
        << v << endl;
   }
   o << blankLine << endl;

   // print team penalty (if applicable)
   if (teamPenalty > 0)
   {
      fill = string(scrW - 18 - digits(teamPenalty), ' ');
      o << v << " Penalty Runs " << fill << -teamPenalty << " " << v << endl
        << blankLine                                             << endl;
   }

   // print bowling scores heading
   fill = string(scrW - 17, ' ');
   o << v << " Bowling Scores" << fill << v << endl
     << separatorLine << endl;

   // print bowling scores
   fill = string(scrW - w1 - w2 - 13, ' ');
   for (i = 0; i < matchRecord::nOversPerInnings; ++i)
   {
      o << v << " "; bowlingRec[i].bowlersName.printFormatted(o, w1, w2);
      o << fill;
      o << setw(2)  << bowlingRec[i].wicketsTaken << " / "
        << setw(3)  << bowlingRec[i].runsConceded << " "
        << v << endl;
   }
   o << blankLine << endl;

   // print opposition team penalty (if applicable)
   if (oppTeamPenalty > 0)
   {
      fill = string(scrW - 18 - digits(oppTeamPenalty), ' ');
      o << v << " Penalty Runs " << fill << -oppTeamPenalty << " " << v << endl
        << blankLine                                                << endl;
   }

   // print bottom border
   fill = string(scrW - 2, h);
   o << bl << fill << br << endl;
}

/*
 *
 */
void matchRecord::getDataFromUser(void)
{
   using std::cin;
   using std::cout;
   using std::endl;

   cout                                 << endl
        << "Creating new match record." << endl
                                        << endl;

   // read date, team names, and team scores
           date.getFromUser("Match date?   ");
       teamName.getFromUser("Team name?                ");
      teamScore.getFromUser("Team score?               ");
    oppTeamName.getFromUser("Opposition team name?     ");
   oppTeamScore.getFromUser("Opposition team score?    ");

   // read which team batted first
   std::ostringstream msg;

   cout << endl;

   msg << "Did " << teamName << endl
       << "bat first? (y/n)          ";
   getBoolFromUser(batted1st, msg.str());

   int i;

   // read batting scores
   cout << endl << teamName << " batting scores." << endl;
   for (i = 0; i < nPlayersPerTeam; ++i)
   {
      std::ostringstream temp;
      temp << "Batsman " << i + 1 << ": Name?          ";
      battingRec[i].getFromUser(temp.str(), "           Runs scored?   ");
   }

   // read team penalty runs
   cout << endl;
   teamPenalty.getFromUser("Team Penalty Runs?        ");

   // read bowling scores
   cout << endl << teamName << " bowling scores." << endl;
   for (i = 0; i < nOversPerInnings; ++i)
   {
      std::ostringstream temp;
      temp << "Bowler " << std::setw(2) << i + 1 << ": Name?          ";

      bowlingRec[i].getFromUser(temp.str(), "           Wickets taken? ",
                                            "           Runs conceded? " );
   }

   // read opposition team penalty runs
   cout << endl;
   oppTeamPenalty.getFromUser("Opp. Team Penalty Runs?   ");

   cout                             << endl
        << "Match record complete." << endl
                                    << endl;

   if (!checkDataIntegrityAndPrintResults())
     getUserToEditData();
}

/*
 *
 */
void matchRecord::getUserToEditData(void)
{
   using std::cin;
   using std::cout;
   using std::endl;
   using std::setw;

   bool happyWithData = false;
   while (!happyWithData)
   {
      cout << "Editing match record." << endl
                                      << endl;

      // PROBLEM: should need only one msg
      std::ostringstream msg[6 + 2 * nPlayersPerTeam + 2 * nOversPerInnings + 2];

      // give option to edit date, team names, and team scores
      msg[0] << "Match date:               " << date;
      if (!userHappy(msg[0].str(), 41))
        date.getFromUser("Match date?   ");

      msg[1] << "Team name:                " << teamName;
      if (!userHappy(msg[1].str(), 51 - teamName.get().length()))
        teamName.getFromUser("Team name?                ");
 
      msg[2] << "Team score:               " << teamScore;
      if (!userHappy(msg[2].str(), 51 - digits(teamScore)))
        teamScore.getFromUser("Team score?               ");

      msg[3] << "Opposition team name:     " << oppTeamName;
      if (!userHappy(msg[3].str(), 51 - oppTeamName.get().length()))
        oppTeamName.getFromUser("Opposition team name?     ");

      msg[4] << "Opposition team score:    " << oppTeamScore;
      if (!userHappy(msg[4].str(), 51 - digits(oppTeamScore)))
        oppTeamScore.getFromUser("Opposition team score?    ");

      // give option to edit which team batted first
      switch (batted1st)
      {
       case true:  msg[5] << endl << teamName << endl << "batted first. "; break;
       case false: msg[5] << endl << teamName << endl << "batted second."; break;
      }
      if (!userHappy(msg[5].str(), 63))
      {
         std::ostringstream batted1stMsg;
         batted1stMsg << "Did " << teamName << " bat first? (y/n) ";
         getBoolFromUser(batted1st, batted1stMsg.str());
      }

      int i;

      // give option to edit batting scores
      cout << endl << teamName << " batting scores." << endl;
      for (i = 0; i < nPlayersPerTeam; ++i)
      {
         msg[6 + 2 * i]
           << "Batsman " << i + 1 << ": Name:          " << getBattingRec(i).batsmansName << endl
           << "        " << " "   << "  Runs scored:   " << getBattingRec(i).runsScored;

         if (!userHappy(msg[6 + 2 * i].str(), 51 - digits(getBattingRec(i).runsScored)))
         {
            msg[6 + 2 * i + 1] << "Batsman " << i + 1 << ": Name?          ";
            battingRec[i].getFromUser(msg[6 + 2 * i + 1].str(), "           Runs scored?   ");
         }
      }

      // give option to edit team penalty runs
      cout << endl;
      msg[6 + 2 * nPlayersPerTeam] << "Team Penalty Runs:        " << teamPenalty;
      if (!userHappy(msg[6 + 2 * nPlayersPerTeam].str(), 51 - digits(teamPenalty)))
        teamPenalty.getFromUser("Team Penalty Runs?        ");

      // give option to edit bowling scores
      cout << endl << teamName << " bowling scores." << endl;
      for (i = 0; i < nOversPerInnings; ++i)
      {
         msg[6 + 2 * nPlayersPerTeam + 1 + 2 * i]
           << "Bowler " << std::setw(2) << i + 1 << ": Name:          "
                                                 << getBowlingRec(i).bowlersName  << endl
           << "       "                 << "  "  << "  Wickets taken: "
                                                 << getBowlingRec(i).wicketsTaken << endl
           << "       "                 << "  "  << "  Runs conceded: "
                                                 << getBowlingRec(i).runsConceded;

         if (!userHappy(msg[6 + 2 * nPlayersPerTeam + 1 + 2 * i].str(),
                        51 - digits(getBowlingRec(i).runsConceded)))
         {
           msg[6 + 2 * nPlayersPerTeam + 1 + 2 * i + 1]
             << "Bowler " << std::setw(2) << i + 1 << ": Name?          ";
           bowlingRec[i].getFromUser(msg[6 + 2 * nPlayersPerTeam + 1 + 2 * i + 1].str(),
                                     "           Wickets taken? ",
                                     "           Runs conceded? "                       );
         }
      }

      // give option to edit opposition team penalty runs
      cout << endl;
      msg[6 + 2 * nPlayersPerTeam + 1 + 2 * nOversPerInnings]
        << "Opp. Team Penalty Runs:   " << oppTeamPenalty;
      if (!userHappy(msg[6 + 2 * nPlayersPerTeam + 1 + 2 * nOversPerInnings].str(),
                     51 - digits(oppTeamPenalty)                                   ))
        oppTeamPenalty.getFromUser("Opp. Team Penalty Runs?   ");

      cout                                     << endl
           << "Edit of match record complete." << endl
                                               << endl;

      happyWithData = checkDataIntegrityAndPrintResults();
   }
}

/*
 * Read match record from ifstream 'in' into 'm'.
 * Return true if succeed, false if fail.
 * 'lineNo' useful for file format error detection.
 * Format must match that used in matchRecord::writeToFile().
 */
bool matchRecord::readFromFile(std::ifstream &in, int &lineNo)
{
   using std::cerr;
   using std::cout;
   using std::endl;

   // exception classes
   struct expectedNewLineErr {expectedNewLineErr(void) {}};
   struct expectedYorNErr    {expectedYorNErr(void)    {}};

   int &l = lineNo;

   // test for NULL stream
   if (in == NULL)
     return false;

   // test for end of file
   if (in.peek() == EOF)
     return false;

   try
   {
      // read date
      in >> date;             ++l;

      if (readChar(in, '\n')) ++l; else throw expectedNewLineErr();

      // read team name and score, opposition team name and score
      in >> teamName;         ++l;
      in >> teamScore;        ++l;
      in >> oppTeamName;      ++l;
      in >> oppTeamScore;     ++l;

      if (readChar(in, '\n')) ++l; else throw expectedNewLineErr();

      // read which team batted first
      switch(in.get())
      {
       case 'Y': batted1st = true;  break;
       case 'N': batted1st = false; break;
       default: throw expectedYorNErr();
      }

      if (readChar(in, '\n')) ++l; else throw expectedNewLineErr();

      // read team batting scores
      int i;
      for (i = 0; i < matchRecord::nPlayersPerTeam; ++i)
      {
         in >> battingRec[i]; l += 2;
      }

      if (readChar(in, '\n')) ++l; else throw expectedNewLineErr();

      // read team penalty
      in >> teamPenalty;      ++l;

      if (readChar(in, '\n')) ++l; else throw expectedNewLineErr();

      // read team bowling scores
      for (i = 0; i < matchRecord::nOversPerInnings; ++i)
      {
         in >> bowlingRec[i]; l += 3;
      }

      if (readChar(in, '\n')) ++l; else throw expectedNewLineErr();

      // read opposition team penalty
      in >> oppTeamPenalty;   ++l;

      if (readChar(in, '\n')) ++l; else throw expectedNewLineErr();
      if (readChar(in, '\n')) ++l; else throw expectedNewLineErr();
   }
   catch (dateRecord::dateRecordErr)
   {
      cout << "File format (date) error at line number "           << lineNo << "." << endl;
      exit(1);
   }
   catch (titleRecord::titleRecordErr)
   {
      cout << "File format (team name) error at line number "      << lineNo << "." << endl;
      exit(1);
   }
   catch (nameRecord::nameRecordErr)
   {
      cout << "File format (name) error at line number "           << lineNo << "." << endl;
      exit(1);
   }
   catch (bowlingRecord::bowlingRecordErr)
   {
      cout << "File format (bowling record) error at line number " << lineNo << "." << endl;
      exit(1);
   }
   catch (expectedNewLineErr)
   {
      cout << "File format error at line number " << lineNo << "." << endl
           << " Expected newline character ('\\n')."               << endl;
      exit(1);
   }
   catch (expectedYorNErr)
   {
      cout << "File format error at line number " << lineNo << "." << endl
           << " Expected Y or N."                                  << endl;
   }
   catch (...)
   {
      std::cout << "Unknown error at line number " << lineNo << "." << endl;
      exit(1);
   }

   return true;
}

/*
 * Write match record to ofstream 'out'.
 * Return true if succeed, false if fail.
 * Format must match that used in matchRecord::readFromFile().
 */
bool matchRecord::writeToFile(std::ofstream &out) const
{
   using std::endl;
   using std::setw;

   try
   {
      // print date, team names, and team scores
      out << date         << endl
                          << endl
          << teamName     << endl
          << teamScore    << endl
          << oppTeamName  << endl
          << oppTeamScore << endl
                          << endl;

      // print Y or N depending on whether team batted first
      switch (batted1st)
      {
       case true:  out << 'Y' << endl << endl; break;
       case false: out << 'N' << endl << endl; break;
      }

      // print batting stats
      int i;
      for (i = 0; i < matchRecord::nPlayersPerTeam; ++i)
        out << battingRec[i].batsmansName << endl
            << battingRec[i].runsScored   << endl;

      out                << endl
          << teamPenalty << endl
                         << endl;

      // print bowling stats
      for (i = 0; i < matchRecord::nOversPerInnings; ++i)
        out << bowlingRec[i].bowlersName  << endl
            << bowlingRec[i].wicketsTaken << endl
            << bowlingRec[i].runsConceded << endl;

      out                   << endl
          << oppTeamPenalty << endl
                            << endl
                            << endl;
   }
   catch (...)
   {
      reportCaughtException("Unknown",
                            "bool matchRecord::writeToFile(std::ofstream &out, int &lineNo)",
                            "Indoor Cricket DB.cpp"                                          );
   }

   return true;
}

/*
 *
 */
bool matchRecord::checkDataIntegrityAndPrintResults(void) const
{
   using std::cout;
   using std::endl;

   int sumBatt, // sum team batting scores         minus     team penalty
       sumBowl, // sum runs conceded while bowling minus opp team penalty
       w,       // width (for formatting)
       i;       // counter

   bool errorDetected = false;
   cout << "Checking data integrity...";

   if (!teamScoreEqSumBattingScoresMinusPenalty(sumBatt))
   {
      errorDetected = true;

      cout << endl
           << "Error detected: "
           << "Team score "                     << getTeamScore() << " != "
           << "Sum batting scores - penalties " << sumBatt        << endl;
   }

   if (!oppTeamScoreEqSumRunsConcededMinusPenalty(sumBowl))
   {
      errorDetected = true;

      cout << endl
           << "Error detected: "
           << "Opposition team score "         << getOppTeamScore() << " != "
           << "Sum runs conceded - penalties " << sumBowl           << endl;
   }

   std::vector<nameRecord> uNames;
   findUniquePlayerNames(uNames);
   if (uNames.size() > nPlayersPerTeam)
   {
      errorDetected = true;

      cout                                                                << endl
           << "Error detected: Too many different names in match record." << endl
           << "                There should be " << nPlayersPerTeam
           << " players per team."                                        << endl;

      // find width of longest first name
      w = 0;
      for (i = 0; i < uNames.size(); ++i)
        w = max(w, static_cast<int>(uNames[i].getFirstName().length()));

      for (i = 0; i < uNames.size(); ++i)
      {
         uNames[i].printFormatted(cout, w);
         cout << endl;
      }
   }

   if (!errorDetected)
   {
      cout << "no problems found." << endl;
      return true;
   }
   else
     return false;
}

/*
 * Test that teamScore equals sum of batting scores minus team penalties, as it should.
 */
bool matchRecord::teamScoreEqSumBattingScoresMinusPenalty(int &s) const
{
   s = 0; // sum batting scores
   for (int i = 0; i < nPlayersPerTeam; ++i)
     s += getBattingRec(i).runsScored;

   s -= teamPenalty;

   if (s == teamScore) return true;
   else                return false;
}

/*
 * Test that oppTeamScore equals sum of runs conceded while bowling
 * minus opposition team penalties, as it should.
 */
bool matchRecord::oppTeamScoreEqSumRunsConcededMinusPenalty(int &s) const
{
   s = 0; // sum runs conceded while bowling
   for (int i = 0; i < nOversPerInnings; ++i)
     s += getBowlingRec(i).runsConceded;

   s -= oppTeamPenalty;

   if (s == oppTeamScore) return true;
   else                   return false;
}

/*
 * Non-const list reference used so that unique names may be found for entire database for example.
 */
void matchRecord::findUniquePlayerNames(std::vector<nameRecord> &uNames) const
{
   int i;

   // find unique names in batting names
   if (uNames.size() == 0)
     uNames.insert(uNames.end(), getBattingRec(0).batsmansName); // 1st name is always unique
   for (i = 1; i < nPlayersPerTeam; ++i)
     if (std::find(uNames.begin(), uNames.end(), getBattingRec(i).batsmansName) == uNames.end())
       uNames.insert(uNames.end(), getBattingRec(i).batsmansName);

   // find unique names in bowling names
   for (i = 1; i < nOversPerInnings; ++i)
     if (std::find(uNames.begin(), uNames.end(), getBowlingRec(i).bowlersName ) == uNames.end())
       uNames.insert(uNames.end(), getBowlingRec(i).bowlersName);
}

/*
 *
 */
void matchRecord::replacePlayerName(const nameRecord &oldName, const nameRecord &newName)
{
   int i;

   // replace name(s) in batting list
   for (i = 0; i < nPlayersPerTeam; ++i)
     if (getBattingRec(i).batsmansName == oldName)
       battingRec[i].batsmansName.setName(newName);

   // replace name(s) in bowling list
   for (i = 0; i < nOversPerInnings; ++i)
     if (getBowlingRec(i).bowlersName == oldName)
       bowlingRec[i].bowlersName.setName(newName);
}

/*
 * Check whether player with name 'name' played in the match.
 */
bool matchRecord::playerPlayed(const nameRecord &name) const
{
   int i;

   // get batting contribution
   for (i = 0; i < nPlayersPerTeam; ++i)
     if (name == getBattingRec(i).batsmansName)
       return true;

   // get bowling contribution
   for (i = 0; i < nOversPerInnings; ++i)
     if (name == getBowlingRec(i).bowlersName)
       return true;

   return false;
}

/*
 *
 */
void matchRecord::getPlayerStats(const nameRecord &playerName, playerMatchStats &r) const
{
   // set match details
   r.date         = date;
   r.oppTeamName  = oppTeamName;

   // initialise totals
   r.innsBatted   = 0;
   r.runsScored   = 0;
   r.oversBowled  = 0;
   r.wicketsTaken = 0;
   r.runsConceded = 0;

   int i;

   // get batting stats
   for (i = 0; i < nPlayersPerTeam; ++i)
     if (playerName == getBattingRec(i).batsmansName)
     {
        r.innsBatted++;
        r.runsScored += getBattingRec(i).runsScored;
     }

   // get bowling stats
   for (i = 0; i < nOversPerInnings; ++i)
     if (playerName == getBowlingRec(i).bowlersName)
     {
        r.oversBowled++;
        r.wicketsTaken += getBowlingRec(i).wicketsTaken;
        r.runsConceded += getBowlingRec(i).runsConceded;
     }
}

/*
 *
 */
void matchRecord::getPlayerStats(const nameRecord &playerName, playerRecordsStruct &r) const
{
   int  runsThisMatch      = 0, //  |
        innsThisMatch      = 0, //  |-- needed for calculating match contribution
        oversThisMatch     = 0, //  |
        wktsTakenThisMatch = 0, //  |
        runsConcThisMatch  = 0, //  |
        i;

   // get batting stats
   for (i = 0; i < matchRecord::nPlayersPerTeam; ++i)
   {
      if (getBattingRec(i).batsmansName == playerName)
      {
         innsThisMatch++;
         runsThisMatch += getBattingRec(i).runsScored;

         r.innsBatted++;
         r.runsScored += getBattingRec(i).runsScored;

         // BEST INNINGS
         // test whether this innings is better than best seen so far
         if (getBattingRec(i).runsScored > r.bestInns.front().runsScored)
         {
            // remove all but one record
            while (r.bestInns.size() > 1)
              r.bestInns.pop_back();

            // update remaining record
            r.bestInns.front()
              = playerInnsRecord(getBattingRec(i).runsScored,
                                 getBattingRec(i).batsmansName, getOppTeamName(), getDate());
         }
         else
           // test whether this innings is equal to the best seen so far
           if (getBattingRec(i).runsScored == r.bestInns.front().runsScored)
             // add record to list
             r.bestInns.push_back(playerInnsRecord(getBattingRec(i).runsScored,
                                                   getBattingRec(i).batsmansName,
                                                   getOppTeamName(), getDate()));

         // WORST INNINGS
         // test whether this innings is worse than worst seen so far
         if (getBattingRec(i).runsScored < r.wrstInns.front().runsScored)
         {
            // remove all but one record
            while (r.wrstInns.size() > 1)
              r.wrstInns.pop_back();

            // update remaining record
            r.wrstInns.front()
              = playerInnsRecord(getBattingRec(i).runsScored,
                                 getBattingRec(i).batsmansName, getOppTeamName(), getDate());
         }
         else
           // test whether this innings is equal to the worst seen so far
           if (getBattingRec(i).runsScored == r.wrstInns.front().runsScored)
             // add record to list
             r.wrstInns.push_back(playerInnsRecord(getBattingRec(i).runsScored,
                                                   getBattingRec(i).batsmansName,
                                                   getOppTeamName(), getDate()));
      }
   }

   // get bowling contribution
   for (i = 0; i < matchRecord::nOversPerInnings; ++i)
   {
      if (getBowlingRec(i).bowlersName == playerName)
      {
         oversThisMatch++;
         wktsTakenThisMatch += getBowlingRec(i).wicketsTaken;
         runsConcThisMatch  += getBowlingRec(i).runsConceded;

         r.oversBowled++;
         r.wicketsTaken += getBowlingRec(i).wicketsTaken;
         r.runsConceded += getBowlingRec(i).runsConceded;

         // BEST OVER
         // test whether this over is better than best seen so far
         if (getBowlingRec(i).runsConceded < r.bestOver.front().runsConceded)
         {
            // remove all but one record
            while (r.bestOver.size() > 1)
              r.bestOver.pop_back();

            // update remaining record
            r.bestOver.front() = playerOverRecord(getBowlingRec(i).wicketsTaken,
                                                  getBowlingRec(i).runsConceded,
                                                  getBowlingRec(i).bowlersName,
                                                  getOppTeamName(), getDate()   );
         }
         else
           // test whether this over is equal to the best seen so far
           if (getBowlingRec(i).runsConceded == r.bestOver.front().runsConceded)
             // add record to list
             r.bestOver.push_back(playerOverRecord(getBowlingRec(i).wicketsTaken,
                                                   getBowlingRec(i).runsConceded,
                                                   getBowlingRec(i).bowlersName,
                                                   getOppTeamName(), getDate())  );

         // WORST OVER
         // test whether this over is worse than worst seen so far
         if (getBowlingRec(i).runsConceded > r.wrstOver.front().runsConceded)
         {
            // remove all but one record
            while (r.wrstOver.size() > 1)
              r.wrstOver.pop_back();

            // update remaining record
            r.wrstOver.front() = playerOverRecord(getBowlingRec(i).wicketsTaken,
                                                  getBowlingRec(i).runsConceded,
                                                  getBowlingRec(i).bowlersName,
                                                  getOppTeamName(), getDate()   );
         }
         else
           // test whether this over is equal to the worst seen so far
           if (getBowlingRec(i).runsConceded == r.wrstOver.front().runsConceded)
             // add record to list
             r.wrstOver.push_back(playerOverRecord(getBowlingRec(i).wicketsTaken,
                                                   getBowlingRec(i).runsConceded,
                                                   getBowlingRec(i).bowlersName,
                                                   getOppTeamName(), getDate()   ));
      }
   }

   if (oversThisMatch > 0)
   {
      // calculate players match bowling figures
      double bowlFigsRunsConc  = bowlFigsRunsConceded(oversThisMatch, runsConcThisMatch),
             bowlFigsWktsTaken = bowlFigsWicketsTaken(oversThisMatch, wktsTakenThisMatch);


      // BEST BOWLING FIGURES
      // test whether this match bowling figures are better than best seen so far
      if (bowlFigsRunsConc < r.bestBowlFigs.front().runsConceded)
      {
         // remove all but one record
         while (r.bestBowlFigs.size() > 1)
           r.bestBowlFigs.pop_back();

         // update remaining record
         r.bestBowlFigs.front() = playerBowlFigsRecord(bowlFigsWktsTaken,
                                                       bowlFigsRunsConc,
                                                       playerName,
                                                       getOppTeamName(), getDate());
      }
      else
        // test whether this match bowling figures are equal to the best seen so far
        if (bowlFigsRunsConc == r.bestBowlFigs.front().runsConceded)
          // add record to list
          r.bestBowlFigs.push_back(playerBowlFigsRecord(bowlFigsWktsTaken,
                                                        bowlFigsRunsConc,
                                                        playerName,
                                                        getOppTeamName(), getDate()));

      // WORST BOWLING FIGURES
      // test whether this match bowling figures are worse than worst seen so far
      if (bowlFigsRunsConc > r.wrstBowlFigs.front().runsConceded)
      {
          // remove all but one record
          while (r.wrstBowlFigs.size() > 1)
            r.wrstBowlFigs.pop_back();

          // update remaining record
          r.wrstBowlFigs.front() = playerBowlFigsRecord(bowlFigsWktsTaken,
                                                        bowlFigsRunsConc,
                                                        playerName,
                                                        getOppTeamName(), getDate());
      }
      else
        // test whether this match bowling figures are equal to the worst seen so far
        if (bowlFigsRunsConc == r.wrstBowlFigs.front().runsConceded)
          // add record to list
          r.wrstBowlFigs.push_back(playerBowlFigsRecord(bowlFigsWktsTaken,
                                                        bowlFigsRunsConc,
                                                        playerName,
                                                        getOppTeamName(), getDate()));
      
      if (innsThisMatch > 0)
      {
         // calculate players match contribution
         double matchContr = matchContribution(innsThisMatch, runsThisMatch,
                                               oversThisMatch, runsConcThisMatch);

         // BEST MATCH CONTRIBUTION
         // test whether this match contribution is better than best seen so far
         if (matchContr > r.bestMatchContr.front().matchContr)
         {
            // remove all but one record
            while (r.bestMatchContr.size() > 1)
              r.bestMatchContr.pop_back();

            // update remaining record
            r.bestMatchContr.front()
              = playerMatchContrRecord(matchContr, playerName, getOppTeamName(), getDate());
         }
         else
           // test whether this match contribution is equal to the best seen so far
           if (matchContr == r.bestMatchContr.front().matchContr)
             // add record to list
             r.bestMatchContr.push_back(playerMatchContrRecord(matchContr, playerName,
                                                               getOppTeamName(), getDate()));

         // WORST MATCH CONTRIBUTION
         // test whether this match contribution is worse than worst seen so far
         if (matchContr < r.wrstMatchContr.front().matchContr)
         {
            // remove all but one record
            while (r.wrstMatchContr.size() > 1)
              r.wrstMatchContr.pop_back();

            // update remaining record
            r.wrstMatchContr.front()
              = playerMatchContrRecord(matchContr, playerName, getOppTeamName(), getDate());
         }
         else
           // test whether this match contribution is equal to the worst seen so far
           if (matchContr == r.wrstMatchContr.front().matchContr)
             // add record to list
             r.wrstMatchContr.push_back(playerMatchContrRecord(matchContr, playerName,
                                                               getOppTeamName(), getDate()));
      } // end if (innsThisMatch > 0)
   } // end if (oversThisMatch > 0)
}

/*
 *
 */
void matchRecord::getTeamStats(teamMatchStats &r) const
{
   // set match details
   r.date         = date;
   r.teamScore    = teamScore;
   r.oppTeamName  = oppTeamName;
   r.oppTeamScore = oppTeamScore;

   // get wicket tally
   r.wicketTally = 0;
   for (int i = 0; i < nOversPerInnings; ++i)
     r.wicketTally += getBowlingRec(i).wicketsTaken;
}

/*
 *
 */
void matchRecord::getTeamStats(teamRecordsStruct &r) const
{
   // BEST TEAM SCORE
   // test whether this teamScore is better than the best seen so far
   if (teamScore > r.bestScore.front().value)
   {
      // remove all but one record
      while (r.bestScore.size() > 1)
        r.bestScore.pop_back();

      // update remaining record
      r.bestScore.front() = teamGenericRecord(teamScore, oppTeamName, date);
   }
   else
     // test whether this teamScore is equal to the best seen so far
     if (teamScore == r.bestScore.front().value)
       // add record to list
       r.bestScore.push_back(teamGenericRecord(teamScore, oppTeamName, date));


   // WORST TEAM SCORE
   // test whether this teamScore is worse than the worst seen so far
   if (teamScore < r.wrstScore.front().value)
   {
      // remove all but one record
      while (r.wrstScore.size() > 1)
        r.wrstScore.pop_back();

      // update remaining record
      r.wrstScore.front() = teamGenericRecord(teamScore, oppTeamName, date);
   }
   else
     // test whether this teamScore is equal to the worst seen so far
     if (teamScore == r.wrstScore.front().value)
       // add record to list
       r.wrstScore.push_back(teamGenericRecord(teamScore, oppTeamName, date));

   // BEST OPPOSITION TEAM SCORE
   // test whether this oppTeamScore is better than the best seen so far
   if (oppTeamScore > r.bestOppScore.front().value)
   {
      // remove all but one record
      while (r.bestOppScore.size() > 1)
        r.bestOppScore.pop_back();

      // update remaining record
      r.bestOppScore.front() = teamGenericRecord(oppTeamScore, oppTeamName, date);
   }
   else
     // test whether this oppTeamScore is equal to the best seen so far
     if (oppTeamScore == r.bestOppScore.front().value)
       // add record to list
       r.bestOppScore.push_back(teamGenericRecord(oppTeamScore, oppTeamName, date));


   // WORST OPPOSITION TEAM SCORE
   // test whether this oppTeamScore is worse than the worst seen so far
   if (oppTeamScore < r.wrstOppScore.front().value)
   {
      // remove all but one record
      while (r.wrstOppScore.size() > 1)
        r.wrstOppScore.pop_back();

      // update remaining record
      r.wrstOppScore.front() = teamGenericRecord(oppTeamScore, oppTeamName, date);
   }
   else
     // test whether this oppTeamScore is equal to the worst seen so far
     if (oppTeamScore == r.wrstOppScore.front().value)
       // add record to list
       r.wrstOppScore.push_back(teamGenericRecord(oppTeamScore, oppTeamName, date));

   // BEST MARGIN
   // calculate winning (or losing) margin
   int margin = teamScore - oppTeamScore;

   // test whether this margin is better than the best seen so far
   if (margin > r.bestMargin.front().value)
   {
      // remove all but one record
      while (r.bestMargin.size() > 1)
        r.bestMargin.pop_back();

      // update remaining record
      r.bestMargin.front() = teamGenericRecord(margin, oppTeamName, date);
   }
   else
     // test whether this margin is equal to the best seen so far
     if (margin == r.bestMargin.front().value)
       // add record to list
       r.bestMargin.push_back(teamGenericRecord(margin, oppTeamName, date));


   // WORST MARGIN
   // test whether this margin is worse than the worst seen so far
   if (margin < r.wrstMargin.front().value)
   {
      // remove all but one record
      while (r.wrstMargin.size() > 1)
        r.wrstMargin.pop_back();

      // update remaining record
      r.wrstMargin.front() = teamGenericRecord(margin, oppTeamName, date);
   }
   else
     // test whether this margin is equal to the worst seen so far
     if (margin == r.wrstMargin.front().value)
       // add record to list
       r.wrstMargin.push_back(teamGenericRecord(margin, oppTeamName, date));


   // calculate match wicket tally
   int w = getTeamWicketTally();

   // BEST TEAM WICKET TALLY
   // test whether this wicket tally is better than the best seen so far
   if (w > r.bestWicketTally.front().value)
   {
      // remove all but one record
      while (r.bestWicketTally.size() > 1)
        r.bestWicketTally.pop_back();

      // update remaining record
      r.bestWicketTally.front() = teamGenericRecord(w, oppTeamName, date);
   }
   else
     // test whether this wicket tally is equal to the best seen so far
     if (w == r.bestWicketTally.front().value)
       // add record to list
       r.bestWicketTally.push_back(teamGenericRecord(w, oppTeamName, date));


   // WORST TEAM WICKET TALLY
   // test whether this wicketTally is worse than the worst seen so far
   if (w < r.wrstWicketTally.front().value)
   {
      // remove all but one record
      while (r.wrstWicketTally.size() > 1)
        r.wrstWicketTally.pop_back();

      // update remaining record
      r.wrstWicketTally.front() = teamGenericRecord(w, oppTeamName, date);
   }
   else
     // test whether this wicket tally is equal to the worst seen so far
     if (w == r.wrstWicketTally.front().value)
       // add record to list
       r.wrstWicketTally.push_back(teamGenericRecord(w, oppTeamName, date));
}

/*
 *
 */
void matchRecord::getBestBattPship(std::list<playerInnsRecord> &b1,
                                   std::list<playerInnsRecord> &b2 ) const
{
   for (int i = 0; i < nPlayersPerTeam ; i += 2)
   {
      // test whether this batting pship is better than the best seen so far
      if (  battingRec[i].runsScored + battingRec[i + 1].runsScored
          > b1.front().runsScored    + b2.front().runsScored       )
      {
         // remove all but one record from both lists
         while (b1.size() > 1) b1.pop_back();
         while (b2.size() > 1) b2.pop_back();

         // update remaining records
         b1.front() = playerInnsRecord(battingRec[i].runsScored,
                                       battingRec[i].batsmansName,
                                       teamName, date             );
         b2.front() = playerInnsRecord(battingRec[i + 1].runsScored,
                                       battingRec[i + 1].batsmansName,
                                       teamName, date                 );
      }
      else
        // test whether this batting pship is equal to the best seen so far
        if (   battingRec[i].runsScored + battingRec[i + 1].runsScored
            == b1.front().runsScored    + b2.front().runsScored       )
        {
           // add record to list
           b1.push_back(playerInnsRecord(battingRec[i].runsScored,
                                         battingRec[i].batsmansName,
                                         teamName, date             ));
           b2.push_back(playerInnsRecord(battingRec[i + 1].runsScored,
                                         battingRec[i + 1].batsmansName,
                                         teamName, date                 ));
        }
   }
}

/*
 *
 */
void matchRecord::getWrstBattPship(std::list<playerInnsRecord> &b1,
                                   std::list<playerInnsRecord> &b2 ) const
{
   for (int i = 0; i < nPlayersPerTeam ; i += 2)
   {
      // test whether this batting pship is worse than the worst seen so far
      if (  battingRec[i].runsScored + battingRec[i + 1].runsScored
          < b1.front().runsScored    + b2.front().runsScored       )
      {
         // remove all but one record from both lists
         while (b1.size() > 1) b1.pop_back();
         while (b2.size() > 1) b2.pop_back();

         // update remaining records
         b1.front() = playerInnsRecord(battingRec[i].runsScored,
                                       battingRec[i].batsmansName,
                                       teamName, date             );
         b2.front() = playerInnsRecord(battingRec[i + 1].runsScored,
                                       battingRec[i + 1].batsmansName,
                                       teamName, date                 );
      }
      else
        // test whether this batting pship is equal to the worst seen so far
        if (   battingRec[i].runsScored + battingRec[i + 1].runsScored
            == b1.front().runsScored    + b2.front().runsScored       )
        {
           // add record to list
           b1.push_back(playerInnsRecord(battingRec[i].runsScored,
                                         battingRec[i].batsmansName,
                                         teamName, date             ));
           b2.push_back(playerInnsRecord(battingRec[i + 1].runsScored,
                                         battingRec[i + 1].batsmansName,
                                         teamName, date                 ));
        }
   }
}

void matchRecord::testFunc(playerInnsRecord &a)
{
}

void testFunc2(playerInnsRecord)
{
}

/*****************************************END*OF*FILE*********************************************/
