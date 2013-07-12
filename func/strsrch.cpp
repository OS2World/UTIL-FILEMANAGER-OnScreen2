/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <sysdep.hpp>
#include <ctype.h>
#include <jac.h>
#include <conio.h>
#include <time.h>

#pragma warn -use
static char rcsID[]="$Id: strsrch.cpp 1.7 1995/06/19 10:57:34 jallen Exp jallen $";
#pragma warn +use

/**
 * 
 */
int strnrcmp(const char* s1, const char* s2, int len)
{
	s1 += len-1;

	while ( --len && *s1 == *s2)
		{
		s1--;
		s2--;
		}

	return *s1-*s2;
}

/**
 * 
 */
int strnrcmpi(const char* s1, const char* s2, int len)
{
	s1 += len-1;

	while (--len && toupper(*s1) == toupper(*s2))
		{
		s1--;
		s2--;
		}

	return toupper(*s1)-toupper(*s2);
}

/**
 * 
 */
const char* EXP_FUNC strsrch(const char* s, const SearchExpression& exp)
{
	if (exp.iStrLen == 0)
		{
		// fail is string is NULL
		return 0;
		}
	else
	if (exp.iStrLen == 1)
		{
		// quick character scan if string is one char long
		if (exp.iCaseSensitive)
			return strchr(s, *exp.iString);
		else
			return strchri(s, *exp.iString);
		}

	int 		sinclen = exp.iStrLen;
	const char*	pend = s+(sinclen-1);
	const char* peos = s+(strlen(s)-1);

	for (;;)
		{
		// Have we gone beyond the end of string
		if (pend > peos)
			break;

		int	c = (unsigned char)*pend;

		if (!exp.iCaseSensitive)
			c = toupper(c);

		int lastOcc = exp.iLastOccurance[c];

		if (exp.iLastChar != c)
			{
			if (lastOcc != -1)
				{
				pend += lastOcc;
				}
			else
				{
				pend += sinclen;
				}
			}
		else
			{
			if (exp.iCaseSensitive)
				{
				if (strnrcmp(exp.iString, pend, exp.iStrLen) == 0)
					return pend-(sinclen-1);
				else
					{
					pend++;
					}
				}
			else
				{
				if (strnrcmpi(exp.iString, pend, exp.iStrLen) == 0)
					return pend-(sinclen-1);
				else
					{
					pend++;
					}
				}
			}
		}

	return 0;
}

/**
 * 
 */
SearchExpression::SearchExpression(const char* s, BOOL caseInSensitive)
{
    int i;

	iCaseSensitive = !caseInSensitive;
	iString = strdup(s);
	iStrLen = strlen(s);
	iLastChar = s[iStrLen-1];

	if (!iCaseSensitive)
		iLastChar = toupper(iLastChar);

	for (i=0; i < 256; i++)
		{
		iLastOccurance[i] = -1;
		}

	for (i=0; *s; i++, s++)
		{
		int	ch;

		if (iCaseSensitive)
			{
			ch = (unsigned char)*s;
			}
		else
			{
			ch = (unsigned char)toupper(*s);
			}

		iLastOccurance[ch] = iStrLen-(i+1);
		}
}

/**
 * 
 */
SearchExpression::~SearchExpression()
{
	if (iString)
		delete iString;
}
