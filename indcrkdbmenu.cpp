/*************************************************************************************************\
*                                                                                                 *
* "IndCrkDBmenu.cpp" - Indoor Cricket Database Menu System.                                       *
*                                                                                                 *
*             Author - Tom McDonnell 2005                                                         *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "IndCrkDBmenu.h"
#include "IndCrkDBui.h"
#include "IndCrkDB.h"

#include <TomsLibrary\titleRecord.h>

// FILE SCOPE CONSTANTS ///////////////////////////////////////////////////////////////////////////

namespace
{
   const std::string getPlayerNameString("Player's name?          "),
                       getTeamNameString("Team name?              "),
                   includeAllTeamsString("Include matches against\n"
                                         "all opposition teams?   "),
                     getMinMatchesString("Minimum no. matches to\n"
                                         "qualify for inclusion?  "),
                    getOppTeamNameString("Opposition Team Name?   "),
                        getOldNameString("Name to replace?        "),
                        getNewNameString("Replacement name?       "),
                    getOldTeamNameString("Team name to replace?   "),
                    getNewTeamNameString("Replacement team name?  "),
                      getMatchDateString("Match date? "            ); // leave space for format
}

// FILE SCOPE FUNCTION DECLARATIONS ///////////////////////////////////////////////////////////////

namespace
{
   void getOppTeamNameIfNecessary(const indCrkDB &db,
                                  titleRecord    &teamName,
                                  titleRecord    &oppTeamName,
                                  dateBeginEnd   &period      );
}

// FUNCTION DEFINITIONS ///////////////////////////////////////////////////////////////////////////

/*
 *
 */
void menuMainMenu(void)
{
   using std::cout;
   using std::endl;

   const std::string fileName("IndCrkDB.dat"); // default filename for database

   matchRecord *newMatchPtr; // used in menu option "Add Match Record"

   std::vector<titleRecord> teamNamesV; // |
   titleRecord teamName;                // |- used in menu option "List Opposition Team Names"
   dateBeginEnd period;                 // |

   indCrkDB db(fileName);
   db.readFromFile();
   db.sortByDate();

   bool exit = false;
   while (!exit)
   {
      cout                                   << endl
           << "Indoor Cricket Database"      << endl
           << "----------------------------" << endl
           << "Main Menu"                    << endl
           << "----------------------------" << endl
           << "Player Stats .......... -> 1" << endl
           << "Team Stats ............ -> 2" << endl
           << "Maintain Database ..... -> 3" << endl
           << "Add Match Record ......... 4" << endl
           << "List Teams ............... 5" << endl
           << "List Opposition Teams .... 6" << endl
           << "Database Summary ......... 7" << endl
           << "Information/Instructions . 8" << endl
           << "Exit ..................... 9" << endl
           << "----------------------------" << endl;

      std::cin.sync();

      switch (std::cin.get())
      {
       case '1': menuPlayerStats(db); break;
       case '2': menuTeamStats(db);   break;
       case '3': menuMaintainDB(db);  break;

       case '4': // menu choice = Add Match Record
         newMatchPtr = new matchRecord;      // Must dynamically allocate as
         newMatchPtr->getDataFromUser();     // insertMatchRecord() does not make copy.
         db.insertMatchRecord(*newMatchPtr); // Should not delete once inserted for same reason.
         updateDBfile(db);
         break;

       case '5': // menu choice = List Team Names
         cout << endl; // blank line

         // get period from user
         period.getFromUser();

         // compile team names list
         db.compileTeamsList(period, teamNamesV);

         if (teamNamesV.size() > 0)
         {
            cout                        << endl
                 << "Teams List"        << endl
                 << "Period " << period << endl;

            printTitlesVect(teamNamesV, cout);
         }
         else
           cout                                             << endl
                << "The database contains no match records" << endl 
                << "in the period " << period << "."        << endl;
         break;

       case '6': // menu choice = List Opposition Team Names
         cout << endl; // blank line

         // get teamName, period from user
         teamName.getFromUser(getTeamNameString);
         period.getFromUser();

         // compile opposition team names list
         db.compileOppTeamsList(period, teamName, teamNamesV);

         if (teamNamesV.size() > 0)
         {
            cout                                           << endl
                 << "Opposition Teams List - " << teamName << endl
                 << "Period " << period                    << endl;

            printTitlesVect(teamNamesV, cout);
         }
         else
           cout                                                         << endl
                << "The database contains no match records for team \""
                << teamName << "\" "                                    << endl 
                << "in the period " << period << "."                    << endl;
         break;

       case '7':              break;
       case '8':              break;
       case '9': exit = true; break;
      }
   }

   cout << endl;
}

