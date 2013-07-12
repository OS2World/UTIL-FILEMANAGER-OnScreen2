/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(REGEXP_HPP)#define REGEXP_HPP#include <sysdep.hpp>#include <types.hpp>extern int	sed, nbra;class EXP_CLASS RegExp;class EXP_CLASS RegExp{public:	static int re_error;
	RegExp();	RegExp(CPCHAR s, BOOL ignoreCase = FALSE);	RegExp(const RegExp& re);	virtual ~RegExp();	RegExp& 	operator = (const RegExp& re);	RegExp& 	operator = (CPCHAR s);	int 		operator == (CPCHAR s);	int 		operator != (CPCHAR s);	BOOL 		match(CPCHAR matchStr);	CPCHAR 		FirstMatchingChar() const;
	CPCHAR		LastMatchingChar() const;
	int			MatchLen() const;
private:	PCHAR 	compile(CPCHAR inCString, PCHAR ep, PCHAR endbuf, int seof);
	int 	step(CPCHAR p1, CPCHAR p2);
	int 	advance(CPCHAR lp, CPCHAR ep);
	PCHAR	expbuf;	int		expbufSize;	int		circf;	BOOL 	ignoreCase;
	CPCHAR  loc1, loc2, locs;};#endif