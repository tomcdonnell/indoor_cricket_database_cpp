/*************************************************************************************************\
*                                                                                                 *
* "IndCrkRecords.cpp" - Member functions for classes defined in "IndCrkRecords.h".                *
*                                                                                                 *
*              Author - Tom McDonnell 2005                                                        *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "IndCrkRecords.h"

#include <limits>

// FILE SCOPE FUNCTION DECLARATIONS ///////////////////////////////////////////////////////////////

namespace
{
   void printMatchBowlingFigs(const playerBowlFigsRecord &, const int &width, std::ostream &);
}

// MEMBER FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

/*
 *
 */
void playerHistoryStruct::print(std::ostream &out) const
{
   using std::endl;
   using std::setw;

   // print player history
   out                                                                                    << endl
                                                                                          << endl
     << "Player History - " << playerName << ", " << teamName                             << endl
     << "Period " << period                                                               << endl
     << "-------------------------------------------------------------------------------" << endl
     << "Date        Opposition                       Batting    Bowling         Overall" << endl
     << "                                             Inns Runs  Ovrs Wkts Runs         " << endl
     << "-------------------------------------------------------------------------------" << endl;

   std::vector<playerMatchStats>::const_iterator p;
   for (p = matchStatsVect.begin(); p != matchStatsVect.end(); ++p)
     p->print(out);

   out
     << "-------------------------------------------------------------------------------" << endl;
}

/*
 * To be used in conjunction with above playerHistoryStruct::print() function.
 */
void playerMatchStats::print(std::ostream &out) const
{
   using std::setw;

   out << date << "  ";

   out << setw(titleRecord::titleMaxLength) << oppTeamName  << " ";

   out << setw(4) << innsBatted   << " " << setw(4) << runsScored   << "  "
       << setw(4) << oversBowled  << " " << setw(4) << wicketsTaken << " "
       << setw(4) << runsConceded << " "
       << setw(8) << matchContribution(innsBatted,  runsScored,
                                       oversBowled, runsConceded) << std::endl;
}

/*
 *
 */
void teamHistoryStruct::print(std::ostream &out) const
{
   using std::endl;
   using std::setw;


   // print team history
   out                                                                               << endl
     << "Team History - " << teamName                                                << endl
     << "Period " << period                                                          << endl
     << "--------------------------------------------------------------------------" << endl
     << "Date        Opposition                        Team   Opp.   Margin  Result" << endl
     << "                                              Score  Score                " << endl
     << "--------------------------------------------------------------------------" << endl;

   std::vector<teamMatchStats>::const_iterator p;
   for (p = matchStatsVect.begin(); p != matchStatsVect.end(); ++p)
     p->print(out);

   out
     << "--------------------------------------------------------------------------" << endl;
}

/*
 * To be used in conjunction with above teamHistoryStruct::print() function.
 */
void teamMatchStats::print(std::ostream &out) const
{
   using std::setw;
   using std::endl;

   out << date << "  ";
       
   out << setw(titleRecord::titleMaxLength) << oppTeamName << "  ";

   out << setw(5) << teamScore                << "  "
       << setw(5) << oppTeamScore             << "  "
       << setw(6) << teamScore - oppTeamScore << "  "
       << setw(6);

        if (teamScore > oppTeamScore) out << "W" << endl;
   else if (teamScore < oppTeamScore) out << "L" << endl;
   else                               out << "D" << endl;
}

// class playerRecordsStruct //

/*
 * Constructor for 'class playerRecordsRecord'.
 * Lists are required because a record score may not be unique.
 * In the case of equal records, they will appear in the list sorted according to date.
 * 'Best'  records are initialised to improbably bad  values.
 * 'Worst' records are initialised to improbably good values.
 * This is required because the best/worst records are found by
 * traversing the list of matchRecords and updating the best/worst
 * records if the latest one is better/worse than the current best/worst.
 */
