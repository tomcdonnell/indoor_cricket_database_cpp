/*************************************************************************************************\
*                                                                                                 *
* "IndCrkRecords.h" - Container classes for keeping track of best/worst player and team           *
*                     performances.                                                               *
*                                                                                                 *
*            Author - Tom McDonnell 2005                                                          *
*                                                                                                 *
\*************************************************************************************************/

#ifndef IND_CRK_RECORDS_H
#define IND_CRK_RECORDS_H

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include <TomsLibrary/dateRecord.h>
#include <TomsLibrary/nameRecord.h>
#include <TomsLibrary/titleRecord.h>
#include <TomsLibrary/restrictedRangeInt.h>

#include <list>
#include <vector>

// FUNCTION DECLARATIONS //////////////////////////////////////////////////////////////////////////

double matchContribution(const int &innsBatted,  const int &runsScored,
                         const int &oversBowled, const int &runsConceded);

// CONSTANT DEFINITIONS ///////////////////////////////////////////////////////////////////////////

namespace indCrkRecords
{
   const int minPossibleRunsScoredPerInns = -122;
     // (assuming: 6 balls per over, 4 overs per batting pair,
     //            -5 run wicket penalty, 2 run no-ball penalty)
     // lowest possible score = -122
     //  explanation:
     //   first over:  lose wicket without changing ends every ball, then must swap ends
     //                score for over = -30
     //   second over: (no-balls are not rebowled so must miss first ball) regain strike
     //                after first ball and lose wicket without changing ends every ball
     //                score for over = -25
     //   third over:  (assuming batting in last pair) same as second over but score one
     //                run on last ball so as to be on strike strike for entire last over
     //                score for over = -19
     //   last over:   (assuming batting in last pair) no-balls are rebowled (once) in last
     //                over so can lose wicket twice for each delivery.
     //                score for over = -48

   const int maxPossibleRunsScoredPerInns = 297;
     // highest possible score = 297 (assuming only 1 extra run per ball)
     //  explanation:
     //   first 3 overs: for each ball:
     //                  no-ball penalty (2) + six and run (7) + extra run to regain strike (1)
     //                  (no extra run on last ball so regain strike after change of ends)
     //                  score for over = 59
     //   last over:     (assuming batting in last pair) no-balls are rebowled (once) in last
     //                  over so can score 20 runs per delivery
     //                  score for over = 120

   const int maxPossibleWicketsTakenPerOver = 12;
     // (assuming 6 balls per over and over is last over, so no-balls are rebowled once)
     // for each delivery: wicket, then another on rebowl

   const int minPossibleRunsConcededPerOver = -48;
     // (assuming: 6 balls per over, -5 run wicket penalty, 2 run no-ball penalty)
     // lowest possible runsConceded = -48
     //  explanation:
     //   (assuming over is last over, so no-balls are rebowled once)
     //   for each delivery: no-ball + wicket, then wicket on rebowl

   const int maxPossibleRunsConcededPerOver = 120;
     // highest possible runsConceded = 120 (assuming only 1 extra run per ball)
     //  explanation:
     //   (assuming over is last over, so no-balls are rebowled once)
     //   for each delivery: no-ball + six and run + extra run, same on rebowl
     // (assuming 6 balls per over and bowling last over, so no-balls are rebowled once)
     //   for each delivery: no-ball + wicket, then wicket on rebowl

} // end namespace indCrkRecords

// FILE SCOPE TYPE DEFINITIONS ////////////////////////////////////////////////////////////////////

namespace
{

/*
 * Common to both player and team record classes.
 */
class oppTeamAndDate
{
 public:
   // constructor
   oppTeamAndDate(void) {}
   oppTeamAndDate(const titleRecord &o, const dateRecord &d): oppTeamName(o), date(d) {}

   void print(std::ostream &) const;

   titleRecord oppTeamName;
   dateRecord  date;
};

inline void oppTeamAndDate::print(std::ostream &o) const
{
   o << "  (Vs " << oppTeamName << ", " << date << ")" << std::endl;
}

// PLAYER RECORDS CLASSES //

/*
 * Class for recording details of a player's best/worst innings batted.
 */
class playerInnsRecord
{
 public:
   // constructors
   playerInnsRecord(const int &r)
   : runsScored(r)
   {}

