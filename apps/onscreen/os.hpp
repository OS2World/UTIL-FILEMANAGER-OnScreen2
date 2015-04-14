/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(OS_HPP)
#define OS_HPP

#include <sysdep.hpp>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <process.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>
#include <jac.h>
#include <getopt.hpp>
#include <dir.hpp>
#include <slist.hpp>
#include <set.hpp>

/**
 * Globals 
 */

extern const int 		gMessageClearTime;
extern PCHAR 			gOnScreenRevision;
extern CPCHAR 			gOnScreenCopyright;

/**
 * 
 */
enum DisplayMode
{
	DM_TEXT,
	DM_HEX,
	DM_LAST
};

/**
 * 
 */
#define BrandSigV2 	'O' + 100, \
							'S' + 100, \
							'V' + 100, \
							'2' + 100, \
							'.' + 100, \
							'X' + 100, \
							'P' + 100, \
							'E' + 100, \
							'R' + 100, \
							'S' + 100, \
							'O' + 100, \
							'N' + 100, \
							'A' + 100, \
							'L' + 100, \
							'I' + 100, \
							'Z' + 100, \
							'E' + 100, \
							'\0'

struct BrandInfo
{
	char	iBrandSig[18];
	char	iSerialNo[20];
	char	iUserName[40];
	char	iOrganization[40];
};

extern BrandInfo brandInfo;

//------------------------------------oOo-----------------------------------//
// inheritable Item class for the Style/Configuration system

template<class T>
class Item
{
public:
	Item();
	~Item();

	void	SetItem(T t);
	void	SetPtr(T* t);
	T*		GetItem();
	T*		GetUnresolved();
	void 	AddBaseItem(Item<T>* t);

	int operator==(const Item<T>& t);
	int operator==(const T& t);
	int operator==(const Item<T>* t);
	int operator==(const T* t);

	T*					iItem;
	Set< Item<T> >	iBaseItems;
};

/**
 * 
 */
template<class T>
Item<T>::Item():
	iItem(0),
	iBaseItems(FALSE)
{
}

/**
 * 
 */
template<class T>
Item<T>::~Item()
{
	delete iItem;
}

/**
 * 
 */
template<class T>
int Item<T>::operator==(const Item<T>& t)
{
	if (!iItem || !t.iItem)
		return false;

	return *iItem == *t.iItem;
}

/**
 * 
 */
template<class T>
int Item<T>::operator==(const T& t)
{
	return *iItem == t;
}

/**
 * 
 */
template<class T>
int Item<T>::operator==(const Item<T>* t)
{
	if (!iItem || !t || !t->iItem)
		return false;

	return *iItem == *t->iItem;
}

/**
 * 
 */
template<class T>
int Item<T>::operator==(const T* t)
{
	if (!iItem || !t)
		return false;

	return *iItem == *t;
}

/**
 * 
 */
template<class T>
void Item<T>::AddBaseItem(Item<T>* t)
{
	if (iBaseItems.Size() > 0)
		iBaseItems.AddBefore((Item<T>*)(*iBaseItems.GetHead()), t);
	else
		iBaseItems.Add(t);
}

/**
 * 
 */
template<class T>
void Item<T>::SetItem(T t)
{
	if (!iItem)
		iItem = new T;

	*iItem = t;
}

/**
 * 
 */
template<class T>
void Item<T>::SetPtr(T* t)
{
	delete iItem;	// delete any previously allocated item

	iItem = t;
}

/**
 * 
 */
template<class T>
T* Item<T>::GetUnresolved()
{
	return iItem;
}

/**
 * 
 */
template<class T>
T* Item<T>::GetItem()
{
	if (iItem)
		{
		return iItem;
		}
	else
		{
		SetIterator< Item<T> >	itemsIterator(iBaseItems);
		Item<T>*					item;

		for (item = itemsIterator.GoHead();
			item;
			item = itemsIterator.GetNext())
			{
			if (item->iItem)
				{
				return item->iItem;
				}
			else
				{
				T* pItem = item->GetItem();

				if (pItem)
					return pItem;
				}
			}
		}

	return 0;
}

/**
 * 
 */
struct ReservedWord
{
	ReservedWord():
		iInherited(FALSE)
	{
	}

	ReservedWord(BOOL inherited, CString& keyword):
		iInherited(inherited),
		iKeyword(keyword)
	{
	}

	BOOL	iInherited;
	CString	iKeyword;

	int operator<(const ReservedWord& rw) {
		return iKeyword < rw.iKeyword;
	}
};

/**
 * 
 */
struct Style
{
	Style(const Style& c) { MFail("Unimplemented"); }
	Style(CString styleName);
	~Style();

	void	AddExtensions(CString& ext);
	void	AddReservedWord(CString& reservedWord, BOOL inherited = FALSE);
	void	AddOpenComment(CString& openComment);
	void	AddCloseComment(CString& closeComment);
	void	AddEolComment(CString& eolComment);
	void	AddNumericPrefix(CString& numericPrefix);

	// inheritance management
	void AddBaseStyle(Style* style);

	CString								iStyleName;
	Set<CString>						iBaseStyles;

	// general Style stuff
	Item<Set <CString> >				iExtensions;
	Item<BYTE>							iForeGndColor;
	Item<BYTE>							iBackGndColor;
	Item<BYTE>							iSelectedForeGndColor;
	Item<BYTE>							iSelectedBackGndColor;
	Item<BYTE>							iBoldColor;
	Item<BYTE>							iUnderlineColor;
	Item<BYTE>							iBoldUnderlineColor;
	Item<BOOL>							iExpandTabs;
	Item<BOOL>							iHighBitFilter;
	Item<BOOL>							iTextWithLayout;
	Item<int>							iTabWidth;
	Item<DisplayMode>			    	iDisplayMode;
	Item<CString>						iExternalFilterCmd;
	Item<BOOL>							iFilterEnabled;
	Item<CString>				    	iEditor;
	Item<int>			    	    	iTopLineFormat;
	Item<BOOL>							iWordBreak;

	// programming language syntax highlighting
	SortableArray<ReservedWord>			iReserved;
	Item<BOOL>			            	iSyntaxHighlightEnabled;
	Item<BYTE>							iSymbolsColor;
	Item<BYTE>							iCommentColor;
	Item<BYTE>							iStringColor;
	Item<BYTE>							iReservedColor;
	Item<BYTE>							iPreprocessorColor;
	Item<BYTE>							iNumberColor;
	Item<BYTE>							iIdentColor;
	Item<CString>						iSymbols;
	Item<Set <CString> >				iOpenComment;
	Item<Set <CString> >				iCloseComment;
	Item<Set <CString> >				iEolComment;
	Item<CString>			  			iString;
	Item<char>							iEscape;
	Item<Set <CString> >				iNumericPrefix;
	Item<BOOL>							iCaseSensitive;
	Item<BOOL>							iCaseConvert;
	Item<CString>	   					iOpenPreprocessor;
	Item<CString>	  					iClosePreprocessor;
	Item<int>					    	iCommentColumn;
	Item<char>							iLineContinuation;
};

#endif
