/*************************************************************************************************\
*                                                                                                 *
* "IndCrkDBui.cpp" - Indoor Cricket Database User Interface.                                      *
*                                                                                                 *
*           Author - Tom McDonnell 2005                                                           *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "IndCrkDBui.h"
#include "IndCrkDB.h"
#include "IndCrkRecords.h"

#include <TomsLibrary\misc.h>

#include <algorithm>
#include <fstream>

// FUNCTION DEFINITIONS ///////////////////////////////////////////////////////////////////////////

/*
 *
 */
bool updateDBfile(const indCrkDB &db)
{
   using std::cout;
   using std::endl;

   cout                                                     << endl
        << "Updating file \""; db.printFileName(cout); cout << "\"...";

   if (db.writeToFile())
   {
      cout << "done." << endl;

      return true;
   }
   else
   {
      cout << "File \""; db.printFileName(cout); cout << " could not be written to." << endl;

      return false;
   }
}

/*
 *
 */
void writeSeasonStatsToFile(const titleRecord  &teamName,
                            const titleRecord  &oppTeamName,
                            const dateBeginEnd &period,
                            const indCrkDB     &db,
                            const int          &minMatches  )
{
   using std::cout;
   using std::endl;

   cout << endl;

   // get fileName from user
   std::string fileName;
   cout << "File name? "; std::cin >> fileName;

   // compile player names list
   std::vector<nameRecord> uNames; db.compilePlayersList(period, teamName, uNames);

   // declare records and history structures
   playerRecordsStruct playerRecords;
   playerHistoryStruct playerHistory;

   // open file
   std::ofstream file(fileName.c_str(), std::ios_base::trunc);

   // compile and write playerRecords and playerHistory to file
   std::vector<nameRecord>::const_iterator namePtr;
   for (namePtr = uNames.begin(); namePtr != uNames.end(); ++namePtr)
   {
      playerRecords.clear();
      playerHistory.clear();

      db.compilePlayerRecords(*namePtr, teamName, oppTeamName, period, playerRecords);
      db.compilePlayerHistory(*namePtr, teamName, oppTeamName, period, playerHistory);

      if (playerRecords.matchesPlayed >= minMatches)
      {
         playerRecords.print(file);
         playerHistory.print(file);
      }
   }
}

// functions available from 'Maintain Database Menu' //

/*
 *
 */
void printNamesVect(const std::vector<nameRecord> &v, std::ostream &out)
{
   using std::cout;
   using std::endl;

   if (v.size() != 0)
   {
      // find width of longest first name
      int i,
          w = 0;
      for (i = 0; i < v.size(); ++i)
        w = max(w, static_cast<int>(v[i].getFirstName().length()));

      // print unique player names list
          // assume heading already printed
      out << "---------------------------------" << endl;
      for (i = 0; i < v.size(); ++i)
      {
         v[i].printFormatted(cout, w);
         out << endl;
      }
      out << "---------------------------------" << endl;
   }
}

/*
 *
 */
void printTitlesVect(const std::vector<titleRecord> &v, std::ostream &out)
{
   using std::cout;
   using std::endl;

   if (v.size() != 0)
   {
      // print unique team names list
          // assume heading already printed (so can be used for both teamNames and oppTeamNames)
      out << "---------------------------------"    << endl;
      for (int i = 0; i < v.size(); ++i)
        out << v[i].get() << endl;

      out << "---------------------------------"    << endl;
   }
}

/*****************************************END*OF*FILE*********************************************/
