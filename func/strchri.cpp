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
static char rcsID[]="$Id: strchri.cpp 1.3 1995/01/31 23:25:45 jallen Exp jallen $";
#pragma warn +use

char* EXP_FUNC strchri(const char *p, int c)
{
	char *q;

	c = toupper(c);

	for (q = (char*)p; toupper(*q) != c && *q; q++)
		;

	if (*q == '\0' && c != '\0')
		return NULL;

	return q;
}