/*
 *
 */
void menuPlayerStats(const indCrkDB &db)
{ 
   using std::cout;
   using std::endl;

   playerRecordsStruct r; // used in menu option Player Records
   playerHistoryStruct h; // used in menu option Player History

   cout << endl;

   // get playerName, teamName, period from user
   nameRecord   playerName; playerName.getFromUser(getPlayerNameString);
   titleRecord  teamName;   teamName.getFromUser(getTeamNameString);
   dateBeginEnd period;     period.getFromUser();

   // test whether player played for team in specified period, return if not
   if (!db.playerPlayed(playerName, teamName, period))
   {
      cout                                                                                 << endl
           << "The database contains no record of a player named \"" << playerName << "\"" << endl
           << "playing in a team named \"" << teamName << "\""                             << endl
           << "in the period " << period << "."                                            << endl;
      return;
   }

   // get oppTeamName if necessary
   titleRecord oppTeamName; getOppTeamNameIfNecessary(db, teamName, oppTeamName, period);

   bool exit = false;
   while (!exit)
   {
      cout                                   << endl
           << "Indoor Cricket Database"      << endl
           << "----------------------------" << endl
           << "Player Stats Menu"            << endl
           << "(" << playerName << ", " << teamName << ")" << endl;

      // print oppTeamName if necessary
      if (oppTeamName != titleRecord())
        cout << "(Vs " << oppTeamName << ")" << endl;

      cout << period                         << endl
           << "----------------------------" << endl
           << "Player Records ........... 1" << endl
           << "Player History ........... 2" << endl
           << "Exit to Main Menu ........ 3" << endl
           << "----------------------------" << endl;

      std::cin.sync();

      switch (std::cin.get())
      {
       case '1': // menu choice = Player Records
         // compile player records
         cout << endl << "Compiling player records...";
         db.compilePlayerRecords(playerName, teamName, oppTeamName, period, r);
         cout << "done." << endl;

         // print player records
         r.print(std::cout);
         break;

       case '2': // menu choice = Player History
         // compile player history
         cout << endl << "Compiling player history...";
         db.compilePlayerHistory(playerName, teamName, oppTeamName, period, h);
         cout << "done." << endl;

         // print player history
         h.print(cout);
         break;

       case '3': exit = true; break;
      }
   }
}

/*
 *
 */
