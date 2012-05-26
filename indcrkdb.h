/*************************************************************************************************\
*                                                                                                 *
* "IndCrkDB.h" -                                                                                  *
*                                                                                                 *
*       Author - Tom McDonnell 2005                                                               *
*                                                                                                 *
\*************************************************************************************************/

#ifndef IND_CRK_DB_H
#define IND_CRK_DB_H

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "IndCrk.h"
#include "IndCrkRecords.h"

#include <string>
#include <list>
#include <algorithm>

// TYPE DEFINITIONS ///////////////////////////////////////////////////////////////////////////////

/*
 *
 */
class indCrkDB
{
 public:
   // constructor
   indCrkDB(const std::string &fName): fileName(fName) {}

   // file I/O functions
   bool readFromFile(void);
   bool writeToFile(void) const;

   // integrity check function
   void checkDBintegrityAndPrintResults(void) const;

   // print functions
   void printFileName(std::ostream &out) const {out << fileName;}
   void printMatchRecord(const titleRecord &, const dateRecord &, std::ostream &) const;

   // database manipulation functions
   void insertMatchRecord(const matchRecord &);
   void deleteMatchRecord(const dateRecord &, const titleRecord &teamName);
   bool getUserToEditMatchRecord(const dateRecord &, const titleRecord &teamName);
   void sortByDate(void) {db.sort(/*matchDateCmpstd::greater<matchRecord>()*/);}

   // statistical functions //
   void compilePlayerRecords(const titleRecord              &teamName,
                             const titleRecord              &oppTeamName,
                             const dateBeginEnd             &period,
                             std::list<playerRecordsStruct> &playerRecordsList) const;
   void compilePlayerRecords(const nameRecord    &playerName,
                             const titleRecord   &teamName,
                             const titleRecord   &oppTeamName,
                             const dateBeginEnd  &period,
                             playerRecordsStruct &playerRecords) const;
   void compilePlayerHistory(const nameRecord    &playerName,
                             const titleRecord   &teamName,
                             const titleRecord   &oppTeamName,
                             const dateBeginEnd  &period,
                             playerHistoryStruct &playerHistory) const;
   void compileTeamRecords(const titleRecord  &teamName,
                           const titleRecord  &oppTeamName,
                           const dateBeginEnd &period,
                           teamRecordsStruct  &teamRecords) const;
   void compileTeamHistory(const titleRecord  &teamName,
                           const titleRecord  &oppTeamName,
                           const dateBeginEnd &period,
                           teamHistoryStruct  &teamHistory) const;
   void compileTeamIndividualRecords(const titleRecord  &teamName,
                                     const titleRecord  &oppTeamName,
                                     const dateBeginEnd &period,
                                     teamIndividualRecordsStruct  &r) const;

   // simple queries
   bool playerPlayed(const nameRecord &, const titleRecord &, const dateRecord   &) const;
   bool playerPlayed(const nameRecord &, const titleRecord &, const dateBeginEnd &) const;
   bool playerPlayed(const nameRecord &, const titleRecord &) const;
   bool teamPlayed(const titleRecord &, const dateRecord   &) const;
   bool teamPlayed(const titleRecord &, const dateBeginEnd &) const;
   bool teamPlayed(const titleRecord &) const;
   bool teamPlayedOppTeam(const titleRecord &, const titleRecord &, const dateRecord   &) const;
   bool teamPlayedOppTeam(const titleRecord &, const titleRecord &, const dateBeginEnd &) const;
   bool teamPlayedOppTeam(const titleRecord &, const titleRecord &) const;
   bool oppTeamPlayed(const titleRecord &, const dateRecord   &) const;
   bool oppTeamPlayed(const titleRecord &, const dateBeginEnd &) const;
   bool oppTeamPlayed(const titleRecord &) const;

   // database maintenance functions
   void compileTeamsList(const dateBeginEnd &, std::vector<titleRecord> &) const;
   void compilePlayersList(const dateBeginEnd &, std::vector<nameRecord> &) const;
   void compilePlayersList(const dateBeginEnd      &,
                           const titleRecord       &teamName,
                           std::vector<nameRecord> &         ) const;
   void compilePlayersList(const dateBeginEnd      &,
                           const titleRecord       &teamName,
                           const titleRecord       &oppTeamName,
                           std::vector<nameRecord> &            ) const;
   void compileOppTeamsList(const dateBeginEnd &, std::vector<titleRecord> &) const;
   void compileOppTeamsList(const dateBeginEnd &,
                            const titleRecord &teamName, std::vector<titleRecord> &) const;
   void updatePlayerNames(const dateBeginEnd &period,  const titleRecord &teamName,
                          const nameRecord   &oldName, const nameRecord  &newName  );
   void updateTeamNames(const dateBeginEnd &, const titleRecord &oldN, const titleRecord &newN);
   void updateOppTeamNames(const dateBeginEnd &,    const titleRecord &teamName,
                           const titleRecord  &oldN, const titleRecord &newN);

 private:
   // private member functions
   std::list<matchRecord>::const_iterator constFirstMatchWithDate(const dateRecord &) const;
   std::list<matchRecord>::iterator            firstMatchWithDate(const dateRecord &);
   void insertSorted(const matchRecord &);

   // private variables (require integrity protection mechanisms)
   const std::string fileName;
   std::list<matchRecord> db;
};

#endif

/*****************************************END*OF*FILE*********************************************/
