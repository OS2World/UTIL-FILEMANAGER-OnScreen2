/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(REGEXP_HPP)

	CPCHAR		LastMatchingChar() const;
	int			MatchLen() const;

	int 	step(CPCHAR p1, CPCHAR p2);
	int 	advance(CPCHAR lp, CPCHAR ep);

	CPCHAR  loc1, loc2, locs;