void menuTeamStats(const indCrkDB &db)
{
   using std::cout;
   using std::cin;
   using std::endl;

   dateRecord date;                // used in menu choice Match Record
   std::vector<nameRecord> namesV; // used in menu choice List Player Names
   teamRecordsStruct rT;           // used in menu choice Team Records
   teamIndividualRecordsStruct rI; // used in menu choice Individual Records
   teamHistoryStruct h;            // used in menu choice History
   int minMatches;                 // used in menu choice Write Season Stats to File

   cout << endl; // blank line

   // get teamName, period from user
   titleRecord teamName; teamName.getFromUser(getTeamNameString);
   dateBeginEnd period;    period.getFromUser();

   // test whether team played in specified period, return if not
   if (!db.teamPlayed(teamName, period))
   {
      cout                                                                             << endl
           << "The database contains no record of a team named \"" << teamName << "\"" << endl
           << "in the period " << period << "."                                        << endl;
      return;
   }

   // get oppTeamName if necessary
   titleRecord oppTeamName; getOppTeamNameIfNecessary(db, teamName, oppTeamName, period);

   bool exit = false;
   while (!exit)
   {
      cout                                     << endl
           << "Indoor Cricket Database"        << endl
           << "------------------------------" << endl
           << "Team Stats Menu"                << endl
           << "(" << teamName;

      // print oppTeamName if necessary
      if (oppTeamName != titleRecord())
        cout << " (Vs " << oppTeamName << ")";

      cout << ")"                              << endl
           << period                           << endl
           << "------------------------------" << endl
           << "Rank Players ............ -> 1" << endl
           << "Rank Performances ....... -> 2" << endl
           << "Team Records ............... 3" << endl
           << "Individual Records ......... 4" << endl
           << "History .................... 5" << endl
           << "Match Record ............... 6" << endl
           << "List Player Names .......... 7" << endl
           << "Write Player Stats to File . 8" << endl
           << "Exit to Main Menu .......... 9" << endl
           << "------------------------------" << endl;

      std::cin.sync();

      switch (std::cin.get())
      {
       case '1': // menu choice = Rank Players
         menuTeamRankPlayers(teamName, oppTeamName, period, db);
         break;

       case '2': // menu choice = Rank Performances
         break;

       case '3': // menu choice = Team Records
         // compile team records
         cout << endl << "Compiling " << teamName << " team records...";
         db.compileTeamRecords(teamName, oppTeamName, period, rT);
         cout << "done." << endl;

         // print team records
         rT.print(cout);

         rT.clear(); // re-initialise struct in case this menu option is selected again
         break;

       case '4': // menu choice = Individual Records
         // compile team records
         cout << endl << "Compiling " << teamName << " team individual records...";
         db.compileTeamIndividualRecords(teamName, oppTeamName, period, rI);
         cout << "done." << endl;

         // print team records
         rI.print(cout);

         rI.clear(); // re-initialise struct in case this menu option is selected again
         break;

       case '5': // menu choice = History
         // compile team history
         cout << endl << "Compiling " << teamName << " history...";
         db.compileTeamHistory(teamName, oppTeamName, period, h);
         cout << "done." << endl;

         // print team history
         h.print(cout);

         h.clear(); // re-initialise struct in case this menu option is selected again
         break;

       case '6': // menu choice = Match Record
         cout << endl;

         // get date from user
         date.getFromUser("Match date? ");
           
         cout << endl;

         if (oppTeamName == titleRecord())
         {
            // all opposition teams are included

            if (db.teamPlayed(teamName, date))
              db.printMatchRecord(teamName, date, cout);
            else
              cout << "The database contains no match record for team \""
                   << teamName << "\" on date " << date << "." << endl;
         }
         else
         {
            // only one opposition team included

            if (db.teamPlayedOppTeam(teamName, oppTeamName, date))
              db.printMatchRecord(teamName, date, cout);
            else
              cout << "The database contains no record of a match between" << endl
                   << "\"" << teamName << "\" and \"" << oppTeamName
                   << "\" on date " << date << "."                         << endl;
         }
         break;

       case '7': // menu choice = List Player Names
         // compile player names list
         if (oppTeamName == titleRecord())
           // all opposition teams are included
           db.compilePlayersList(period, teamName, namesV);
         else
           // only one opposition team included
           db.compilePlayersList(period, teamName, oppTeamName, namesV);

         // print Player's Names List table
         cout                          << endl
              << "Player's Names List" << endl
              << "(" << teamName;

         // print oppTeamName if necessary
         if (oppTeamName != titleRecord())
           cout << " (Vs " << oppTeamName << ")";

         cout << ")"                   << endl
              << period                << endl;
 
         printNamesVect(namesV, cout);

         break;

       case '8': // Menu choice = Write Season Stats to File
         cout << endl << getMinMatchesString;
         cin >> minMatches;
         writeSeasonStatsToFile(teamName, oppTeamName, period, db, minMatches);
         break;

       case '9': // menu choice = Exit
         exit = true;
         break;
      }
   }
}

extern rankingEnum ranking; // defined in "indcrkrecords.cpp"

/*
 *
 */
void menuTeamRankPlayers(const titleRecord  &teamName,
                         const titleRecord  &oppTeamName,
                         const dateBeginEnd &period,
                         const indCrkDB     &db          )
{
   using std::cin;
   using std::cout;
   using std::endl;

   int minMatches;
   cout << endl << getMinMatchesString;
   cin >> minMatches;

   // compile player statistics for team 'teamName'
   // (disregard minMatches for now, take into account when displaying tables)
   cout << endl << "Compiling " << teamName << " player statistics...";
   std::list<playerRecordsStruct> r; db.compilePlayerRecords(teamName, oppTeamName, period, r);
   cout << "done." << endl;

   bool exit = false;
   while (!exit)
   {
      cout                                      << endl
           << "Indoor Cricket Database"         << endl
           << "-------------------------------" << endl
           << "Player Rankings Menu"            << endl;

      cout << "(" << teamName;

      // print oppTeamName if necessary
      if (oppTeamName != titleRecord())
        cout << " (Vs " << oppTeamName << ")";

      cout << ")" << endl;

      // print minMatches if necessary
      if (minMatches != 1)
        cout << "(Minimum " << minMatches << " matches)" << endl;

      cout << period                            << endl
           << "-------------------------------" << endl
           << "Rank By What Criteria?"          << endl
           << "-------------------------------" << endl
           << "Averages ................. -> 1" << endl
           << "Totals ................... -> 2" << endl
           << "Best Performances ........ -> 3" << endl
           << "Worst Performances ....... -> 4" << endl
           << "Exit to Previous Menu ....... 5" << endl
           << "-------------------------------" << endl;

      std::cin.sync();

      switch (std::cin.get())
      {
       case '1': menuTeamRankPlayerAverages(teamName, oppTeamName, period, db, r, minMatches);
                 break;
       case '2': menuTeamRankPlayerTotals(teamName, oppTeamName, period, db, r, minMatches);
                 break;
       case '3': menuTeamRankPlayerBests(teamName, oppTeamName, period, db, r, minMatches);
                 break;
       case '4': menuTeamRankPlayerWorsts(teamName, oppTeamName, period, db, r, minMatches);
                 break;
       case '5': r.clear(); // clear player records list
                 exit = true;
                 break;
      }
   }
}

