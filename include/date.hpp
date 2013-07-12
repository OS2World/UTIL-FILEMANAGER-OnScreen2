/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(DATE_HPP)
#define DATE_HPP

#include <sysdep.hpp>
#include <time.h>

// Date_t must not be an unsigned quantity
typedef SHORT Date_t;

#define  MAX_MONTHS  12
#define  MAX_DAYS    7

class EXP_CLASS Date;

Date EXP_FUNC systemDate();

enum DaySymbolicName
{
	Monday=1,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
};

/**
 * 
 */
class Date
{
public:
	Date();
	Date(time_t unixDate);
	Date(Date_t  julianDate);
	Date(const CString& dateStr);
	Date(SHORT day, SHORT month, SHORT year);
	Date(SHORT day, PCHAR monthStr, SHORT year);

	friend   Date systemDate();
	Date& 	operator += (SHORT dayCount);
	Date& 	operator -= (SHORT dayCount);
	int 	   operator == (const Date& dt) const;
	int 	   operator != (const Date& dt) const;
	int 	   operator <  (const Date& dt) const;
	int 	   operator >  (const Date& dt) const;
	int 	   operator <= (const Date& dt) const;
	int 	   operator >= (const Date& dt) const;
	Date_t   operator - (const Date& dt) const;
	operator CString() const;
	operator Date_t() const;
	SHORT    dayOfWeek() const;
	SHORT    day() const;
	SHORT    month() const;
	SHORT    year() const;
	CString  dayStr() const;
	CString  monthStr() const;
	CString	 shortCString() const;
	CString	 longCString() const;
	Date_t   operator ++ () { *this += 1; return *this; }
	Date_t   operator -- () { *this -= 1; return *this; }

	SHORT    daysInMonth(SHORT whichMonth = 0, SHORT whichYear = 0) const;
	SHORT    daysInYear(SHORT whichYear = 0) const;
	BOOL     isLeapYear(SHORT whichYear = 0) const;

	static SHORT    gDaysInMonth(SHORT whichMonth, SHORT whichYear);
	static SHORT    gDaysInYear(SHORT whichYear);
	static BOOL     gIsLeapYear(SHORT whichYear);
	static BOOL     gIsValidDate(const CString& dateStr);
	static BOOL     gIsValidDate(SHORT day, SHORT month, SHORT year);
	static BOOL     gIsValidDate(SHORT day, PCHAR monthStr, SHORT year);

private:
	static const SHORT	startYear;
	static const SHORT	maxYear;
	static const SHORT	monthsPerYear;
	static const SHORT	daysPerWeek;
	static const SHORT	numDays[MAX_MONTHS];
	static CPCHAR			dayName[MAX_DAYS];
	static CPCHAR 			monthName[MAX_MONTHS];
	static RegExp			ddmmyyFormat;
	static RegExp			ddmmmyyFormat1;
	static RegExp			ddmmmyyFormat2;
	static RegExp			ddmmmyyFormat3;
	static RegExp			ddmmmyyFormat4;

	Date_t	julianDate;
	SHORT 	_day;
	SHORT 	_month;
	SHORT 	_year;

	void  setDayMonthYear();
	void	setTH(CString& str, int pos) const;
};

#endif
