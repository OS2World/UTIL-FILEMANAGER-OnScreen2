/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#pragma warn -use
static char rcsID[]="$Id: date.cpp 1.4 1994/10/22 14:55:26 johna Exp jallen $";
#pragma warn +use

//------------------------------------oOo-----------------------------------//

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
#include <limits.h>
#include <dos.h>

const SHORT Date::startYear   = 1970;
const SHORT Date::maxYear     = SHORT(((SHRT_MAX / 366) + Date::startYear) - 1);

const SHORT Date::numDays[MAX_MONTHS]   = {31,28,31,30,31,30,31,31,30,31,30,31};
CPCHAR Date::dayName[MAX_DAYS]   = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
CPCHAR Date::monthName[MAX_MONTHS] = 	{
											"Jan","Feb","Mar","Apr","May","Jun","Jul",
											"Aug","Sep","Oct","Nov","Dec"
											};

const SHORT	Date::monthsPerYear	= 12;
const SHORT	Date::daysPerWeek		= 7;
RegExp		Date::ddmmyyFormat	= "^[0-9]\\{1,\\}[^0-9][0-9]\\{1,\\}[^0-9][0-9]\\{1,\\}$";
RegExp		Date::ddmmmyyFormat1 = "^[0-9]\\{1,\\}[nstr]\\{1\\}[dht]\\{1\\} [a-zA-Z]\\{3\\} [0-9]\\{1,\\}$";
RegExp		Date::ddmmmyyFormat2 = "^[0-9]\\{1,\\} [a-zA-Z]\\{3\\} [0-9]\\{1,\\}$";
RegExp		Date::ddmmmyyFormat3 = "^[a-zA-Z]\\{3\\} [0-9]\\{1,\\}[nstr]\\{1\\}[dht]\\{1\\} [0-9]\\{1,\\}$";
RegExp		Date::ddmmmyyFormat4 = "^[a-zA-Z]\\{3\\} [0-9]\\{1,\\} [0-9]\\{1,\\}$";

/**
 * 
 */
Date::Date(const CString& dateStr):
julianDate(0),
_day(0),
_month(0),
_year(0)
{
	MStackCall(__FILE__,__LINE__);

#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
	COUNTRY	countryInfo;

	country(0, &countryInfo);
#endif

	if (dateStr == ddmmyyFormat)
		{
#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
		switch (countryInfo.co_date)
#else
		switch (1)
#endif
			{
			case 0:
				{
				sscanf(dateStr, "%hd%*c%hd%*c%hd", &_month, &_day, &_year);
				}
			break;

			case 1:
				{
				sscanf(dateStr, "%hd%*c%hd%*c%hd", &_day, &_month, &_year);
				}
			break;

			case 2:
				{
				sscanf(dateStr, "%hd%*c%hd%*c%hd", &_year, &_month, &_day);
				}
			break;
			}

		*this = Date(_day, _month, _year);

		return;
		}

	char _monthStr[4];

	_monthStr[3] = '\0';

	if (dateStr == ddmmmyyFormat1)
		{
		sscanf(dateStr, "%hd%*c%*c %3c %hd", &_day, &_monthStr, &_year);
		*this = Date(_day, _monthStr, _year);

		return;
		}

	if (dateStr == ddmmmyyFormat2)
		{
		sscanf(dateStr, "%hd %3c %hd", &_day, &_monthStr, &_year);
		*this = Date(_day, _monthStr, _year);

		return;
		}

	if (dateStr == ddmmmyyFormat3)
		{
		sscanf(dateStr, "%3c %hd%*c%*c %hd", &_monthStr, &_day, &_year);
		*this = Date(_day, _monthStr, _year);

		return;
		}

	if (dateStr == ddmmmyyFormat4)
		{
		sscanf(dateStr, "%3c %hd %hd", &_monthStr, &_day, &_year);
		*this = Date(_day, _monthStr, _year);

		return;
		}
}

/**
 * 
 */
Date::Date():
julianDate(0),
_day(0),
_month(0),
_year(0)
{
	MStackCall(__FILE__,__LINE__);
	setDayMonthYear();
}

/**
 * 
 */
