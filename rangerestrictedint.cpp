/*************************************************************************************************\
*                                                                                                 *
* "restrictedRangeInt.cpp" -                                                                      *
*                                                                                                 *
*                   Author - Tom McDonnell 2005                                                   *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include <TomsLibrary\restrictedRangeInt.h>

#include <TomsLibrary\misc.h>

#include <iostream>
#include <string>

#include <cstdlib>
#include <cassert>

// MEMBER FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

/*
 * Constructor.
 */
restrictedRangeInt::restrictedRangeInt(const int &mn, const int &mx)
{
   setRange(mn, mx);

   set(mn);
}

/*
 * Constructor.
 */
restrictedRangeInt::restrictedRangeInt(const int &mn, const int &mx, const int &vl)
{
   setRange(mn, mx);
   
   set(vl);
}

/*
 *
 */
void restrictedRangeInt::getFromUser(const std::string &prompt)
{
   using std::cin;
   using std::cout;
   using std::endl;

   bool success = false;
   while (!success)
   {
      try
      {
         cout << prompt;

         cin.sync();
         cin >> *this;
         success = true;
      }
      catch (restrictedRangeIntErr)
      {
         printValidUserInputRules(cout);
         cout << "Retry." << endl;
         success = false;
      }
   }
}

// FUNCTION DEFINITIONS ///////////////////////////////////////////////////////////////////////////

/*
 *
 */
std::istream &operator>>(std::istream &in, restrictedRangeInt &rrInt)
{
   eatwhite(in);

   // check for sign
   bool negative;
   switch (in.peek())
   {
    case '-': in.ignore(); negative = true;  break;
    case '+': in.ignore(); negative = false; break;
    default :              negative = false; break;
   }

   // read integer string
   char c;
   std::string s = "";
   bool haveMinimumIntString = false,
        doneReadingIntString = false;
   while (!doneReadingIntString)
   {
      in.get(c);
      if (isdigit(c))
      {
         s += c;
         haveMinimumIntString = true;
      }
      else
      {
         if (c == '\n' && haveMinimumIntString)
           doneReadingIntString = true;
         else
           throw restrictedRangeInt::restrictedRangeIntErr();
      }
   }

   // calculate integer indicated by string
   int length = s.length();
   int v = 0, dv = 0;
   for (int i = 0; i < length; ++i)
   {
      dv = (s[i] - '0') * pow(10, (length - 1) - i);

      if (negative) v -= dv;
      else          v += dv;
   }

   rrInt.set(v);

   return in;
}

// PRIVATE MEMBER FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////

restrictedRangeInt::setRange(const int &mn, const int &mx)
{
   if (mn < mx)
   {
      max = mx;
      min = mn;
   }
   else
   {
      std::cerr << "Invalid range [" << mn << ", " << mx << "] "
                << "given to constructor of restrictedRangeInt." << std::endl;

      exit(EXIT_FAILURE);
   }
}


/*****************************************END*OF*FILE*********************************************/
