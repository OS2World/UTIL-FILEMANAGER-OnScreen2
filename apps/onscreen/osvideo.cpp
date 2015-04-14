/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: osvideo.cpp 2.50 1996/03/17 22:35:00 jallen Exp jallen $";
#pragma warn +use

/**
 * 
 */
void tprintf(const char* format, ...)
{
	va_list argptr;
	char buffer[256];

	va_start(argptr, format);
	int count = vsprintf(buffer, format, argptr);
	va_end(argptr);

	DisplayBufferInfo di;

	GetDisplayBufferInfo(&di);

	DisplayCell	tmp[256];
	DisplayCell*	pt = tmp;
	char*			bp = buffer;

	for (int i=0; i < count; i++, pt++)
		{
		pt->Char.AsciiChar = *bp++;
		pt->Attributes = WORD(di.attribute);
		}

	PutTextBuf(di.x, di.y, (di.x+count)-1, di.y, tmp);
	MoveToXY(di.x+count, di.y);
}

/**
 * 
 */
void MoveToXY(int x, int y)
{
#if defined(__WIN32__) && !defined(__DPMI32__)
	// under Win32 the console buffer size can be greater
	// than the visible area, so find out what the visible
	// area is and return that as the maximum displayable area.
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	COORD	cursorPos;
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);

	GetConsoleScreenBufferInfo(hConsole, &csbi);

	cursorPos.X = SHORT((x+csbi.srWindow.Left)-1);
	cursorPos.Y = SHORT((y+csbi.srWindow.Top)-1);

	SetConsoleCursorPosition(hConsole, cursorPos);
#else
	gotoxy(x, y);
#endif
}

/**
 * 
 */
void GetDisplayBufferInfo(DisplayBufferInfo* di)
{
	text_info	ti;
	gettextinfo(&ti);

#if defined(__WIN32__) && !defined(__DPMI32__)
	// under Win32 the console buffer size can be greater
	// than the visible area, so find out what the visible
	// area is and return that as the maximum displayable area.
	CONSOLE_SCREEN_BUFFER_INFO	csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);

	di->screenwidth 	= (csbi.srWindow.Right - csbi.srWindow.Left)+1;
	di->screenheight 	= (csbi.srWindow.Bottom - csbi.srWindow.Top)+1;
	di->x 				= (csbi.dwCursorPosition.X-csbi.srWindow.Left)+1;
	di->y 				= (csbi.dwCursorPosition.Y-csbi.srWindow.Top)+1;
	di->attribute 		= ti.attribute;
#else
	di->screenwidth 	= ti.screenwidth;
	di->screenheight 	= ti.screenheight;
	di->x 				= ti.curx;
	di->y 				= ti.cury;
	di->attribute 		= ti.attribute;
#endif
}

/**
 * 
 */
void PutTextBuf(int x, int y, int wx, int wy, DisplayCell* buf)
{
#if defined(__OS2__)
	puttext(x, y, wx, wy, (char*)buf);
#elif defined(__DPMI32__) || !defined(__WIN32__)
	puttext(x, y, wx, wy, (char*)buf);
#elif defined(__WIN32__)
// replacement for puttext under Win32 with Borland C++
// the Borland one with BC++ 4.0 does not work right

	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	COORD	size, source;
	SMALL_RECT	target;
	CONSOLE_SCREEN_BUFFER_INFO	csbi;

	GetConsoleScreenBufferInfo(hConsole, &csbi);

	target.Left		= SHORT((x-1) + csbi.srWindow.Left);
	target.Top 		= SHORT((y-1) + csbi.srWindow.Top);
	target.Right 	= SHORT((wx-1) + csbi.srWindow.Left);
	target.Bottom 	= SHORT((wy-1) + csbi.srWindow.Top);

	size.X = SHORT((wx-x)+1);
	size.Y = SHORT((wy-y)+1);

	source.X = SHORT(0);
	source.Y = SHORT(0);

	WriteConsoleOutput(hConsole, buf, size, source, &target);
#endif
}

/**
 * 
 */
void GetTextBuf(int x, int y, int wx, int wy, DisplayCell* buf)
{
#if defined(__OS2__)
	gettext(x, y, wx, wy, (char*)buf);
#elif defined(__DPMI32__) || !defined(__WIN32__)
	gettext(x, y, wx, wy, (char*)buf);
#elif defined(__WIN32__)
// replacement for gettext under Win32 with Borland C++
// the Borland one with BC++ 4.0 does not work correctly

	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	COORD	size, source;
	SMALL_RECT	target;
	CONSOLE_SCREEN_BUFFER_INFO	csbi;

	GetConsoleScreenBufferInfo(hConsole, &csbi);

	target.Left		= SHORT((x-1) + csbi.srWindow.Left);
	target.Top 		= SHORT((y-1) + csbi.srWindow.Top);
	target.Right 	= SHORT((wx-1) + csbi.srWindow.Left);
	target.Bottom 	= SHORT((wy-1) + csbi.srWindow.Top);

	size.X = SHORT((wx-x)+1);
	size.Y = SHORT((wy-y)+1);

	source.X = SHORT(0);
	source.Y = SHORT(0);

	ReadConsoleOutput(hConsole, buf, size, source, &target);
#endif
}

/**
 * 
 */
void MoveTextBuf(int left, int top, int right, int bottom, int destleft, int desttop)
{
#if defined(__OS2__)
	movetext(left, top, right, bottom, destleft, desttop);
#elif defined(__DPMI32__) || !defined(__WIN32__)
	movetext(left, top, right, bottom, destleft, desttop);
#elif defined(__WIN32__)
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	COORD	target;
	SMALL_RECT	source;
	DisplayCell	ci;
	CONSOLE_SCREEN_BUFFER_INFO	csbi;

	GetConsoleScreenBufferInfo(hConsole, &csbi);

	ci.Char.AsciiChar = ' ';
	ci.Attributes = 0;

	source.Left		= SHORT((left-1) + csbi.srWindow.Left);
	source.Top 		= SHORT((top-1) + csbi.srWindow.Top);
	source.Right 	= SHORT((right-1) + csbi.srWindow.Left);
	source.Bottom 	= SHORT((bottom-1) + csbi.srWindow.Top);

	target.X			= SHORT((destleft-1) + csbi.srWindow.Left);
	target.Y			= SHORT((desttop-1) + csbi.srWindow.Top);

	ScrollConsoleScreenBuffer(hConsole, &source, NULL, target, &ci);
#endif
}

/**
 * 
 */
void clrline()
{
	DisplayBufferInfo di;

	GetDisplayBufferInfo(&di);

	DisplayCell	tmp[256];
	DisplayCell*	pt = tmp;

	for (int i=di.x-1; i < di.screenwidth; i++, pt++)
		{
		pt->Char.AsciiChar = ' ';
		pt->Attributes = WORD(di.attribute);
		}

	PutTextBuf(di.x, di.y, di.screenwidth, di.y, tmp);
}

/**
 * 
 */
void setTextAttr(int foreground, int background)
{
	textattr((background*16) + foreground);
}