playerRecordsStruct::playerRecordsStruct(void)
: matchesPlayed(0, std::numeric_limits<int>::max()),
  innsBatted(0, std::numeric_limits<int>::max()),
  runsScored(0, std::numeric_limits<int>::max()),
  oversBowled(0, std::numeric_limits<int>::max()),
  wicketsTaken(0, std::numeric_limits<int>::max()),
  runsConceded(0, std::numeric_limits<int>::max()),
  avgRunsScrdPerInns(0.0),
  avgWktsTakenPerOver(0.0),
  avgRunsConcPerOver(0.0),
  avgMatchContr(0.0)
{
   initLists();
}

/*
 *
 */
void playerRecordsStruct::initLists(void)
{
   // insert initial records into lists
   bestInns.push_back(playerInnsRecord(-999));
   wrstInns.push_back(playerInnsRecord( 999));
   bestOver.push_back(playerOverRecord(  99));
   wrstOver.push_back(playerOverRecord( -99));
   bestBowlFigs.push_back(playerBowlFigsRecord( 0, 99));
   wrstBowlFigs.push_back(playerBowlFigsRecord(99, -99));
   bestMatchContr.push_back(playerMatchContrRecord(-999));
   wrstMatchContr.push_back(playerMatchContrRecord( 999));
}

/*
 *
 */
void playerRecordsStruct::print(std::ostream &out) const
{
   using std::endl;
   using std::setw;

   // print player's records
   out 
                                                                                       << endl
     << "Player Records - " << playerName << ", " << teamName                          << endl
     << "Period " << period                                                            << endl
     << "--------------------------------------------------------------"               << endl
     << "Matches:                      " << setw(32) << matchesPlayed                  << endl
                                                                                       << endl
     << "Batting"                                                                      << endl
     << " Total Innings Batted:        " << setw(32) << innsBatted                     << endl
     << " Total Runs Scored:           " << setw(32) << runsScored                     << endl
     << " Average Runs / Innings:      " << setw(32) << avgRunsScrdPerInns             << endl;

   out                                                                                 << endl
     << " Best Innings:                " << setw(32) << bestInns.front().runsScored    << endl;
   for (std::list<playerInnsRecord>::const_reverse_iterator p1
          = bestInns.rbegin(); p1 != bestInns.rend(); ++p1    )
     p1->match.print(out);

   out
     << " Worst Innings:               " << setw(32) << wrstInns.front().runsScored    << endl;
   for (p1 = wrstInns.rbegin(); p1 != wrstInns.rend(); ++p1)
     p1->match.print(out);

   out                                                                                 << endl
     << "Bowling"                                                                      << endl
     << " Total Overs Bowled:          " << setw(32) << oversBowled                    << endl
     << " Total Wickets Taken:         " << setw(32) << wicketsTaken                   << endl
     << " Total Runs Conceded:         " << setw(32) << runsConceded                   << endl
     << " Average Wickets / Over:      " << setw(32) << avgWktsTakenPerOver            << endl
     << " Average Runs Conceded / Over:" << setw(32) << avgRunsConcPerOver             << endl;

   out                                                                                 << endl
     << " Best Over: " << setw(46) << bestOver.front().wicketsTaken << "/"
                       << setw(3)  << bestOver.front().runsConceded                    << endl;
   for (std::list<playerOverRecord>::const_reverse_iterator p2
          = bestOver.rbegin(); p2 != bestOver.rend(); ++p2    )
     p2->match.print(out);

   out
     << " Worst Over:" << setw(46) << wrstOver.front().wicketsTaken << "/"
                       << setw(3)  << wrstOver.front().runsConceded                    << endl;
   for (p2 = wrstOver.rbegin(); p2 != wrstOver.rend(); ++p2)
     p2->match.print(out);

   out                                                                                 << endl
     << " Best Match Bowling Figures: (*) ";
   printMatchBowlingFigs(bestBowlFigs.front(), 28, out);
   for (std::list<playerBowlFigsRecord>::const_reverse_iterator p3
          = bestBowlFigs.rbegin(); p3 != bestBowlFigs.rend(); ++p3)
     p3->match.print(out);

   out
     << " Worst Match Bowling Figures: (*)";
   printMatchBowlingFigs(wrstBowlFigs.front(), 28, out);
   for (p3 = wrstBowlFigs.rbegin(); p3 != wrstBowlFigs.rend(); ++p3)
     p3->match.print(out);

   out                                                                                 << endl
     << "Overall"                                                                      << endl
     << " Average Match Contribution: (*)" << setw(30) << avgMatchContr                << endl;

   out                                                                                 << endl
     << " Best Match Contribution: (*) " << setw(32)
                                         << bestMatchContr.front().matchContr          << endl;
   for (std::list<playerMatchContrRecord>::const_reverse_iterator p4
          = bestMatchContr.rbegin(); p4 != bestMatchContr.rend(); ++p4)
     p4->match.print(out);

   out
     << " Worst Match Contribution: (*)" << setw(32)
                                         << wrstMatchContr.front().matchContr          << endl;
   for (p4 = wrstMatchContr.rbegin(); p4 != wrstMatchContr.rend(); ++p4)
     p4->match.print(out);

   out
     << "--------------------------------------------------------------"               << endl;

   printMatchContrDefinition(out);
   printBowlFigsDefinition(out);

   out
     << "--------------------------------------------------------------"               << endl;
}

