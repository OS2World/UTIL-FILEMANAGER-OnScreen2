/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <sysdep.hpp>
#include <ctype.h>
#include <jac.h>

#pragma warn -use
static char rcsID[]="$Id: strstri.cpp 1.2 1994/10/22 14:55:53 johna Exp jallen $";
#pragma warn +use

char* EXP_FUNC strstri(const char *str1, const char *str2)
{
	int len1 = strlen(str1);
   int len2 = strlen(str2);
   int i,j,k;

   if (!len2)
   	return (char *)str1;    /* return str1 if str2 empty */
   
	if (!len1)
      return 0;               /* return NULL if str1 empty */

	i = 0;

   for (;;)
		{
		char c = char(toupper(str2[0]));

		while (i < len1 && toupper(str1[i]) != c)
      	++i;

		if (i == len1)
      	return 0;

		j = 0;
      k = i;
      
		while (i < len1 && j < len2 && toupper(str1[i]) == toupper(str2[j]))
      	{
         ++i;
         ++j;
         }

		if (j == len2)
      	return (char *)str1+k;

		if (i == len1)
      	return 0;
		}
}

//*[l*--------------------------------oOo-----------------------------------//
