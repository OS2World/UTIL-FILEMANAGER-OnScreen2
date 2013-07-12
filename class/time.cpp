/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#pragma warn -use
static char rcsID[]="$Id: time.cpp 1.5 1994/10/22 14:55:45 johna Exp jallen $";
#pragma warn +use

#include <sysdep.hpp>
#include <conditio.hpp>
#include <array.hpp>
#include <slist.hpp>
#include <dlist.hpp>
#include <cstring.hpp>
#include <date.hpp>
#include <dir.hpp>
#include <getopt.hpp>
#include <refptr.hpp>
#include <regexp.hpp>
#include <time.hpp>
#include <tracer.hpp>
#include <types.hpp>
#pragma hdrstop
#include <dos.h>

RegExp Time::timeFormat1 = "^[0-9]\\{1,2\\}[^0-9][0-9]\\{1,2\\}$";
RegExp Time::timeFormat2 = "^[0-9]\\{1,2\\}[^0-9][0-9]\\{1,2\\}[^0-9][0-9]\\{1,2\\}$";
RegExp Time::timeFormat3 = "^[0-9]\\{1,2\\}[^0-9][0-9]\\{1,2\\}[APap][mM]$";
RegExp Time::timeFormat4 = "^[0-9]\\{1,2\\}[^0-9][0-9]\\{1,2\\}[^0-9][0-9]\\{1,2\\}[APap][mM]$";

/**
 * 
 */
Time::Time():
ti(0)
{
}

/**
 * 
 */
Time::Time(const USHORT hours, const USHORT min, const USHORT seconds):
ti(0)
{
	MFailIf(hours > 24 || min > 60 || seconds > 60);

	ti = long(seconds) + (long(min)*60) + (long(hours)*60*60);
}

/**
 * 
 */
Time::Time(time_t unixTime)
{
	PCHAR	ct = ctime(&unixTime) + 11;
	USHORT hours, mins, secs;

	hours = USHORT(atoi(&ct[0]));
	mins	= USHORT(atoi(&ct[3]));
	secs	= USHORT(atoi(&ct[6]));

	*this = Time(hours, mins, secs);
}

/**
 * 
 */
Time::Time(const CString& timeStr):
ti(0)
{
	long hours=0, mins=0, seconds=0;
#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
	COUNTRY	countryInfo;

	country(0, &countryInfo);
#endif

	if (timeStr == timeFormat1)
		{
		sscanf(timeStr, "%ld%*c%ld", &hours, &mins);
		}

	if (timeStr == timeFormat2)
		{
		sscanf(timeStr, "%ld%*c%ld%*c%ld", &hours, &mins, &seconds);
		}

#if defined(__BORLANDC__) && !defined(__WIN32__)
	if (countryInfo.co_time == 0)
#else
	// TODO: get country info under DJGPP & Win32
	if (0 == 0) 
#endif
		{
		char	ampm = '\0';

		if (timeStr == timeFormat3)
			{
			sscanf(timeStr, "%ld%*c%ld%c", &hours, &mins, &ampm);
			}

		if (timeStr == timeFormat4)
			{
			sscanf(timeStr, "%ld%*c%ld%*c%ld%c", &hours, &mins, &seconds, &ampm);
			}

		if ((ampm == 'p' || ampm == 'P') && hours < 12)
			{
			hours += 12;
			}

		if ((ampm == 'a' || ampm == 'A') && hours == 12)
			{
			hours = 0;
			}
		}

	ti = long(seconds) + (long(mins)*60) + (long(hours)*60*60);
}

/**
 * 
 */
Time& Time::operator += (long seconds)
{
	ti += seconds;

	return *this;
}

/**
 * 
 */
Time& Time::operator -= (long seconds)
{
	ti -= seconds;

	return *this;
}

/**
 * 
 */
int Time::operator == (const Time& t) const
{
	return ti == t.ti;
}

/**
 * 
 */
int Time::operator != (const Time& t) const
{
	return ti != t.ti;
}

/**
 * 
 */
int Time::operator <  (const Time& t) const
{
	return ti < t.ti;
}

/**
 * 
 */
int Time::operator >  (const Time& t) const
{
	return ti > t.ti;
}

/**
 * 
 */
int Time::operator <= (const Time& t) const
{
	return ti <= t.ti;
}

/**
 * 
 */
int Time::operator >= (const Time& t) const
{
	return ti >= t.ti;
}

/**
 * 
 */
long Time::operator - (const Time& t) const
{
	return ti - t.ti;
}

/**
 * 
 */
Time::operator long() const
{
	return ti;
}

/**
 * 
 */
Time::operator CString() const
{
#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
	COUNTRY	countryInfo;

	country(0, &countryInfo);
#endif

#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
	switch (countryInfo.co_time)
#else
	switch (0)
#endif		
		{
		case 1:
			{
			MTrace(("Time=%s", (const char*)sformat("%02i%s%02i", hours(), ":", minutes())));
			return sformat("%02i%s%02i", hours(), ":", minutes());
			}

		case 0:
			{
			if (hours() == 0)
				{
				MTrace(("Time=%s", (const char*)sformat("12%s%02iam", ":", minutes())));
				return sformat("12%s%02iam", ":", minutes());
				}

			if (hours() == 12)
				{
				MTrace(("Time=%s", (const char*)sformat("12%s%02ipm", ":", minutes())));
				return sformat("12%s%02ipm", ":", minutes());
				}

			if (hours() > 12)
				{
				MTrace(("Time=%s", (const char*)sformat("%02i%s%02ipm", hours()-12, ":", minutes())));
				return sformat("%02i%s%02ipm", hours()-12, ":", minutes());
				}

			if (hours() < 12)
				{
				MTrace(("Time=%s", (const char*)sformat("%02i%s%02iam", hours(), ":", minutes())));
				return sformat("%02i%s%02iam", hours(), ":", minutes());
				}
			}
		}

	return CString();
}

/**
 * 
 */
CString Time::shortCString() const
{
	return CString(*this);
}

/**
 * 
 */
CString Time::longCString() const
{
#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
	COUNTRY	countryInfo;

	country(0, &countryInfo);
#endif

#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
	switch (countryInfo.co_time)
#else	
	switch (0)
#endif
		{
		case 1:
			return sformat("%02i%s%02i%s%02i", hours(), ":", minutes(), ":", seconds());

		case 0:
			{
			if (hours() == 0)
				{
				return sformat("12%s%02i%s%02iam", ":", minutes(), ":", seconds());
				}

			if (hours() == 12)
				{
				return sformat("12%s%02i%s%02ipm", ":", minutes(), ":", seconds());
				}

			if (hours() > 12)
				{
				return sformat("%02i%s%02i%s%02ipm", hours()-12, ":", minutes(), ":", seconds());
				}

			if (hours() < 12)
				{
				return sformat("%02i%s%02i%s%02iam", hours(), ":", minutes(), ":", seconds());
				}
			}
		}

	return CString();
}

/**
 * 
 */
USHORT Time::hours() const
{
	return USHORT(ti / (60l * 60l));
}

/**
 * 
 */
USHORT Time::minutes() const
{
	return USHORT((ti - (hours() * 60l * 60l)) / 60);
}

/**
 * 
 */
USHORT Time::seconds() const
{
	return USHORT(ti - ((hours() * 60l * 60l) + (minutes() * 60l)));
}

/**
 * 
 */
Time EXP_FUNC systemTime()
{
	return Time(time(NULL));
}