// class teamRecordsStruct //

/*
 * Constructor for 'class teamRecordsStruct'.
 * Lists are required because a record score may not be unique.
 * In the case of equal records, they will appear in the list sorted according to date.
 * 'Best'  records are initialised to improbably bad  values.
 * 'Worst' records are initialised to improbably good values.
 * This is required because the best/worst records are found by
 * traversing the list of matchRecords and updating the best/worst
 * records if the latest one is better/worse than the current best/worst.
 */
teamRecordsStruct::teamRecordsStruct(void)
: longestWstreak(0),
  longestLstreak(0)
{
   // insert initial records into lists
   bestScore.push_back(teamGenericRecord(-999));
   wrstScore.push_back(teamGenericRecord( 999));
   bestOppScore.push_back(teamGenericRecord(-999));
   wrstOppScore.push_back(teamGenericRecord( 999));
   bestMargin.push_back(teamGenericRecord(-999));
   wrstMargin.push_back(teamGenericRecord( 999));
   bestWicketTally.push_back(teamGenericRecord(  0));
   wrstWicketTally.push_back(teamGenericRecord(999));
}

/*
 *
 */
void teamRecordsStruct::print(std::ostream &out) const
{
   using std::endl;
   using std::setw;

   using teamRecordsStruct::overall;
   using teamRecordsStruct::batting1st;
   using teamRecordsStruct::batting2nd;
   using teamRecordsStruct::withFullTeam;
   using teamRecordsStruct::missing1Player;
   using teamRecordsStruct::missing2Players;

   int oldPrecision;
   std::ios_base::fmtflags oldFlags;

   oldFlags     = out.flags(std::ios_base::fixed); // set to fixed format and save old
   oldPrecision = out.precision(2);                // set precision and save old

   // print team records
   out 
                                                                                        << endl
     << "Team Records - " << teamName                                                   << endl
     << "Period " << period                                                             << endl
     << "-----------------------------------------------------------------------------" << endl
                                                                                        << endl
     << " Results Summary Table"                                                        << endl
     << " ---------------------------------------------------------------------------"  << endl
     << "                      ::::::Totals::::::         ::::::::::Averages:::::::::"  << endl
     << "                                                  Team   Opp.         Wicket"  << endl
     << " Subset of Matches      P    W    L    D  Win %  Score  Score  Margin  Tally"  << endl
     << " ---------------------------------------------------------------------------"  << endl;

   for (int i = 0; i < 6; ++i)
   {
      switch (i)
      {
       case overall:         out << " All Matches Played "; break;
       case batting1st:      out << " Batted 1st         "; break;
       case batting2nd:      out << " Batted 2nd         "; break;
       case withFullTeam:    out << " Had Full Team      "; break;
       case missing1Player:  out << " Missing 1 Player   "; break;
       case missing2Players: out << " Missing 2 Players  "; break;
      }

      out << setw(5) << category[i].matchesPlayed
          << setw(5) << category[i].matchesWon
          << setw(5) << category[i].matchesLost
          << setw(5) << category[i].matchesDrawn
          << setw(7) << category[i].winRate
          << setw(7) << category[i].avgTeamScore
          << setw(7) << category[i].avgOppTeamScore
          << setw(8) << category[i].avgMargin
          << setw(7) << category[i].avgWicketTally  << endl;
   }

   out
     << " ---------------------------------------------------------------------------"  << endl;

   out.precision(oldPrecision);                    // reset precision
   out.flags(oldFlags);                            // reset to default format

   out                                                                                  << endl
     << "Streaks"                                                                       << endl
     << " Longest Winning:        " << setw(44) << longestWstreak << " matches"         << endl
     << "  " << longestWstreakPeriod                                                    << endl
     << " Longest Losing:         " << setw(44) << longestLstreak << " matches"         << endl
     << "  " << longestLstreakPeriod                                                    << endl;

   out                                                                                  << endl
     << "Best / Worst Team Performances"                                                << endl
     << " Best Score:             " << setw(52) << bestScore.front().value              << endl;
   for (std::list<teamGenericRecord>::const_reverse_iterator p1
          = bestScore.rbegin(); p1 != bestScore.rend(); ++p1 )
     p1->match.print(out);

   out
     << " Worst Score:            " << setw(52) << wrstScore.front().value              << endl;
   for (p1 = wrstScore.rbegin(); p1 != wrstScore.rend(); ++p1)
     p1->match.print(out);

   out                                                                                  << endl
     << " Best Opposition Score:  " << setw(52) << bestOppScore.front().value           << endl;
   for (std::list<teamGenericRecord>::const_reverse_iterator p2
          = bestOppScore.rbegin(); p2 != bestOppScore.rend(); ++p2)
     p2->match.print(out);

   out
     << " Worst Opposition Score: " << setw(52) << wrstOppScore.front().value           << endl;
   for (p2 = wrstOppScore.rbegin(); p2 != wrstOppScore.rend(); ++p2)
     p2->match.print(out);

   out                                                                                  << endl
     << " Best Margin:            " << setw(52) << bestMargin.front().value             << endl;
   for (std::list<teamGenericRecord>::const_reverse_iterator p3
          = bestMargin.rbegin(); p3 != bestMargin.rend(); ++p3)
     p3->match.print(out);

   out
     << " Worst Margin:           " << setw(52) << wrstMargin.front().value             << endl;
   for (p3 = wrstMargin.rbegin(); p3 != wrstMargin.rend(); ++p3)
     p3->match.print(out);

   out                                                                                  << endl
     << " Best Team Wicket Tally: " << setw(52) << bestWicketTally.front().value        << endl;
   for (std::list<teamGenericRecord>::const_reverse_iterator p4
          = bestWicketTally.rbegin(); p4 != bestWicketTally.rend(); ++p4)
     p4->match.print(out);

   out
     << " Worst Team Wicket Tally:" << setw(52) << wrstWicketTally.front().value        << endl;
   for (p4 = wrstWicketTally.rbegin(); p4 != wrstWicketTally.rend(); ++p4)
     p4->match.print(out);

   out
     << "-----------------------------------------------------------------------------" << endl;
}

