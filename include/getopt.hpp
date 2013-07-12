/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(GETOPT_HPP)
#define GETOPT_HPP

#include <sysdep.hpp>
#include <cstring.hpp>

#define EOO (-1)

struct option
{
	enum {
		NO_ARGS,
		CAN_HAVE_ARGS,
		REQUIRES_ARGS
		} argtype;

	int   optno;
	PCHAR text;
	PCHAR desc;
	PCHAR example;
};

int 	EXP_FUNC isSwitch(char Ch);
void 	EXP_FUNC printOptions(option opts[]);
int 	EXP_FUNC getopt(int& argno, char *argv[], option opts[]);
CString EXP_FUNC getoptArg();
void	EXP_FUNC getoptErr(BOOL state = FALSE);

#endif

//*[l*--------------------------------oOo-----------------------------------//
