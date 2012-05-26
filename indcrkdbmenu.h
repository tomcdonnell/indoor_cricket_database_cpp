/*************************************************************************************************\
*                                                                                                 *
* "IndCrkDBmenu.h" -                                                                              *
*                                                                                                 *
*           Author - Tom McDonnell 2005                                                           *
*                                                                                                 *
\*************************************************************************************************/

#ifndef IND_CRK_DB_MENU_H
#define IND_CRK_DB_MENU_H

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "IndCrkDB.h"

// FUNCTION DECLARATIONS //////////////////////////////////////////////////////////////////////////

void menuMainMenu(void);
void menuPlayerStats(const indCrkDB &);
void menuTeamStats(const indCrkDB &);
void menuTeamRankPlayers(const titleRecord  &teamName,
                         const titleRecord  &oppTeamName,
                         const dateBeginEnd &,
                         const indCrkDB     &            );
void menuTeamRankPlayerAverages(const titleRecord              &teamName,
                                const titleRecord              &oppTeamName,
                                const dateBeginEnd             &,
                                const indCrkDB                 &,
                                std::list<playerRecordsStruct> &,
                                const int                      &minMatches  );
void menuTeamRankPlayerTotals(const titleRecord              &teamName,
                              const titleRecord              &oppTeamName,
                              const dateBeginEnd             &,
                              const indCrkDB                 &,
                              std::list<playerRecordsStruct> &,
                              const int                      &minMatches  );
void menuTeamRankPlayerBests(const titleRecord              &teamName,
                             const titleRecord              &oppTeamName,
                             const dateBeginEnd             &,
                             const indCrkDB                 &,
                             std::list<playerRecordsStruct> &,
                             const int                      &minMatches  );
void menuTeamRankPlayerWorsts(const titleRecord              &teamName,
                              const titleRecord              &oppTeamName,
                              const dateBeginEnd             &,
                              const indCrkDB                 &,
                              std::list<playerRecordsStruct> &,
                              const int                      &minMatches  );

void menuMaintainDB(indCrkDB &);

#endif

/*****************************************END*OF*FILE*********************************************/