/*
 * Constructor for 'class teamIndividualRecordsStruct'.
 * Lists are required because a record score may not be unique.
 * In the case of equal records, they will appear in the list sorted according to date.
 * 'Best'  records are initialised to improbably bad  values.
 * 'Worst' records are initialised to improbably good values.
 * This is required because the best/worst records are found by
 * traversing the list of matchRecords and updating the best/worst
 * records if the latest one is better/worse than the current best/worst.
 */
teamIndividualRecordsStruct::teamIndividualRecordsStruct(void)
{
   // insert initial records into lists
   bestBattPship1.push_back(playerInnsRecord(indCrkRecords::minPossibleRunsScoredPerInns));
   bestBattPship2.push_back(playerInnsRecord(indCrkRecords::minPossibleRunsScoredPerInns));
   wrstBattPship1.push_back(playerInnsRecord(indCrkRecords::maxPossibleRunsScoredPerInns));
   wrstBattPship2.push_back(playerInnsRecord(indCrkRecords::maxPossibleRunsScoredPerInns));
}

/*
 *
 */
void teamIndividualRecordsStruct::print(std::ostream &out) const
{
   using std::endl;
   using std::setw;

   // print team individual records
   out 
                                                                                       << endl
     << "Team Individual Records - " << teamName                                       << endl
     << "Period " << period                                                            << endl
     << "--------------------------------------------------------------"               << endl;

   // best player records

   const playerRecordsStruct &p = bestPlayerRecords;

   out
     << " Best Innings:                " << setw(32) << p.bestInns.front().runsScored  << endl;
   for (std::list<playerInnsRecord>::const_reverse_iterator p5
          = p.bestInns.rbegin(); p5 != p.bestInns.rend(); ++p5)
     p5->print(out);

   out
     << " Worst Innings:               " << setw(32) << p.wrstInns.front().runsScored  << endl;
   for (p5 = p.wrstInns.rbegin(); p5 != p.wrstInns.rend(); ++p5)
     p5->print(out);

   out                                                                                 << endl
     << " Best Over: " << setw(46) << p.bestOver.front().wicketsTaken << "/"
                       << setw(3)  << p.bestOver.front().runsConceded                  << endl;
   for (std::list<playerOverRecord>::const_reverse_iterator p6
          = p.bestOver.rbegin(); p6 != p.bestOver.rend(); ++p6)
     p6->print(out);

   out
     << " Worst Over:" << setw(46) << p.wrstOver.front().wicketsTaken << "/"
                       << setw(3)  << p.wrstOver.front().runsConceded                  << endl;
   for (p6 = p.wrstOver.rbegin(); p6 != p.wrstOver.rend(); ++p6)
     p6->print(out);

   out                                                                                 << endl
     << " Best Match Bowling Figures: (*) ";
   printMatchBowlingFigs(p.bestBowlFigs.front(), 28, out);
   for (std::list<playerBowlFigsRecord>::const_reverse_iterator p7
          = p.bestBowlFigs.rbegin(); p7 != p.bestBowlFigs.rend(); ++p7)
     p7->print(out);

   out
     << " Worst Match Bowling Figures: (*)";
   printMatchBowlingFigs(p.wrstBowlFigs.front(), 28, out);
   for (p7 = p.wrstBowlFigs.rbegin(); p7 != p.wrstBowlFigs.rend(); ++p7)
     p7->print(out);

   out                                                                                 << endl
     << " Best Match Contribution: (*) " << setw(32)
                                         << p.bestMatchContr.front().matchContr        << endl;
   for (std::list<playerMatchContrRecord>::const_reverse_iterator p8
          = p.bestMatchContr.rbegin(); p8 != p.bestMatchContr.rend(); ++p8)
     p8->print(out);

   out
     << " Worst Match Contribution: (*)" << setw(32)
                                         << p.wrstMatchContr.front().matchContr        << endl;
   for (p8 = p.wrstMatchContr.rbegin(); p8 != p.wrstMatchContr.rend(); ++p8)
     p8->print(out);

   out                                                                                 << endl
     << " Best Batting Partnership:    " << setw(32)
                                         <<   bestBattPship1.front().runsScored
                                            + bestBattPship2.front().runsScored        << endl;
   std::list<playerInnsRecord>::const_reverse_iterator p9  = bestBattPship1.rbegin();
   std::list<playerInnsRecord>::const_reverse_iterator p10 = bestBattPship2.rbegin();
   for (; p9  != bestBattPship1.rend() && p10 != bestBattPship2.rend(); ++p9, ++p10)
   {
      //assert(p9->match == p10->match);
      //assert(p9->runsScored == p10->runsScored);
      out << "  " << p9->runsScored;  p9->print(out); 
      out << "  " << p10->runsScored; p10->print(out); 
   }

   out
     << " Worst Batting Partnership:   " << setw(32)
                                         <<   wrstBattPship1.front().runsScored
                                            + wrstBattPship2.front().runsScored        << endl;
   std::list<playerInnsRecord>::const_reverse_iterator p11 = wrstBattPship1.rbegin();
   std::list<playerInnsRecord>::const_reverse_iterator p12 = wrstBattPship2.rbegin();
   for (; p11  != wrstBattPship1.rend() && p12 != wrstBattPship2.rend(); ++p11, ++p12)
   {
      //assert(p9->match == p10->match);
      //assert(p9->runsScored == p10->runsScored);
      out << "  " << p11->runsScored; p11->print(out); 
      out << "  " << p12->runsScored; p12->print(out); 
   }

   out
     << "--------------------------------------------------------------"               << endl;

   printMatchContrDefinition(out);
   printBowlFigsDefinition(out);

   out
     << "--------------------------------------------------------------"               << endl;
}

