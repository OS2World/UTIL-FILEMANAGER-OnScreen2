/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(JAC_H)
#define JAC_H

#define COPR		"(c) Copyright. 1993-2006\n"\
						"John Allen. Released under the GNU GPL v2 or later.\n";

#include <sysdep.hpp>
#include <types.hpp>

const char* EXP_FUNC getRCSVersion(const char* rcsID);
char* EXP_FUNC strstri(const char *str1, const char *str2);
char* EXP_FUNC strrchri(const char *p, int c);
char* EXP_FUNC strchri(const char *p, int c);

class EXP_CLASS SearchExpression
{
public:
	SearchExpression(const char* s, BOOL caseInSensitive = FALSE);
	~SearchExpression();

private:
	BOOL	iCaseSensitive;
	int	iLastOccurance[256];
	char*	iString;
	int	iStrLen;
	int	iLastChar;

	friend const char* strsrch(const char* s, const SearchExpression& exp);
};

const char* EXP_FUNC strsrch(const char* s, const SearchExpression& exp);

#endif

/**[l*--------------------------------oOo-----------------------------------*/