   playerInnsRecord(const int &r,
                    const nameRecord &n, const titleRecord &o, const dateRecord &d)
   : runsScored(r), playerName(n), match(o, d)
   {}

   // print function
   void print(std::ostream &) const;

   int            runsScored;
   oppTeamAndDate match;
   nameRecord     playerName;
};

inline void playerInnsRecord::print(std::ostream &o) const
{
   o << "  " << playerName;

   match.print(o);
}

/*
 * Class for recording details of a players best/worst over bowled.
 */
class playerOverRecord
{
 public:
   // constructors
   playerOverRecord(const int &r): runsConceded(r), wicketsTaken(0) {}
   playerOverRecord(const int &w, const int &r,
                    const nameRecord &n, const titleRecord &o, const dateRecord &d)
   : wicketsTaken(w), runsConceded(r), playerName(n), match(o, d)
   {}

   // print function
   void print(std::ostream &) const;

   int            wicketsTaken,
                  runsConceded;
   oppTeamAndDate match;
   nameRecord     playerName;
};

inline void playerOverRecord::print(std::ostream &o) const
{
   o << "  " << playerName;

   match.print(o);
}

/*
 * Class for recording details of a players best/worst bowling figures in a match.
 */
class playerBowlFigsRecord
{
 public:
   // constructors
   playerBowlFigsRecord(const double &w, const double &r): wicketsTaken(w), runsConceded(r) {}
   playerBowlFigsRecord(const double &w, const double &r,
                        const nameRecord &n, const titleRecord &o, const dateRecord &d)
   : wicketsTaken(w), runsConceded(r), playerName(n), match(o, d)
   {}

   // print function
   void print(std::ostream &) const;

   double         wicketsTaken,
                  runsConceded;
   oppTeamAndDate match;
   nameRecord     playerName;
};

inline void playerBowlFigsRecord::print(std::ostream &o) const
{
   o << "  " << playerName;

   match.print(o);
}

/*
 *
 */
class playerMatchContrRecord
{
 public:
   // constructors
   playerMatchContrRecord(const double &m): matchContr(m) {}
   playerMatchContrRecord(const double &m, const nameRecord &n,
                          const titleRecord &o, const dateRecord &d)
   : matchContr(m), playerName(n), match(o, d)
   {}

   // print function
   void print(std::ostream &) const;

   double         matchContr;
   oppTeamAndDate match;
   nameRecord     playerName;
};

inline void playerMatchContrRecord::print(std::ostream &o) const
{
   o << "  " << playerName;

   match.print(o);
}

// TEAM RECORDS CLASSES //

/*
 * All team record types (teamScore, oppTeamScore, margin, teamWicketTally)
 * need only one integer to record.  So generic class suffices.
 */
class teamGenericRecord
{
 public:
   // constructors
   teamGenericRecord(const int &i): value(i) {}
   teamGenericRecord(const int &i, const titleRecord &o, const dateRecord &d)
   : value(i), match(o, d)
   {}

   int            value;
   oppTeamAndDate match;
};

} // end anonymous namespace

// TYPE DEFINITIONS ///////////////////////////////////////////////////////////////////////////////

/*
 * Record of player's statistics for a single match.
 * For use in compiling player's history.
 * (see indoorCricketDB::compilePlayerHistory())
 */
class playerMatchStats
{
 public:
   // constructor
   playerMatchStats(void)
   : date(), oppTeamName(),
     innsBatted(0), runsScored(0),
     oversBowled(0), wicketsTaken(0), runsConceded(0)
   {}

   dateRecord  date;
   titleRecord oppTeamName;
   int         innsBatted,
               runsScored,
               oversBowled,
               wicketsTaken,
               runsConceded;

   // print function
   void print(std::ostream &) const;
};

/*
 *
 */
class playerHistoryStruct
{
 public:
   // constructor
   playerHistoryStruct(void) {}

