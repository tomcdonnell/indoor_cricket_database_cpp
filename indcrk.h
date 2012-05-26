/*************************************************************************************************\
*                                                                                                 *
* "IndCrk.h" -                                                                                    *
*                                                                                                 *
*     Author - Tom McDonnell 2005                                                                 *
*                                                                                                 *
\*************************************************************************************************/

#ifndef IND_CRK_H
#define IND_CRK_H

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "IndCrkRecords.h"

#include <TomsLibrary/dateRecord.h>
#include <TomsLibrary/nameRecord.h>
#include <TomsLibrary/titleRecord.h>
#include <TomsLibrary/restrictedRangeInt.h>
#include <TomsLibrary/misc.h>

#include <vector>

// CLASS DEFINITION ///////////////////////////////////////////////////////////////////////////////

/*
 *
 */
class battingRecord
{
 public:
   // public variables (have own integrity protection mechanisms or need none)
   nameRecord         batsmansName;
   restrictedRangeInt runsScored;

   // constructors
   battingRecord(void);
   battingRecord(const int &r);
   battingRecord(const std::string &f, const int &r);
   battingRecord(const std::string &f, const std::string &l, const int &r);

   // user input function
   void getFromUser(const std::string &namePrompt, const std::string &scorePrompt);
};

// INLINE MEMBER FUNCTION DEFINITIONS /////////////////////////////////////////////////////////////

inline battingRecord::battingRecord(void)
: batsmansName(), runsScored(indCrkRecords::minPossibleRunsScoredPerInns,
                             indCrkRecords::maxPossibleRunsScoredPerInns, 0)
{}

inline battingRecord::battingRecord(const int &r)
: batsmansName(), runsScored(indCrkRecords::minPossibleRunsScoredPerInns,
                             indCrkRecords::maxPossibleRunsScoredPerInns, r)
{}

inline battingRecord::battingRecord(const std::string &f, const int &r)
: batsmansName(f), runsScored(indCrkRecords::minPossibleRunsScoredPerInns,
                              indCrkRecords::maxPossibleRunsScoredPerInns, r)
{}

inline battingRecord::battingRecord(const std::string &f, const std::string &l, const int &r)
: batsmansName(f, l), runsScored(indCrkRecords::minPossibleRunsScoredPerInns,
                                 indCrkRecords::maxPossibleRunsScoredPerInns, r)
{}

inline void battingRecord::getFromUser(const std::string &namePrompt,
                                       const std::string &scorePrompt)
{
   batsmansName.getFromUser(namePrompt);
   runsScored.getFromUser(scorePrompt);
}

// INLINE FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

inline std::ostream &operator<<(std::ostream &out, const battingRecord &b)
{
   out << b.batsmansName << " " << b.runsScored;

   return out;
}

inline std::istream &operator>>(std::istream &in, battingRecord &b)
{
   in >> b.batsmansName
      >> b.runsScored;

   return in;
}

// CLASS DEFINITION ///////////////////////////////////////////////////////////////////////////////

/*
 *
 */
class bowlingRecord
{
 public:
   // exception class
   class bowlingRecordErr {};

   // public variables (have own integrity protection mechanisms or need none)
   nameRecord         bowlersName;
   restrictedRangeInt wicketsTaken,
                      runsConceded;

   // constructors
   bowlingRecord(void);
   bowlingRecord(const std::string &f, const int &w, const int &r);
   bowlingRecord(const std::string &f, const std::string &l, const int &w, const int &r);

   // user input function
   void getFromUser(const std::string &namePrompt,
                    const std::string &wicketsPrompt,
                    const std::string &runsPrompt    );
};

// INLINE MEMBER FUNCTION DEFINITIONS /////////////////////////////////////////////////////////////

inline bowlingRecord::bowlingRecord(void)
: bowlersName(), wicketsTaken(0, indCrkRecords::maxPossibleWicketsTakenPerOver, 0),
                 runsConceded(indCrkRecords::minPossibleRunsConcededPerOver,
                              indCrkRecords::maxPossibleRunsConcededPerOver, 0)
{}

inline bowlingRecord::bowlingRecord(const std::string &f, const int &w, const int &r)
: bowlersName(f), wicketsTaken(0, indCrkRecords::maxPossibleWicketsTakenPerOver, 0),
                  runsConceded(indCrkRecords::minPossibleRunsConcededPerOver,
                               indCrkRecords::maxPossibleRunsConcededPerOver, 0)
{}

inline bowlingRecord::bowlingRecord(const std::string &f,
                                    const std::string &l, const int &w, const int &r)
: bowlersName(f, l), wicketsTaken(0, indCrkRecords::maxPossibleWicketsTakenPerOver, 0),
                     runsConceded(indCrkRecords::minPossibleRunsConcededPerOver,
                                  indCrkRecords::maxPossibleRunsConcededPerOver, 0)
{}

inline void bowlingRecord::getFromUser(const std::string &namePrompt,
                                       const std::string &wicketsPrompt,
                                       const std::string &runsPrompt    )
{
   bowlersName.getFromUser(namePrompt);
   wicketsTaken.getFromUser(wicketsPrompt);
   runsConceded.getFromUser(runsPrompt);
}

// INLINE FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

inline std::ostream &operator<<(std::ostream &out, const bowlingRecord &b)
{
   out << b.bowlersName << " " << b.wicketsTaken << "/" << b.runsConceded;

   return out;
}