Date::Date(time_t unixDate)
{
	MStackCall(__FILE__,__LINE__);
	PCHAR	ct = ctime(&unixDate);
	char 	mon[4];

	sscanf(ct, "%*s %s %hd %*hd:%*hd:%*hd %hd", mon, &_day, &_year);

	for (SHORT i = 0; i < Date::monthsPerYear; i++)
		{
		if (stricmp(mon, Date::monthName[i]) == 0)
			{
			_month = SHORT(i + 1);
			break;
			}
		}

	*this = Date(_day, _month, _year);
}

/**
 * 
 */
Date::Date(Date_t julianDate):
julianDate(julianDate),
_day(0),
_month(0),
_year(0)
{
	MStackCall(__FILE__,__LINE__);
	julianDate = julianDate > Date_t(0) ? julianDate : Date_t(0);

	setDayMonthYear();
}

/**
 * 
 */
Date::Date(SHORT _day, SHORT _month, SHORT _year):
julianDate(0),
_day(0),
_month(0),
_year(0)
{
	MStackCall(__FILE__,__LINE__);
	MFailIf(_year < startYear || _year > maxYear);

	if (_year < startYear || _year > maxYear)
		{
		return;
		}

	this->_day	= _day;
	this->_month	= _month;
	this->_year	= _year;

	if (_year >= startYear && _month > 0 && _month <= 12 && _day > 0 && _day <= daysInMonth())
	  {
	  SHORT i;

		for (i=startYear; i < _year; i++)
			{
			julianDate += daysInYear(i);
         }

		for (i=1; i < _month; i++)
			{
			julianDate += daysInMonth(i, _year);
			}

		julianDate += _day;
		}
}

/**
 * 
 */
Date::Date(SHORT _day, PCHAR _monthStr, SHORT _year):
julianDate(0),
_day(0),
_month(0),
_year(0)

{
	MStackCall(__FILE__,__LINE__);
	for (int i=0; i < monthsPerYear; i++)
		{
		if (stricmp(_monthStr, monthName[i]) == 0)
			{
			_month = SHORT(i + 1);

			break;
			}
		}

	*this = Date(_day, _month, _year);
}

/**
 * 
 */
BOOL Date::gIsValidDate(const CString& dateStr)
{
	MStackCall(__FILE__,__LINE__);

	SHORT		day=0, month=0, year=0;
#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
	COUNTRY	countryInfo;

	country(0, &countryInfo);
#endif

	if (dateStr == ddmmyyFormat)
		{
#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
		switch (countryInfo.co_date)
#else
		switch (1)
#endif
			{
			case 0:
				{
				sscanf(dateStr, "%hd%*c%hd%*c%hd", &month, &day, &year);
				}
			break;

			case 1:
				{
				sscanf(dateStr, "%hd%*c%hd%*c%hd", &day, &month, &year);
				}
			break;

			case 2:
				{
				sscanf(dateStr, "%hd%*c%hd%*c%hd", &year, &month, &day);
				}
			break;
			}

		return gIsValidDate(day, month, year);
		}

	char _monthStr[4];

	_monthStr[3] = '\0';

	if (dateStr == ddmmmyyFormat1)
		{
		sscanf(dateStr, "%hd%*c%*c %3c %hd", &day, &_monthStr, &year);

		return gIsValidDate(day, _monthStr, year);
		}

	if (dateStr == ddmmmyyFormat2)
		{
		sscanf(dateStr, "%hd %3c %hd", &day, &_monthStr, &year);

		return gIsValidDate(day, _monthStr, year);
		}

	if (dateStr == ddmmmyyFormat3)
		{
		sscanf(dateStr, "%3c %hd%*c%*c %hd", &_monthStr, &day, &year);

		return gIsValidDate(day, _monthStr, year);
		}

	if (dateStr == ddmmmyyFormat4)
		{
		sscanf(dateStr, "%3c %hd %hd", &_monthStr, &day, &year);

		return gIsValidDate(day, _monthStr, year);
		}

	return FALSE;
}

/**
 * 
 */