   // print function
   void print(std::ostream &) const;

   // clear function
   void clear(void) {matchStatsVect.clear();}

   dateBeginEnd                  period;
   nameRecord                    playerName;
   titleRecord                   teamName;
   std::vector<playerMatchStats> matchStatsVect;
};

/*
 * Record of team's statistics for a single match.
 * For use in compiling team's history.
 * (see indoorCricketDB::compileTeamHistory())
 */
class teamMatchStats
{
 public:
   // constructor
   teamMatchStats(void)
   : date(), oppTeamName(),
     teamScore(), oppTeamScore(), wicketTally()
   {}

   dateRecord  date;
   titleRecord oppTeamName;
   int         teamScore,
               oppTeamScore,
               wicketTally;

   // print function
   void print(std::ostream &) const;
};

/*
 *
 */
class teamHistoryStruct
{
 public:
   // constructor
   teamHistoryStruct(void) {}

   // print function
   void print(std::ostream &) const;

   // clear function
   void clear(void) {matchStatsVect.clear();}

   dateBeginEnd                period;
   titleRecord                 teamName;
   std::vector<teamMatchStats> matchStatsVect;
};

/*
 * Structure containing records of a player's best/worst performances
 * averages, and totals over a period of possibly many matches.
 */
class playerRecordsStruct
{
 public:
   // constructor
   playerRecordsStruct(void);

   // print function
   void print(std::ostream &) const;

   // clear function
   void clear(void);

   // period
   dateBeginEnd period;

   // player and team names
   nameRecord  playerName;
   titleRecord teamName;

   // totals
   restrictedRangeInt matchesPlayed,
                      innsBatted, runsScored,
                      oversBowled, wicketsTaken, runsConceded;

   // averages
   double avgRunsScrdPerInns,
          avgWktsTakenPerOver,
          avgRunsConcPerOver,
          avgMatchContr;

   // best and worst performance records
   std::list<playerInnsRecord>       bestInns,       wrstInns;
   std::list<playerOverRecord>       bestOver,       wrstOver;
   std::list<playerBowlFigsRecord>   bestBowlFigs,   wrstBowlFigs;
   std::list<playerMatchContrRecord> bestMatchContr, wrstMatchContr;

 private:
   void initLists(void);
   void initTotalsAndAvgs(void);
};

/*
 *
 */
inline void playerRecordsStruct::clear(void)
{
   // clear lists
   bestInns.clear();       wrstInns.clear();
   bestOver.clear();       wrstOver.clear();
   bestBowlFigs.clear();   wrstBowlFigs.clear();
   bestMatchContr.clear(); wrstMatchContr.clear();

   // clear totals
   initTotalsAndAvgs();

   // initialise lists
   initLists();
}

/*
 *
 */
inline void playerRecordsStruct::initTotalsAndAvgs(void)
{
   // totals
   matchesPlayed = 0;
   innsBatted    = runsScored   = 0;
   oversBowled   = wicketsTaken = runsConceded = 0;

   // averages
   avgRunsScrdPerInns  = 0.0;
   avgWktsTakenPerOver = 0.0;
   avgRunsConcPerOver  = 0.0;
   avgMatchContr       = 0.0;
}

enum rankingEnum {matchesPlayed,      innsBatted,          runsScored,
                  oversBowled,        wicketsTaken,        runsConceded,
                  avgRunsScrdPerInns, avgWktsTakenPerOver,
                  avgRunsConcPerOver, avgMatchContr,
                  bestInns, bestOver, bestBowlFigs,        bestMatchContr,
                  wrstInns, wrstOver, wrstBowlFigs,        wrstMatchContr };

extern rankingEnum ranking; // defined in "indcrkrecords.cpp"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        