inline std::istream &operator>>(std::istream &in, bowlingRecord &b)
{
   in >> b.bowlersName
      >> b.wicketsTaken
      >> b.runsConceded;

   return in;
}

// CLASS DEFINITION ///////////////////////////////////////////////////////////////////////////////

/*
 *
 */
class matchRecord
{
 public:
   // static constant declarations
   static const int nPlayersPerTeam,
                    nOversPerInnings;

   // exception classes
   class matchRecordErr {};

   // constructors
   matchRecord(void);

   // get functions
   dateRecord    getDate(void)                 const  {return date;       }
   titleRecord   getTeamName(void)             const  {return teamName;   }
   titleRecord   getOppTeamName(void)          const  {return oppTeamName;}
   battingRecord getBattingRec(int battingPos) const;
   bowlingRecord getBowlingRec(int over)       const;
   int           getTeamPenalty(void)          const  {return teamPenalty;   }
   int           getOppTeamPenalty(void)       const  {return oppTeamPenalty;}
   int           getTeamScore(void)            const  {return teamScore;     }
   int           getOppTeamScore(void)         const  {return oppTeamScore;  }
   int           getTeamWicketTally(void)      const;

   // simple query
   bool battedFirst(void) const {return batted1st;}

   // no set functions, use getUserToEditData()

   // print functions
   void printRecord(std::ostream &) const;
   void printResult(std::ostream &) const;

   // user input & edit functions
   void getDataFromUser(void);
   void getUserToEditData(void);

   // file I/O functions
   bool readFromFile(std::ifstream &, int &lineNo);
   bool writeToFile(std::ofstream &) const;

   // integrity testing functions
   bool checkDataIntegrityAndPrintResults(void) const;
   bool teamScoreEqSumBattingScoresMinusPenalty(int &sum)   const;
   bool oppTeamScoreEqSumRunsConcededMinusPenalty(int &sum) const;

   // matchRecord maintenance functions
   void findUniquePlayerNames(std::vector<nameRecord> &) const;
   void replacePlayerName(const nameRecord &oldName, const nameRecord &newName);
   void replaceTeamName(const titleRecord &newTeamName)       {teamName    = newTeamName;   }
   void replaceOppTeamName(const titleRecord &newOppTeamName) {oppTeamName = newOppTeamName;}
   bool playerPlayed(const nameRecord &) const;

   // statistical functions
   void getPlayerStats(const nameRecord &, playerMatchStats &)    const;
   void getPlayerStats(const nameRecord &, playerRecordsStruct &) const;
   void getPlayerStats(teamRecordsStruct &)                       const;
   void getTeamStats(teamMatchStats &)    const;
   void getTeamStats(teamRecordsStruct &) const;
   void getBestBattPship(std::list<playerInnsRecord> &, std::list<playerInnsRecord> &) const;
   void getWrstBattPship(std::list<playerInnsRecord> &, std::list<playerInnsRecord> &) const;
   void testFunc(playerInnsRecord &);

 private:
   // private variables (need integrity protection mechanisms)

   bool batted1st;

   // PROBLEM: need time as well as date as teams can play more than one game per day
   dateRecord  date;         // These variables have been made private so that a complete
   titleRecord teamName,     //  match mecord is created at one time.  If an error is made
               oppTeamName;  //  when the match data is input, must use editMatchRecord().

   battingRecord *battingRec;  // should contain nPlayersPerTeam unique players names
   bowlingRecord *bowlingRec;  // should contain nPlayersPerTeam unique players names

   restrictedRangeInt teamPenalty,    // runs penalty for incorrect uniform for example
                      oppTeamPenalty, // runs penalty for incorrect uniform for example
                      teamScore,      // must equal sum runs scored   (battingRec[]) - teamPenalty
                      oppTeamScore; // must equal sum runs conceded (bowlingRec[]) - oppTeamPenalty
};

// object-functions concerning class matchRecord (for use with standard library algorithms) //

// for use in sorting list of matchRecords
//struct matchDateCmp//std::greater<class matchRecord>
//{
   inline bool matchDateCmp(const matchRecord &m1, const matchRecord &m2)
   {
      return (m1.getDate() < m2.getDate()); // sign is intentionally less than
                                            // because don't know how to sort by 'less' function
   }
//};

// INLINE MEMBER FUNCTION DEFINITIONS /////////////////////////////////////////////////////////////

inline bool operator<(const matchRecord &m1, const matchRecord &m2)
{
   return (m1.getDate() < m2.getDate());
}

inline bool operator>(const matchRecord &m1, const matchRecord &m2)
{
   return (m1.getDate() > m2.getDate());
}

inline battingRecord matchRecord::getBattingRec(int battingPos) const
{
   if (0 <= battingPos && battingPos <= nPlayersPerTeam)
     return battingRec[battingPos];
   else
     error("Attempted to access battingRec array out of range.");

   return battingRec[0]; // to avoid compiler warning
}

inline bowlingRecord matchRecord::getBowlingRec(int overNo) const
{
   if (0 <= overNo && overNo <= nOversPerInnings)
     return bowlingRec[overNo];
   else
     error("Attempted to access bowlingRec array out of range.");

   return bowlingRec[0]; // to avoid compiler warning
}

inline int matchRecord::getTeamWicketTally(void) const
{
   int w = 0;
   for (int i = 0; i < nOversPerInnings; ++i)
     w += getBowlingRec(i).wicketsTaken;

   return w;
}

#endif

/*****************************************END*OF*FILE*********************************************/