BOOL Date::gIsValidDate(SHORT day, SHORT month, SHORT year)
{
	MStackCall(__FILE__,__LINE__);

	if (year < startYear || year > maxYear)
		return FALSE;

	if (month < 1 || month > 12)
		return FALSE;

	if (day < 1 || day > gDaysInMonth(month, year))
		return FALSE;

	return TRUE;
}

/**
 * 
 */
BOOL Date::gIsValidDate(SHORT day, PCHAR monthStr, SHORT year)
{
	MStackCall(__FILE__,__LINE__);
	SHORT month = 0;

	for (int i=0; i < monthsPerYear; i++)
		{
		if (stricmp(monthStr, monthName[i]) == 0)
			{
			month = SHORT(i + 1);

			break;
			}
		}

	return gIsValidDate(day, month, year);
}

/**
 * 
 */
SHORT Date::daysInMonth(SHORT whichMonth, SHORT whichYear) const
{
	MStackCall(__FILE__,__LINE__);
	if (whichYear == 0)
		whichYear = _year;

	if (whichMonth == 0)
		whichMonth = _month;

	return gDaysInMonth(whichMonth, whichYear);
}

/**
 * 
 */
SHORT Date::gDaysInMonth(SHORT whichMonth, SHORT whichYear)
{
	MStackCall(__FILE__,__LINE__);

	if (whichMonth == 2)
		{
		if (gIsLeapYear(whichYear))
			return SHORT(numDays[whichMonth-1]+1);
		else
			return numDays[whichMonth-1];
		}
	else
		{
		return numDays[whichMonth-1];
		}
}

/**
 * 
 */
SHORT Date::gDaysInYear(SHORT whichYear)
{
	MStackCall(__FILE__,__LINE__);

	return (gIsLeapYear(whichYear) ? SHORT(366) : SHORT(365));
}

/**
 * 
 */
SHORT Date::daysInYear(SHORT whichYear) const
{
	MStackCall(__FILE__,__LINE__);
	if (whichYear == 0)
		whichYear = _year;

	return gDaysInYear(whichYear);
}

/**
 * 
 */
BOOL Date::gIsLeapYear(SHORT whichYear)
{
	MStackCall(__FILE__,__LINE__);

	if (
		 (whichYear % 4) == 0 &&
		 (whichYear % 100) != 0 ||
		 (whichYear % 400) == 0
		)
		return TRUE;
	else
		return FALSE;
}

/**
 * 
 */
BOOL Date::isLeapYear(SHORT whichYear) const
{
	MStackCall(__FILE__,__LINE__);
	if (whichYear == 0)
		whichYear = _year;

	return gIsLeapYear(whichYear);
}

/**
 * 
 */
Date::operator Date_t () const
{
	MStackCall(__FILE__,__LINE__);
	return julianDate;
}

/**
 * 
 */
Date& Date::operator += (SHORT n_days)
{
	MStackCall(__FILE__,__LINE__);
	julianDate += n_days;

	setDayMonthYear();

   return *this;
}

/**
 * 
 */
Date& Date::operator -= (SHORT n_days)
{
	MStackCall(__FILE__,__LINE__);
	julianDate -= n_days;

	setDayMonthYear();

   return *this;
}

/**
 * 
 */
int Date::operator == (const Date& dt) const
{
	MStackCall(__FILE__,__LINE__);
	return dt.julianDate == julianDate;
}

/**
 * 
 */
int Date::operator != (const Date& dt) const
{
	MStackCall(__FILE__,__LINE__);
	return dt.julianDate != julianDate;
}

/**
 * 
 */
int Date::operator < (const Date& dt) const
{
	MStackCall(__FILE__,__LINE__);
	return julianDate < dt.julianDate;
}

/**
 * 
 */
int Date::operator > (const Date& dt) const
{
	MStackCall(__FILE__,__LINE__);
	return julianDate > dt.julianDate;
}

/**
 * 
 */
int Date::operator <= (const Date& dt) const
{
	MStackCall(__FILE__,__LINE__);
	return julianDate <= dt.julianDate;
}

/**
 * 
 */
int Date::operator >= (const Date& dt) const
{
	MStackCall(__FILE__,__LINE__);
	return julianDate >= dt.julianDate;
}

/**
 * 
 */