// for use in sorting list of playerRecordsStructs
struct std::greater<class playerRecordsStruct>
{
   bool operator()(const playerRecordsStruct &r1, const playerRecordsStruct &r2) const
   {
      bool decision;

      switch (ranking)
      {
         // totals
         case matchesPlayed: decision = r1.matchesPlayed > r2.matchesPlayed; break;
         case innsBatted:    decision = r1.innsBatted    > r2.innsBatted;    break;
         case runsScored:    decision = r1.runsScored    > r2.runsScored;    break;
         case oversBowled:   decision = r1.oversBowled   > r2.oversBowled;   break;
         case wicketsTaken:  decision = r1.wicketsTaken  > r2.wicketsTaken;  break;
         case runsConceded:  decision = r1.runsConceded  > r2.runsConceded;  break;

         // averages
         case avgRunsScrdPerInns:  decision = r1.avgRunsScrdPerInns  > r2.avgRunsScrdPerInns;
              break;
         case avgWktsTakenPerOver: decision = r1.avgWktsTakenPerOver > r2.avgWktsTakenPerOver;
              break;
         case avgRunsConcPerOver:  decision = r1.avgRunsConcPerOver  < r2.avgRunsConcPerOver;
              break;
         case avgMatchContr:       decision = r1.avgMatchContr       > r2.avgMatchContr;
              break;

         // best performance records
         case bestInns:       decision =   r1.bestInns.front().runsScored
                                         > r2.bestInns.front().runsScored;       break;
         case bestOver:       decision =   r1.bestOver.front().runsConceded
                                         < r2.bestOver.front().runsConceded;     break;
         case bestBowlFigs:   decision =   r1.bestBowlFigs.front().runsConceded
                                         < r2.bestBowlFigs.front().runsConceded; break;
         case bestMatchContr: decision =   r1.bestMatchContr.front().matchContr
                                         > r2.bestMatchContr.front().matchContr; break;

         // worst performance records
         case wrstInns:       decision =   r1.wrstInns.front().runsScored
                                         < r2.wrstInns.front().runsScored;       break;
         case wrstOver:       decision =   r1.wrstOver.front().runsConceded
                                         > r2.wrstOver.front().runsConceded;     break;
         case wrstBowlFigs:   decision =   r1.wrstBowlFigs.front().runsConceded
                                         > r2.wrstBowlFigs.front().runsConceded; break;
         case wrstMatchContr: decision =   r1.wrstMatchContr.front().matchContr
                                         < r2.wrstMatchContr.front().matchContr; break;

      }

      // if want reverse order, set decision = !decision;
      return decision;
   }
};

/*
 * Intended to be used only in 'class teamRecordsStruct'.
 * Sub categories are:
 *  overall, batting1st, batting2nd, withFullTeam, missing1Player, missing2Players
 * Purpose is to avoid duplication of code.
 */
class teamRecordsStructSubCategory
{
 public:
   // constructor
   teamRecordsStructSubCategory(void);

   // totals
   restrictedRangeInt matchesPlayed,
                      matchesWon,
                      matchesLost,
                      matchesDrawn,
                      wicketsTaken;
   int                runsScored,
                      runsConceded;
   // averages
   double winRate,
          avgTeamScore,
          avgOppTeamScore,
          avgMargin,
          avgWicketTally;
};

/*
 *
 */
inline teamRecordsStructSubCategory::teamRecordsStructSubCategory(void)
: matchesPlayed(0, std::numeric_limits<int>::max()),
  matchesWon(0, std::numeric_limits<int>::max()),
  matchesLost(0, std::numeric_limits<int>::max()),
  matchesDrawn(0, std::numeric_limits<int>::max()),
  wicketsTaken(0, std::numeric_limits<int>::max()),
  runsScored(0),
  runsConceded(0),
  winRate(0.0),
  avgTeamScore(0.0),
  avgOppTeamScore(0.0),
  avgMargin(0.0),
  avgWicketTally(0.0)
{}

/*
 * Structure containing records of a team's best/worst performances
 * averages, and totals over a period of possibly many matches.
 */
class teamRecordsStruct
{
 public:
   // constructor
   teamRecordsStruct(void);

   // print function
   void print(std::ostream &) const;

   // clear function
   void clear(void);

   // period
   dateBeginEnd period;

   // team name
   titleRecord teamName;

