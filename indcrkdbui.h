/*************************************************************************************************\
*                                                                                                 *
* "IndCrkDBui.h" -                                                                                *
*                                                                                                 *
*         Author - Tom McDonnell 2005                                                             *
*                                                                                                 *
\*************************************************************************************************/

#ifndef IND_CRK_DB_UI_H
#define IND_CRK_DB_UI_H

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "IndCrkDB.h"
#include "IndCrkRecords.h"

#include <list>

// FUNCTION DECLARATIONS //////////////////////////////////////////////////////////////////////////

bool updateDBfile(const indCrkDB &);

void writeSeasonStatsToFile(const titleRecord  &teamName,
                            const titleRecord  &oppTeamName,
                            const dateBeginEnd &,
                            const indCrkDB     &,
                            const int          &minMatches  );

void printNamesVect(const std::vector<nameRecord> &, std::ostream &);
void printTitlesVect(const std::vector<titleRecord> &, std::ostream &);

#endif

/*****************************************END*OF*FILE*********************************************/
