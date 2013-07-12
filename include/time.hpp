/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(TIME_HPP)
#define TIME_HPP

#include <time.h>
#include <cstring.hpp>

class EXP_CLASS Time;

/**
 * 
 */
class EXP_CLASS Time
{
public:
	Time();
	Time(time_t unixTime);
	Time(const USHORT hours, const USHORT min, const USHORT seconds);
	Time(const CString& timeStr);

	friend   Time 	EXP_FUNC systemTime();
	Time& 	operator += (long seconds);
	Time& 	operator -= (long seconds);
	int 	   operator == (const Time& t) const;
	int 	   operator != (const Time& t) const;
	int 	   operator <  (const Time& t) const;
	int 	   operator >  (const Time& t) const;
	int 	   operator <= (const Time& t) const;
	int 	   operator >= (const Time& t) const;
	long		operator - (const Time& t) const;
	operator long() const;
	operator CString() const;
	CString	shortCString() const;
	CString	longCString() const;
	USHORT	hours() const;
	USHORT	minutes() const;
	USHORT	seconds() const;
	Time		operator ++ () { *this += 1; return *this; }
	Time		operator -- () { *this -= 1; return *this; }

private:
	static	RegExp	timeFormat1;
	static	RegExp	timeFormat2;
	static	RegExp	timeFormat3;
	static	RegExp	timeFormat4;

	long		ti;
};

#endif