rankingEnum ranking;

/*
 *
 */
void printRankingsTable(const std::list<playerRecordsStruct> &r,
                        const titleRecord &teamName,
                        const titleRecord &oppTeamName,
                        const dateBeginEnd &period,
                        const int &minMatches, std::ostream &out)
{
   using std::endl;
   using std::setw;

   //////////////////////////////////////////////////////////////////// print table title START //
   out << endl << "Player Rankings ";

   // print next part of table title depending on ranked variable
   switch (ranking)
   {
    // totals
    case matchesPlayed: out << "(Total Matches Played)"; break;
    case innsBatted:    out << "(Total Innings Batted)"; break;
    case runsScored:    out << "(Total Runs Scored)";    break;
    case oversBowled:   out << "(Total Overs Bowled)";   break;
    case wicketsTaken:  out << "(Total Wickets Taken)";  break;
    case runsConceded:  out << "(Total Runs Conceded)";  break;

    // averages
    case avgRunsScrdPerInns:  out << "(Avg. Runs Scored Per Innings)"; break;
    case avgWktsTakenPerOver: out << "(Avg. Wickets Taken Per Over)";  break;
    case avgRunsConcPerOver:  out << "(Avg. Runs Conceded Per Over)";  break;
    case avgMatchContr:       out << "(Avg. Match Contribution)";      break;

    // best performance records
    case bestInns:       out << "(Best Innings)";               break;
    case bestOver:       out << "(Best Over)";                  break;
    case bestBowlFigs:   out << "(Best Match Bowling Figures)"; break;
    case bestMatchContr: out << "(Best Match Contribution)";    break;

    // worst performance records
    case wrstInns:       out << "(Worst Innings)";               break;
    case wrstOver:       out << "(Worst Over)";                  break;
    case wrstBowlFigs:   out << "(Worst Match Bowling Figures)"; break;
    case wrstMatchContr: out << "(Worst Match Contribution)";    break;
   }
   out << endl;

   out << "Team   " << teamName;
   if (oppTeamName != titleRecord())
     out << " (Vs. " << oppTeamName << ")";
   out << endl;

   out << "Period " << period << endl;

   if (minMatches > 1)
     out << "(Minimum " << minMatches << " Matches)" << endl;
   //////////////////////////////////////////////////////////////////// print table title FINISH //

   // format column headings depending on ranked variable
   switch (ranking)
   {
    // totals
    case matchesPlayed:
      out << "------------------------------------------------" << endl
          << "Rank  First name       Last name         Matches" << endl
          << "------------------------------------------------" << endl;
      break;
    case innsBatted:
      out << "--------------------------------------------------------" << endl
          << "Rank  First name       Last name        Matches  Innings" << endl
          << "--------------------------------------------------------" << endl;
      break;
    case runsScored:
      out << "-----------------------------------------------------------------" << endl
          << "Rank  First name       Last name        Matches  Innings    Total" << endl
          << "-----------------------------------------------------------------" << endl;
      break;
    case oversBowled:
      out << "--------------------------------------------------------" << endl
          << "Rank  First name       Last name        Matches    Overs" << endl
          << "--------------------------------------------------------" << endl;
      break;
    case wicketsTaken:
    case runsConceded:
      out << "-----------------------------------------------------------------" << endl
          << "Rank  First name       Last name        Matches    Overs    Total" << endl
          << "-----------------------------------------------------------------" << endl;
      break;

    // averages
    case avgRunsScrdPerInns:
      out << "-----------------------------------------------------------------" << endl
          << "Rank  First name       Last name        Matches  Innings  Average" << endl
          << "-----------------------------------------------------------------" << endl;
      break;
    case avgWktsTakenPerOver:
    case avgRunsConcPerOver:
      out << "-----------------------------------------------------------------" << endl
          << "Rank  First name       Last name        Matches    Overs  Average" << endl
          << "-----------------------------------------------------------------" << endl;
      break;
    case avgMatchContr:
      out << "--------------------------------------------------------" << endl
          << "Rank  First name       Last name        Matches  Average" << endl
          << "--------------------------------------------------------" << endl;
      break;

    // bests
    case bestInns:
    case bestOver:
    case bestBowlFigs:
    case bestMatchContr:
      out
      << "-------------------------------------------------------------------------------" << endl
      << "Rank  First name       Last name        Opp. Team           Date           Best" << endl
      << "-------------------------------------------------------------------------------" << endl;
      break;

    // worsts
    case wrstInns:
    case wrstOver:
    case wrstBowlFigs:
    case wrstMatchContr:
      out
      << "-------------------------------------------------------------------------------" << endl
      << "Rank  First name       Last name        Opp. Team           Date          Worst" << endl
      << "-------------------------------------------------------------------------------" << endl;
      break;
   }

   int    rank = 0;
   double prevValue = -1,
          currValue;
   int oldPrecision;
   std::ios_base::fmtflags oldFlags;
   std::list<playerRecordsStruct>::const_iterator p;
   for (p = r.begin(); p != r.end(); ++p)
   {
      if (p->matchesPlayed >= minMatches)
      {
         // set currValue depending on ranked variable
         switch (ranking)
         {
          // totals
          case matchesPlayed: currValue = p->matchesPlayed; break;
          case innsBatted:    currValue = p->innsBatted;    break;
          case runsScored:    currValue = p->runsScored;    break;
          case oversBowled:   currValue = p->oversBowled;   break;
          case wicketsTaken:  currValue = p->wicketsTaken;  break;
          case runsConceded:  currValue = p->runsConceded;  break;

          // averages
          case avgRunsScrdPerInns:  currValue = p->avgRunsScrdPerInns;  break;
          case avgWktsTakenPerOver: currValue = p->avgWktsTakenPerOver; break;
          case avgRunsConcPerOver:  currValue = p->avgRunsConcPerOver;  break;
          case avgMatchContr:       currValue = p->avgMatchContr;       break;

          // best performance records
          case bestInns:       currValue = p->bestInns.front().runsScored;       break;
          case bestOver:       currValue = p->bestOver.front().runsConceded;     break;
          case bestBowlFigs:   currValue = p->bestBowlFigs.front().runsConceded; break;
          case bestMatchContr: currValue = p->bestMatchContr.front().matchContr; break;

          // worst performance records
          case wrstInns:       currValue = p->wrstInns.front().runsScored;       break;
          case wrstOver:       currValue = p->wrstOver.front().runsConceded;     break;
          case wrstBowlFigs:   currValue = p->wrstBowlFigs.front().runsConceded; break;
          case wrstMatchContr: currValue = p->wrstMatchContr.front().matchContr; break;
         }

         // print ranking
         if (rank == 0)
         {
            // 1st line, rank is always 1
            rank = 1;
            out << setw(4) << rank << "  ";
         }
         else
         {
            if (currValue == prevValue)
              out << "   =  "; // print equal symbol (=) instead of rank
            else
            {
               ++rank;
               out << setw(4) << rank << "  "; // print rank as normal
            }
         }

         p->playerName.printFormatted(out, 16, 16);

         // print data for extra columns depending on ranked variable
         switch (ranking)
         {
          case matchesPlayed: break; // no extra columns needed

          // cases needing only one extra column - matchesPlayed
          case innsBatted:
          case oversBowled:
            out << setw(8) << p->matchesPlayed;
            break;

          // cases involving batting (needing extra columns matchesPlayed & innsBatted)
          case runsScored:
          case avgRunsScrdPerInns:
            out << setw(8) << p->matchesPlayed
                << setw(9) << p->innsBatted;
            break;

          // cases involving bowling (needing extra columns matchesPlayed & oversBowled)
          case wicketsTaken:
          case runsConceded:
          case avgWktsTakenPerOver:
          case avgRunsConcPerOver:
            out << setw(8) << p->matchesPlayed
                << setw(9) << p->oversBowled;
            break;

          // cases involving match contribution
          case avgMatchContr:
            out << setw(8) << p->matchesPlayed;
            break;

          // cases involving bests
          case bestInns:
            out << " " << setw(20) << p->bestInns.front().match.oppTeamName
                       << setw(12) << p->bestInns.front().match.date;
            break;
          case bestOver:
            out << " " << setw(20) << p->bestOver.front().match.oppTeamName
                       << setw(12) << p->bestOver.front().match.date;
            break;
          case bestBowlFigs:
            out << " " << setw(20) << p->bestBowlFigs.front().match.oppTeamName
                       << setw(12) << p->bestBowlFigs.front().match.date;
            break;
          case bestMatchContr:
            out << " " << setw(20) << p->bestMatchContr.front().match.oppTeamName
                       << setw(12) << p->bestMatchContr.front().match.date;
            break;

          // cases involving worsts
          case wrstInns:
            out << " " << setw(20) << p->wrstInns.front().match.oppTeamName
                       << setw(12) << p->wrstInns.front().match.date;
            break;
          case wrstOver:
            out << " " << setw(20) << p->wrstOver.front().match.oppTeamName
                       << setw(12) << p->wrstOver.front().match.date;
            break;
          case wrstBowlFigs:
            out << " " << setw(20) << p->wrstBowlFigs.front().match.oppTeamName
                       << setw(12) << p->wrstBowlFigs.front().match.date;
            break;
          case wrstMatchContr:
            out << " " << setw(20) << p->wrstMatchContr.front().match.oppTeamName
                       << setw(12) << p->wrstMatchContr.front().match.date;
            break;
         }

         // print current value of ranked variable (as integer or float)
         switch (ranking)
         {
          // integer values
          case matchesPlayed:
          case innsBatted:
          case runsScored:
          case oversBowled:
          case wicketsTaken:
          case runsConceded:
          case bestInns:
          case bestOver:
          case wrstInns:
          case wrstOver:
            oldFlags     = out.flags(std::ios_base::fixed); // set to fixed format and save old
            oldPrecision = out.precision(0);                // set precision and save old
            out << setw(9) << currValue << endl;
            out.precision(oldPrecision);                    // reset precision
            out.flags(oldFlags);                            // reset to default format
            break;

          // floating point values
          default:
            oldFlags     = out.flags(std::ios_base::fixed); // set to fixed format and save old
            oldPrecision = out.precision(4);                // set precision and save old
            out << setw(9) << currValue << endl;
            out.precision(oldPrecision);                    // reset precision
            out.flags(oldFlags);                            // reset to default format
            break;
         }

         prevValue = currValue;
      }
   }

   // print bottom border depending on ranking variable
   switch (ranking)
   {
    // case having 4 columns
    case matchesPlayed:
      out << "------------------------------------------------" << endl;
      break;

    // cases having 5 columns
    case innsBatted:
    case oversBowled:
    case avgMatchContr:
      out << "--------------------------------------------------------" << endl;
      break;

    // cases involving bests/worsts
    case bestInns:
    case bestOver:
    case bestBowlFigs:
    case bestMatchContr:
    case wrstInns:
    case wrstOver:
    case wrstBowlFigs:
    case wrstMatchContr:
      out
      << "-------------------------------------------------------------------------------" << endl;
      break;

    // cases having 6 columns
    default:
      out << "-----------------------------------------------------------------" << endl;
      break;
   }
}

// FILE SCOPE FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////

namespace
{
   /*
    * Match bowling figures will occaisonally be non integer
    * (see definition in printBowlFigsDefinition()).
    * This function formats output.
    */
   void printMatchBowlingFigs(const playerBowlFigsRecord &r, const int &width, std::ostream &out)
   {
      assert(width >= 15);

      // test whether runsConceded is integer
      if (r.runsConceded - static_cast<int>(r.runsConceded) == 0.0)
        // is integer
        out << std::setw(width - 3) << r.wicketsTaken << "/"
            << std::setw(        3) << r.runsConceded        << std::endl;
      else
        // is not integer
        out << std::setw(width - 8) << r.wicketsTaken << "/"
            << std::setw(        8) << r.runsConceded        << std::endl;
   }
}

/*****************************************END*OF*FILE*********************************************/
