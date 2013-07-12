/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#pragma warn -use
static char rcsID[]="$Id: cstring.cpp 1.7 1995/04/29 07:57:49 jallen Exp jallen $";
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

const char* nullCString = "";

/**
 * 
 */
CString::CString(): len(0),s((char*)nullCString)
{
}

/**
 * 
 */
CString::CString(const char* _s): len(0)
{
	if (_s)
		len = strlen(_s);

	s = new char[len+1];

	if (_s)
		strcpy(s, _s);
	else
		s = (char*)nullCString;
}

/**
 * 
 */
CString::CString(const CString& _s)
{
	len = _s.length();
	s = new char[len+1];
	strcpy(s, (const char*)_s);
}

/**
 * 
 */
CString::CString(const char c, const int charCount)
{
	s = new char[(len = charCount)+1];
	memset(s, c, len);
	s[len] = '\0';
}

/**
 * 
 */
CString::CString(const int charCount)
{
	s = new char[(len = charCount)+1];
	memset(s, ' ', len);
	s[len] = '\0';
}

/**
 * 
 */
CString::~CString()
{
	if (s != nullCString)
		delete s;
}

/**
 * 
 */
int CString::pos(const char ch) const
{
	CPCHAR	cp = (CPCHAR)*this;

	if (*cp)
		{
		CPCHAR p  = strchr(cp, ch);

		if (p)
			{
			return int(p - cp)+1;
			}
		}

	return 0;
}

/**
 * 
 */
int CString::posr(const char ch) const
{
	CPCHAR	cp = (CPCHAR)*this;

	if (*cp)
		{
		CPCHAR p  = (char*)strrchr(cp, ch);

		if (p)
			{
			return int(p - cp)+1;
			}
		}

	return 0;
}

/**
 * 
 */
int CString::pos(CPCHAR s) const
{
	CPCHAR	cp = (CPCHAR)*this;

	if (*cp)
		{
		CPCHAR p  = strstr(cp, s);

		if (p)
			{
			return int(p - cp)+1;
			}
		}

	return 0;
}

/**
 * 
 */
int CString::posi(const char ch) const
{
	CPCHAR	cp = (CPCHAR)*this;

	if (*cp)
		{
		CPCHAR p  = strchr(cp, ch);

		if (p)
			{
			return int(p - cp)+1;
			}
		}

	return 0;
}

/**
 * 
 */
int CString::posir(const char ch) const
{					 
	CPCHAR	cp = (CPCHAR)*this;

	if (*cp)
		{
		CPCHAR p  = (char*)strrchr(cp, ch);

		if (p)
			{
			return int(p - cp)+1;
			}
		}

	return 0;
}

/**
 * 
 */
int CString::posi(CPCHAR s) const
{
	CPCHAR	cp = (CPCHAR)*this;

	if (*cp)
		{
		CPCHAR p  = strstr(cp, s);

		if (p)
			{
			return int(p - cp)+1;
			}
		}

	return 0;
}

/**
 * 
 */
int CString::length() const
{
	return len;
}

/**
 * 
 */
CString::operator const char*() const
{
	return s;
}

/**
 * 
 */
CString& CString::operator=(const CString& _s)
{
	if (s != nullCString)
		delete s;

	len = strlen((const char*)_s);
	s = new char[len+1];
	strcpy(s, (const char*)_s);

	return *this;
}

/**
 * 
 */
CString& CString::operator=(const char* _s)
{
	if (s != nullCString)
		delete s;

	len = strlen((const char*)_s);
	s = new char[len+1];
	strcpy(s, (const char*)_s);

	return *this;
}

/**
 * 
 */
int CString::operator== (const CString& _s) const
{
	return strcmp(*this, _s) == 0;
}

/**
 * 
 */
int CString::operator== (RegExp& r) const
{
	return r.match((CPCHAR)*this);
}

/**
 * 
 */
int CString::operator!= (const CString& _s) const
{
	return strcmp(*this, _s) != 0;
}

/**
 * 
 */
int CString::operator< (const CString& _s) const
{
	return strcmp(*this, _s) < 0;
}

/**
 * 
 */
int CString::operator> (const CString& _s) const
{
	return strcmp(*this, _s) > 0;
}

/**
 * 
 */
int CString::operator<= (const CString& _s) const
{
	return strcmp(*this, _s) <= 0;
}

/**
 * 
 */
int CString::operator>= (const CString& _s) const
{
	return strcmp(*this, _s) >= 0;
}

/**
 * 
 */