/*
 *
 */
void menuTeamRankPlayerAverages(const titleRecord              &teamName,
                                const titleRecord              &oppTeamName,
                                const dateBeginEnd             &period,
                                const indCrkDB                 &db,
                                std::list<playerRecordsStruct> &r,
                                const int                      &minMatches  )
{
   using std::cout;
   using std::endl;

   bool exit = false;
   while (!exit)
   {
      cout                                      << endl
           << "Indoor Cricket Database"         << endl
           << "-------------------------------" << endl
           << "Player Rankings (Averages) Menu" << endl;

      cout << "(" << teamName;

      // print oppTeamName if necessary
      if (oppTeamName != titleRecord())
        cout << " (Vs " << oppTeamName << ")";

      cout << ")" << endl;

      // print minMatches if necessary
      if (minMatches > 1)
        cout << "(Minimum " << minMatches << " matches)" << endl;

      cout << period                            << endl
           << "-------------------------------" << endl
           << "Rank Players By Average What?"   << endl
           << "-------------------------------" << endl
           << "Runs Per Innings ............ 1" << endl
           << "Wickets Per Over ............ 2" << endl
           << "Runs Conceded Per Over ...... 3" << endl
           << "Match Contribution .......... 4" << endl
           << "Exit to Previous Menu ....... 5" << endl
           << "-------------------------------" << endl;

      std::cin.sync();

      switch (std::cin.get())
      {
       case '1': ranking = avgRunsScrdPerInns;  break;
       case '2': ranking = avgWktsTakenPerOver; break;
       case '3': ranking = avgRunsConcPerOver;  break;
       case '4': ranking = avgMatchContr;       break;
       case '5': exit = true;                   break;
      }

      if (!exit)
      {
         r.sort(std::greater<class playerRecordsStruct>());
         printRankingsTable(r, teamName, oppTeamName, period, minMatches, cout);
      }
   }
}

/*
 *
 */
void menuTeamRankPlayerTotals(const titleRecord              &teamName,
                              const titleRecord              &oppTeamName,
                              const dateBeginEnd             &period,
                              const indCrkDB                 &db,
                              std::list<playerRecordsStruct> &r,
                              const int                      &minMatches  )
{
   using std::cout;
   using std::endl;

   bool exit = false;
   while (!exit)
   {
      cout                                     << endl
           << "Indoor Cricket Database"        << endl
           << "------------------------------" << endl
           << "Player Rankings (Totals) Menu"  << endl;

      cout << "(" << teamName;

      // print oppTeamName if necessary
      if (oppTeamName != titleRecord())
        cout << " (Vs " << oppTeamName << ")";

      cout << ")" << endl;

      // print minMatches if necessary
      if (minMatches > 1)
        cout << "(Minimum " << minMatches << " matches)" << endl;

      cout << "------------------------------" << endl
           << "Rank Players By Total What?"    << endl
           << "------------------------------" << endl
           << "Matches Played ............. 1" << endl
           << "Innings Batted ............. 2" << endl
           << "Overs Bowled ............... 3" << endl
           << "Runs Scored ................ 4" << endl
           << "Wickets Taken .............. 5" << endl
           << "Runs Conceded .............. 6" << endl
           << "Exit to Previous Menu ...... 7" << endl
           << "------------------------------" << endl;

      std::cin.sync();
      switch (std::cin.get())
      {
       case '1': ranking = matchesPlayed; break;
       case '2': ranking = innsBatted;    break;
       case '3': ranking = oversBowled;   break;
       case '4': ranking = runsScored;    break;
       case '5': ranking = wicketsTaken;  break;
       case '6': ranking = runsConceded;  break;
       case '7': exit = true;             break;
      }

      if (!exit)
      {
         r.sort(std::greater<class playerRecordsStruct>());
         printRankingsTable(r, teamName, oppTeamName, period, minMatches, cout);
      }
   }
}

