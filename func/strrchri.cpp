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
static char rcsID[]="$Id: strrchri.cpp 1.2 1994/10/22 14:55:52 johna Exp jallen $";
#pragma warn +use

char* EXP_FUNC strrchri(const char *p, int c)
{
	char *q;

	for (q = (char *)(p + strlen(p)); toupper(*q) != toupper(c); q--)
   	{
		if (q == p)
      	return (NULL);
		}

	return q;
}
