/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: ostime.cpp 2.50 1996/03/17 22:34:56 jallen Exp jallen $";
#pragma warn +use

/**
 * 
 */
void displayTime()
{
	static 	BOOL 	showSeparators;
	DisplayBufferInfo	di;

	CString timeStr = systemTime().longCString();

	WaitSync();
	GetDisplayBufferInfo(&di);
	MoveToXY(di.screenwidth - 7, di.screenheight);
	setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);

	showSeparators = !showSeparators;
	PCHAR ts = (PCHAR)(CPCHAR)timeStr;

	ts[5] = '\0';

	if (!showSeparators)
		{
		ts[2] = ' ';
		}

	if (*ts == '0')
		*ts = ' ';

	tprintf("%s%s", ts, &ts[8]);

	if (messageTime > 0 && --messageTime == 0)
		{
		messageTime = gMessageClearTime;
		displayStdStatusLine();
		}

	MoveToXY(di.x, di.y);
	ClearSync();

	// check if the current file has changed
	if (gCurrFile != -1 && App::GetViewedFiles()[gCurrFile]->fileHasChanged())
		{
		WaitSync();
		GetDisplayBufferInfo(&di);
		App::GetViewedFiles()[gCurrFile]->displayFileInfo();
		MoveToXY(di.x, di.y);
		ClearSync();
		}
}

/**
 * 
 */
void dispTime(void* /* vp */)
{
#if defined(__OS2__)
	int	times = 0;

	while (!gTimeThreadEnd)
		{
		if (++times == 4)
			{
			times = 0;
			displayTime();
			}

		DosSleep(250);
		}
#elif defined(__WIN32__) && !defined(__DPMI32__)
	int	times = 0;

	while (!gTimeThreadEnd)
		{
		if (++times == 4)
			{
			times = 0;
			displayTime();
			}

		Sleep(250);
		}
#elif defined(__MSDOS__) || defined(__DPMI32__)
	Time t = systemTime();
	static long lasttime;

	if ((long(t) - lasttime) < 1)
		return;

	lasttime = t;
	displayTime();
#endif
}