/*
 *
 */
void menuTeamRankPlayerBests(const titleRecord              &teamName,
                             const titleRecord              &oppTeamName,
                             const dateBeginEnd             &period,
                             const indCrkDB                 &db,
                             std::list<playerRecordsStruct> &r,
                             const int                      &minMatches  )
{
   using std::cout;
   using std::endl;

   bool exit = false;
   while (!exit)
   {
      cout                                     << endl
           << "Indoor Cricket Database"        << endl
           << "------------------------------" << endl
           << "Player Rankings (Bests) Menu"   << endl;

      cout << "(" << teamName;

      // print oppTeamName if necessary
      if (oppTeamName != titleRecord())
        cout << " (Vs " << oppTeamName << ")";

      cout << ")" << endl;

      // print minMatches if necessary
      if (minMatches > 1)
        cout << "(Minimum " << minMatches << " matches)" << endl;

      cout << "------------------------------" << endl
           << "Rank Players By Best What?"     << endl
           << "------------------------------" << endl
           << "Innings .................... 1" << endl
           << "Over ....................... 2" << endl
           << "Match Bowling Figures ...... 3" << endl
           << "Match Contribution ......... 4" << endl
           << "Exit to Previous Menu ...... 5" << endl
           << "------------------------------" << endl;

      std::cin.sync();
      switch (std::cin.get())
      {
       case '1': ranking = bestInns;       break;
       case '2': ranking = bestOver;       break;
       case '3': ranking = bestBowlFigs;   break;
       case '4': ranking = bestMatchContr; break;
       case '5': exit = true;              break;
      }

      if (!exit)
      {
         r.sort(std::greater<class playerRecordsStruct>());
         printRankingsTable(r, teamName, oppTeamName, period, minMatches, cout);
      }
   }
}

/*
 *
 */
void menuTeamRankPlayerWorsts(const titleRecord              &teamName,
                              const titleRecord              &oppTeamName,
                              const dateBeginEnd             &period,
                              const indCrkDB                 &db,
                              std::list<playerRecordsStruct> &r,
                              const int                      &minMatches  )
{
   using std::cout;
   using std::endl;

   bool exit = false;
   while (!exit)
   {
      cout                                     << endl
           << "Indoor Cricket Database"        << endl
           << "------------------------------" << endl
           << "Player Rankings (Worsts) Menu"  << endl;

      cout << "(" << teamName;

      // print oppTeamName if necessary
      if (oppTeamName != titleRecord())
        cout << " (Vs " << oppTeamName << ")";

      cout << ")" << endl;

      // print minMatches if necessary
      if (minMatches > 1)
        cout << "(Minimum " << minMatches << " matches)" << endl;

      cout << "------------------------------" << endl
           << "Rank Players By Worst What?"    << endl
           << "------------------------------" << endl
           << "Innings .................... 1" << endl
           << "Over ....................... 2" << endl
           << "Match Bowling Figures ...... 3" << endl
           << "Match Contribution ......... 4" << endl
           << "Exit to Previous Menu ...... 5" << endl
           << "------------------------------" << endl;

      std::cin.sync();
      switch (std::cin.get())
      {
       case '1': ranking = wrstInns;       break;
       case '2': ranking = wrstOver;       break;
       case '3': ranking = wrstBowlFigs;   break;
       case '4': ranking = wrstMatchContr; break;
       case '5': exit = true;              break;
      }

      if (!exit)
      {
         r.sort(std::greater<class playerRecordsStruct>());
         printRankingsTable(r, teamName, oppTeamName, period, minMatches, cout);
      }
   }
}

/*
 *
 */