   // totals and averages for various categories as defined in class teamRecordsStructSubCategory
   teamRecordsStructSubCategory category[6];
   enum {overall = 0,  batting1st,     batting2nd,
         withFullTeam, missing1Player, missing2Players};

   // longest streaks
   int          longestWstreak,       // no. of matches
                longestLstreak;       // no. of matches
   dateBeginEnd longestWstreakPeriod,
                longestLstreakPeriod;

   // best and worst team performance records
   std::list<teamGenericRecord> bestScore,       wrstScore,
                                bestOppScore,    wrstOppScore,
                                bestMargin,      wrstMargin,
                                bestWicketTally, wrstWicketTally;
};

inline void teamRecordsStruct::clear(void)
{
   // clear lists
   bestScore.clear();       wrstScore.clear();
   bestOppScore.clear();    wrstOppScore.clear();
   bestMargin.clear();      wrstMargin.clear();
   bestWicketTally.clear(); wrstWicketTally.clear();

   // initialise lists
   teamRecordsStruct();
}

/*
 *
 */
class teamIndividualRecordsStruct
{
 public:
   // constructor
   teamIndividualRecordsStruct(void);

   // print function
   void print(std::ostream &) const;

   // clear function
   void clear(void);

   // period
   dateBeginEnd period;

   // team name
   titleRecord teamName;

   // best and worst batting partnerships
   std::list<playerInnsRecord> bestBattPship1, // best  partnership player 1 innings record
                               bestBattPship2, //                   player 2 innings record
                               wrstBattPship1, // worst partnership player 1 innings record
                               wrstBattPship2; //                   player 2 innings record

   // best and worst player performance records
   playerRecordsStruct bestPlayerRecords;
};

inline void teamIndividualRecordsStruct::clear(void)
{
   // clear lists
   bestBattPship1.clear(); wrstBattPship1.clear();
   bestBattPship2.clear(); wrstBattPship2.clear();

   // initialise lists
   teamIndividualRecordsStruct();
}

// MORE FUNCTION DECLARATIONS /////////////////////////////////////////////////////////////////////

void printRankingsTable(const std::list<playerRecordsStruct> &,
                        const titleRecord &teamName,
                        const titleRecord &oppTeamName,
                        const dateBeginEnd &, const int &minMatches, std::ostream &);

// INLINE FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

/*
 *
 */
inline double matchContribution(const int &innsBatted,  const int &runsScored,
                                const int &oversBowled, const int &runsConceded)
{
   return   static_cast<double>(      runsScored)   / static_cast<double>(innsBatted)
          - static_cast<double>(2.0 * runsConceded) / static_cast<double>(oversBowled);
}

/*
 *
 */
inline void printMatchContrDefinition(std::ostream &out)
{
   using std::endl;

   out << "* Match Contribution:"                                          << endl
       << "   Loosely = runs scored minus runs conceded for the match."    << endl
       << "   To account for the case when player bats more than once or"  << endl
       << "   bowls more than twice (possible if team is short players),"  << endl
       << "   the formula used is:"                                        << endl
       << "                         (Runs Scored)     2 * (Runs Conceded)" << endl
       << "   Match Contribution = ---------------- - -------------------" << endl
       << "                        (Innings Batted)     (Overs Bowled)"    << endl;
}

/*
 *
 */
inline double bowlFigsWicketsTaken(const int &oversBowled, const int &wicketsTaken)
{
   return static_cast<double>(2.0 * wicketsTaken) / static_cast<double>(oversBowled);
}

/*
 *
 */
inline double bowlFigsRunsConceded(const int &oversBowled, const int &runsConceded)
{
   return static_cast<double>(2.0 * runsConceded) / static_cast<double>(oversBowled);
}

/*
 *
 */
inline void printBowlFigsDefinition(std::ostream &out)
{
   using std::endl;

   out << "* Bowling Figures:"                                           << endl
       << "   To account for the case when player bowls more than twice" << endl
       << "   (possible if team is short players), The figures used are" << endl
       << "   the averages (wickets taken, runs conceded) per two overs" << endl
       << "   bowled."                                                   << endl;
}

#endif

/*****************************************END*OF*FILE*********************************************/