int CString::operator== (const char* _s) const
{
	return strcmp(*this, _s) == 0;
}

/**
 * 
 */
int CString::operator!= (const char* _s) const
{
	return strcmp(*this, _s) != 0;
}

/**
 * 
 */
int CString::operator< (const char* _s) const
{
	return strcmp(*this, _s) < 0;
}

/**
 * 
 */
int CString::operator> (const char* _s) const
{
	return strcmp(*this, _s) > 0;
}

/**
 * 
 */
int CString::operator<= (const char* _s) const
{
	return strcmp(*this, _s) <= 0;
}

/**
 * 
 */
int CString::operator>= (const char* _s) const
{
	return strcmp(*this, _s) >= 0;
}

/**
 * 
 */
CString CString::operator+ (const CString& _s) const
{
	CString tmp(length() + _s.length());

	strcpy(((char*)(const char*)tmp), *this);
	strcpy(((char*)(const char*)tmp)+length(), _s);

	return tmp;
}

/**
 * 
 */
CString CString::operator+ (const char* _s) const
{
	CString tmp(length() + (int)strlen(_s));

	strcpy(((char*)(const char*)tmp), *this);
	strcpy(((char*)(const char*)tmp)+length(), _s);

	return tmp;
}

/**
 * 
 */
CString CString::operator+ (const char& c) const
{
	CString tmp(length() + 1);

	strcpy(((char*)(const char*)tmp), *this);
	char* ep = (char*)strchr(tmp, '\0');

	*ep++ = c;
	*ep = '\0';

	return tmp;
}

/**
 * 
 */
char& CString::operator[] (const int i)
{
	MFailIf(i < 1 && i > len);
	return s[i-1];
}

/**
 * 
 */
char CString::operator[] (const int i) const
{
	MFailIf(i < 1 && i > len);
	return s[i-1];
}

/**
 * 
 */
CString copyOf(const CString& _s)
{
	return CString((const char*)_s);
}

/**
 * 
 */
CString operator+ (const char* cs, const CString& _s)
{
	int      len = strlen(cs);
	CString    tmp(len + _s.length());

	strcpy(((char*)(const char*)tmp), cs);
	strcpy(((char*)(const char*)tmp)+len, _s);

	return tmp;
}

/**
 * 
 */
SubCString::SubCString():source(CString())
{
	MFail("Default SubCString constructor called.");
}

/**
 * 
 */
SubCString::SubCString(CString& s, int index, int count):
	source(s),
	index(index),
	count(count)
{
	MFailIf(index < 1);
	MFailIf(count < 0);

	if (index + count > s.length()+1)
		{
		count = (s.length() - index)+1;
		}

	if (count > 0)
		{
		if (index <= s.length())
			{
			str = CString(count);

			memcpy((char*)(const char*)str,
								((char*)(const char*)s) + (index-1), count);

			str.s[count] = '\0';
			}
		}

}

/**
 * 
 */
SubCString::operator const CString&() const
{
   return str;
}

/**
 * 
 */
CString& SubCString::operator= (const CString& s)
{
   CString tmp = copyOf(source);

	if (index < tmp.length())
      {
		int   maxbytes = tmp.length() - index;
		int   nbytes   = count < s.length() ? count:s.length();

      memcpy(((char*)(const char*)tmp) + index,
               ((char*)(const char*)s),
               maxbytes > nbytes ? nbytes:maxbytes);
      }

   return source = tmp;
}

/**
 * 
 */
CString upperCase(const CString& s)
{
	CString us = copyOf(s);

	strupr((char*)(const char*)us);

	return us;
}

/**
 * 
 */
CString lowerCase(const CString& s)
{
	CString us = copyOf(s);

	strlwr((char*)(const char*)us);

	return us;
}

/**
 * 
 */
CString sformat(CPCHAR format ...)
{
	PCHAR    buf = new char [2048];
	CString   s;

	MFailIf(buf == 0);

	va_list va;

	va_start(va, format);

	vsprintf(buf, format, va);

	va_end(va);

	s = buf;

   delete buf;

	return s;
}

/**
 * 
 */
CString leftTrim(const CString& s)
{
	CString 	tmp = s;
	char*		pSpace = (char*)strrchr(tmp, ' ');

	if (pSpace)
		{
		*pSpace = '\0';
		tmp.len = strlen(pSpace);
		}

	return tmp;
}

/**
 * 
 */
CString rightTrim(const CString& s)
{
	char*		pSpace = (char*)(const char*)s;

	while (*pSpace && *pSpace == ' ')
		pSpace++;

	return pSpace;
}