void menuMaintainDB(indCrkDB &db)
{
   using std::cout;
   using std::endl;

   titleRecord  teamName,
                oldTeamName,
                newTeamName;
   dateBeginEnd period;
   dateRecord   date;
   nameRecord   oldName,
                newName;


   std::vector<titleRecord> teamNamesV;

   bool exit = false;
   while (!exit)
   {
      cout                                     << endl
           << "Indoor Cricket Database"        << endl
           << "------------------------------" << endl
           << "Maintain Database Menu"         << endl
           << "------------------------------" << endl
           << "Check DB Integrity ......... 1" << endl
           << "List Team Names ............ 2" << endl
           << "Edit Match Record .......... 3" << endl
           << "Delete Match Record ........ 4" << endl
           << "Update Player Names ........ 5" << endl
           << "Update Team Names .......... 6" << endl
           << "Update Opposition Team Names 7" << endl
           << "Exit to Main Menu .......... 8" << endl
           << "------------------------------" << endl;

      std::cin.sync();

      switch (std::cin.get())
      {
       case '1': // menu choice = Check DB Integrity
         db.checkDBintegrityAndPrintResults();
         break;

       case '2': // menu choice = List Team Names
         cout << endl;

         // get period
         period.getFromUser();

         // compile team names list
         db.compileTeamsList(period, teamNamesV);

         if (teamNamesV.size() > 0)
         {
            cout                        << endl
                 << "Teams List"        << endl
                 << "Period " << period << endl;

            printTitlesVect(teamNamesV, cout);
         }
         else
           cout                                                                 << endl
                << "The database contains no match records in the time period " 
                << period << "."                                                << endl;

         break;

       case '3': // menu choice = Edit Match Record
         cout << endl;

         // get match date, team name
         date.getFromUser(getMatchDateString);
         teamName.getFromUser(getTeamNameString);

         cout << endl;

         // get user to edit match record
         if (db.teamPlayed(teamName, date))
         {
            db.getUserToEditMatchRecord(date, teamName);
            updateDBfile(db);
         }
         else
           cout << "The database contains no match record for team \""
                << teamName << "\" on date " << date << "." << endl;
         break;

       case '4': // menu choice = Delete Match Record
         break;

       case '5': // menu choice = Update Player Names
         cout << endl;

         // get teamName, period, old & new names
         teamName.getFromUser(getTeamNameString);
         period.getFromUser();
         oldName.getFromUser(getOldNameString);
         newName.getFromUser(getNewNameString);

         if (oldName != newName)
         {
            // update player names in database
            db.updatePlayerNames(period, teamName, oldName, newName);
            updateDBfile(db);
         }
         break;

       case '6': // menu choice = Update Team Names
         cout << endl;

         // get period, old & new names
         period.getFromUser();
         oldName.getFromUser(getOldNameString);
         newName.getFromUser(getNewNameString);

         if (oldTeamName != newTeamName)
         {
            // update team names in database
            db.updateTeamNames(period, oldTeamName, newTeamName);
            updateDBfile(db);
         }
         break;

       case '7': // menu choice = Update Opposition Team Names
         cout << endl;

         // get teamName, period, old & new oppTeam names
         teamName.getFromUser(getTeamNameString);
         period.getFromUser();
         oldTeamName.getFromUser(getOldTeamNameString);
         newTeamName.getFromUser(getNewTeamNameString);

         if (oldTeamName != newTeamName)
         {
            // update opposition team names in database
            db.updateOppTeamNames(period, teamName, oldTeamName, newTeamName);
            updateDBfile(db);
         }
         break;

       case '8': // menu choice = Exit to Main Menu
         exit = true;
         break;
      }
   }
}

// FILE SCOPE FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////

namespace
{
   void getOppTeamNameIfNecessary(const indCrkDB &db,
                                  titleRecord    &teamName,
                                  titleRecord    &oppTeamName,
                                  dateBeginEnd   &period      )
   {
      using std::cout;
      using std::endl;

      cout << endl; // blank line
      bool considerAllOppTeams; getBoolFromUser(considerAllOppTeams, includeAllTeamsString);
      if (!considerAllOppTeams)
      {
         cout << endl; // blank line
         oppTeamName.getFromUser(getOppTeamNameString);

         if (!db.oppTeamPlayed(oppTeamName, period))
         {
            cout                                                                       << endl
                 << "The database contains no records of matches between " << teamName << endl
                 << "and " << oppTeamName << " in the period " << period << "."        << endl
                 << "Records against all opposition teams will be included."           << endl;

            // reset oppTeamName to default (indicates that all opp. teams should be included)
            oppTeamName = titleRecord();
         }
      }
   }
}

/*****************************************END*OF*FILE*********************************************/