Date_t Date::operator - (const Date& dt) const
{
	MStackCall(__FILE__,__LINE__);
	return Date_t(julianDate - dt.julianDate);
}

/**
 * 
 */
Date systemDate()
{
	return(time(NULL));
}

/**
 * 
 */
SHORT Date::dayOfWeek() const
{
	MStackCall(__FILE__,__LINE__);
	Date RefDate(7,1,1980);  // This is a Monday

#if defined(__GNUG__)
	long n_days = abs(RefDate - *this);
#else
	long n_days = labs(RefDate - *this);
#endif

	return SHORT((n_days % 7)+1);
}

/**
 * 
 */
SHORT Date::year() const
{
	MStackCall(__FILE__,__LINE__);
	return _year;
}

/**
 * 
 */
SHORT Date::month() const
{
	MStackCall(__FILE__,__LINE__);
	return _month;
}

/**
 * 
 */
CString Date::dayStr() const
{
	MStackCall(__FILE__,__LINE__);
	return dayName[dayOfWeek()-1];
}

/**
 * 
 */
CString Date::monthStr() const
{
	MStackCall(__FILE__,__LINE__);
	if (julianDate != 0)
		return monthName[_month];
	else
		return CString();
}

/**
 * 
 */
Date::operator CString() const
{
	MStackCall(__FILE__,__LINE__);
	CString	str;

#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
	COUNTRY	countryInfo;

	country(0, &countryInfo);
#endif

	if (julianDate != 0)
		{
#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
		switch (countryInfo.co_date)
#else
		switch (1)
#endif
			{
			case 0:
				{
				str = sformat("%02d%s%02d%s%04d", _month, "-", _day, "-", _year);
				}
			break;

			case 1:
				{
				str = sformat("%02d%s%02d%s%04d", _day, "-", _month, "-", _year);
				}
			break;

			case 2:
				{
				str = sformat("%04d%s%02d%s%02d", _year, "-", _month, "-", _day);
				}
			break;
			}
		}

	return str;
}

/**
 * 
 */
CString Date::shortCString() const
{
	MStackCall(__FILE__,__LINE__);
	return CString(*this);
}

/**
 * 
 */
CString Date::longCString() const
{
	MStackCall(__FILE__,__LINE__);
	CString	str;

#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
	COUNTRY	countryInfo;

	country(0, &countryInfo);
#endif

	if (julianDate != 0)
		{
#if defined(__BORLANDC__) && !defined(__WIN32__)
	// TODO: get country info under DJGPP & Win32
		switch (countryInfo.co_date)
#else
		switch (1)
#endif
			{
			case 0:
				{
				str = sformat("%s %s %02d   %04d", (CPCHAR)dayStr(), monthName[_month-1], _day, _year);
				setTH(str, 11);
				}
			break;

			case 1:
			case 2:
				{
				str = sformat("%s %02d   %s %04d", (CPCHAR)dayStr(), _day, monthName[_month-1], _year);
				setTH(str, 7);
				}
			break;
			}
		}

	return str;
}

/**
 * 
 */
void Date::setDayMonthYear()
{
	MStackCall(__FILE__,__LINE__);
	if (julianDate == 0)
		return;

	Date_t   i  = 0;

	_day      = 0;
	_month    = 0;
	_year     = startYear - 1;

	while (i < julianDate)
		{
		_year++;

		i += daysInYear();
		}

	i -= daysInYear(_year);

	while (i < julianDate)
		{
		_month++;

		i += daysInMonth();
		}

	i -= daysInMonth();

	_day = SHORT(julianDate - i);
}

/**
 * 
 */
void Date::setTH(CString& str, int pos) const
{
	MStackCall(__FILE__,__LINE__);

	switch (_day)
		{
		case 1:
		case 21:
		case 31:
			{
			str[pos] = 's';
			str[pos+1] = 't';
			}
		break;

		case 2:
		case 22:
			{
			str[pos] = 'n';
			str[pos+1] = 'd';
			}
		break;

		case 3:
		case 23:
			{
			str[pos] = 'r';
			str[pos+1] = 'd';
			}
		break;

		default:
			{
			str[pos] = 't';
			str[pos+1] = 'h';
			}
		break;
		}
}
