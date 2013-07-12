/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(CSTRING_HPP)
#define CSTRING_HPP

#include <sysdep.hpp>
#include <conditio.hpp>
#include <regexp.hpp>
#include <array.hpp>

class EXP_CLASS CString;
class EXP_CLASS SubCString;

/**
 * 
 */
class EXP_CLASS CString
{
public:
	CString();
	CString(const CString& s);
	CString(const char c, const int charCount = 1);
	CString(const int charCount);
	CString(const char* s);
   ~CString();

	// overloaded operators. These operators modify the object
	CString& operator=(const CString& s);
	CString& operator=(const char* s);
	CString  operator+ (const char& ch) const;
	CString  operator+ (const char* s) const;
	CString  operator+ (const CString& s) const;

	// overloaded operators. These operators do not modify the object
	int operator== (const CString& s) const;
	int operator!= (const CString& s) const;
	int operator< (const CString& s) const;
	int operator> (const CString& s) const;
	int operator<= (const CString& s) const;
	int operator>= (const CString& s) const;

	int operator== (const char* s) const;
	int operator!= (const char* s) const;
	int operator< (const char* s) const;
	int operator> (const char* s) const;
	int operator<= (const char* s) const;
	int operator>= (const char* s) const;

	int operator== (RegExp& r) const;

	char& operator[] (const int i);
	char operator[] (const int i) const;

	// conversion operators
	operator const char*() const;

	// methods
	int   length() const;
	int	pos(const char ch) const;
	int	posr(const char ch) const;
	int	pos(CPCHAR s) const;
	int	posi(const char ch) const;
	int	posir(const char ch) const;
	int	posi(CPCHAR s) const;

private:
	int	len;
	char*	s;

	friend class SubCString;
	friend CString leftTrim(const CString& s);
	friend CString rightTrim(const CString& s);
};

CString EXP_FUNC operator+(const char* cs, const CString& s);
CString EXP_FUNC copyOf(const CString& s);
CString EXP_FUNC upperCase(const CString& s);
CString EXP_FUNC lowerCase(const CString& s);
CString EXP_FUNC sformat(CPCHAR format ...);
CString EXP_FUNC leftTrim(const CString& s);
CString EXP_FUNC rightTrim(const CString& s);

/**
 * 
 */
class EXP_CLASS SubCString
{
public:
	SubCString();
	SubCString(CString& s, int index, int count);

	operator const CString&() const;

	CString& operator= (const CString& s);
protected:
private:
	CString    	str;
	CString&   	source;
	int			index, count;
};

#include <refptr.hpp>
typedef RefPtr<CString> RCString;
typedef RefPtr<SubCString> RSubCString;
Array<CString>;

#endif
