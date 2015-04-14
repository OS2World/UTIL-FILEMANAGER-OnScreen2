/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop
#include <malloc.h>

#pragma warn -use
static char rcsID[]="$Id: osmisc.cpp 2.50 1996/03/17 22:34:53 jallen Exp jallen $";
#pragma warn +use

#if defined(__OS2__)
HMTX        hMtxListSync = 0;
#elif defined(__WIN32__) && !defined(__DPMI32__)
HANDLE        		hMtxListSync;
#endif

BOOL	bFirstTime = TRUE;

/**
 * 
 */
#if defined(__GO32__)
// replacement stricmp() as DJ's gpp version does not work correctly

extern "C"
int stricmp(const char* s1, const char* s2)
{
	while (*s1 && toupper(*s1) == toupper(*s2))
		{
		s1++;
		s2++;
		}

	return toupper(*s1) - toupper(*s2);
}

#endif

/**
 * 
 */
void WaitSync()
{
#if defined(__OS2__)
	if (hMtxListSync)
		DosRequestMutexSem(hMtxListSync, SEM_INDEFINITE_WAIT);
#elif defined(__WIN32__) && !defined(__DPMI32__)
	if (hMtxListSync)
		WaitForSingleObject(hMtxListSync, INFINITE);
#endif
}

/**
 * 
 */
void ClearSync()
{
#if defined(__OS2__)
	if (hMtxListSync)
		DosReleaseMutexSem(hMtxListSync);
#elif defined(__WIN32__) && !defined(__DPMI32__)
	if (hMtxListSync)
		ReleaseMutex(hMtxListSync);
#endif
}

/**
 * 
 */
int nextTabStop(int col, Style* style)
{
	int nextStop;

	if (style)
		{
		int tabWidth = *style->iTabWidth.GetItem();
		nextStop = col + tabWidth;

		nextStop = (nextStop / (tabWidth)) * (tabWidth);
		}

	return nextStop;
}

/**
 * 
 */
#if defined(__BORLANDC__) && (defined(__MSDOS__) || defined(__DPMI32__))
// ignore Ctrl-Break & Ctrl-C
int ctrlbrk_ignore(void)
{
  return 1;
}
#endif

/**
 * 
 */
int getKey()
{
   int key;

#if (defined(__OS2__) || defined(__WIN32__)) && !defined(__DPMI32__)
	key = getch();
#else
	#if defined(__BORLANDC__) && (defined(__MSDOS__) || defined(__DPMI32__))
		ctrlbrk(ctrlbrk_ignore);
	#endif

	while (!kbhit())
		{
		#if defined(__GO32__)
			// stop Ctrl-Break from quitting
			// under DJ's GO32 extender
   		_go32_was_ctrl_break_hit();
		#endif

		dispTime(0);
		}

	#if defined(__GO32__)
		// stop Ctrl-Break from quitting
		// under DJ's GO32 extender
  		_go32_was_ctrl_break_hit();
	#endif

	key = getch();
#endif

   if (key == 0)
      {
		#if defined(__GO32__)
			// stop Ctrl-Break from quitting
			// under DJ's GO32 extender
			_go32_was_ctrl_break_hit();
		#endif

		key = getch() + 0xFF00;
		}

   return key;
}

/**
 * 
 */
void displayMsg(CPCHAR msg)
{
   WaitSync();

   messageTime = gMessageClearTime;
	setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
	MoveToXY(1, di.screenheight);

	tprintf("%-70s", msg);

   ClearSync();
}

/**
 * 
 */
void displayPrompt(CPCHAR msg)
{
   WaitSync();
   int i=1;

   messageTime = gMessageClearTime;
	setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
	MoveToXY(1, di.screenheight);

	while (*msg)
		{
		if (*msg == '~')
			{
			msg++;
			setTextAttr(setupInfo.curDiskTextForeColor, setupInfo.curDiskTextBackColor);
			tprintf("%c", *msg++);
			setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
			}
		else
			{
			tprintf("%c", *msg++);
			}

		i++;
		}

	while (i < 70)
		{
		tprintf(" ");
		i++;
		}

	ClearSync();
}

/**
 * 
 */
void displayErrMsg(CPCHAR msg)
{
	WaitSync();

	messageTime = gMessageClearTime;

	setTextAttr(setupInfo.statusTextForeColor+BLINK, setupInfo.statusTextBackColor);

	MoveToXY(1, di.screenheight);
	tprintf("%70s", "");

	MoveToXY(1, di.screenheight);
	tprintf("** %s **", msg);

	if (setupInfo.soundOn)
		{
#if defined(__OS2__)
		DosBeep(500, 100);
		DosBeep(200, 100);
#elif defined(__WIN32__) && !defined(__DPMI32__)
		Beep(500, 100);
		Beep(200, 100);
#elif defined(__MSDOS__) || defined(__DPMI32__)
		sound(500);
		delay(100);
		sound(200);
		delay(100);
		nosound();
#endif
		}

	ClearSync();
}

/**
 * 
 */
void displayStdStatusLine()
{
#if defined(__OS2__)
	char* os = "OS/2";
#elif defined(__DPMI32__)
	char* os ="DOS";
#elif defined(__WIN32__) && !defined(__DPMI32__)
	char* os ="Win32";
#endif

	static int whichMsg = 0;
	setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
	MoveToXY(1, di.screenheight);

	if (bFirstTime)
		{
		clrline();
		bFirstTime = FALSE;
		}

	MoveToXY(1, di.screenheight);

	CString	msg;

	msg = sformat("OnScreen/2%s for %s, v%s, %s",
#if defined(BETA)
					"\xE1",
#else
					"",
#endif
					os,
					getRCSVersion(gOnScreenRevision),
					gOnScreenCopyright);

	tprintf("%-70s", (const char*)msg);
}

/**
 * 
 */
void clearMsg()
{
	WaitSync();

	displayStdStatusLine();

	ClearSync();
}

/**
 * 
 */
BOOL isDupe(SList<Viewer>& viewedFiles, const Dirent& d)
{
	for (int i=0; i < viewedFiles.Size(); i++)
		if (viewedFiles[i]->iFileInfo->iFileName == d.fullName())
			return TRUE;

	return FALSE;
}

/**
 * 
 */
int findDirent(SList<Viewer>& viewedFiles, const Dirent& d)
{
	for (int i=0; i < viewedFiles.Size(); i++)
		if (viewedFiles[i]->iFileInfo->iFileName == d.fullName())
			return i;

	return -1;
}

/**
 * 
 */
int findFile(SList<Viewer>& viewedFiles, const CString& s)
{
	for (int i=0; i < viewedFiles.Size(); i++)
		if (viewedFiles[i]->iFileInfo->iFileName == s)
			return i;

	return -1;
}

/**
 * 
 */
void cleanup(int key)
{
	displayMsg("Terminating...");

#if defined(__OS2__)
	if (gTimeThread)
		{
		TID tmpTID = gTimeThread;
		gTimeThreadEnd = TRUE;
		DosWaitThread(&tmpTID, DCWW_WAIT);
		}
#elif defined(__WIN32__) && !defined(__DPMI32__)
	if (gTimeThread)
		{
		gTimeThreadEnd = TRUE;
		WaitForSingleObject(gTimeThread, INFINITE);
		}
#endif

	switch (key)
		{
		case VKALT_Q:
		case 'q':
		case 'Q':
		case 'x':
		case 'X':
			{
			setTextAttr(WHITE, BLACK);
			clrscr();
			}
		break;

		case VKALT_X:
			{
			setTextAttr(WHITE, BLACK);
			PutTextBuf(1, 1, di.screenwidth, di.screenheight, screenBuf);
			MoveToXY(di.x, di.y);
			}
		break;

		case VKESCAPE:
		case VKCTRL_X:
		case VKALT_A:
			{
			setTextAttr(WHITE, BLACK);
			MoveToXY(1, di.screenheight);
			clrline();
			MoveToXY(1, di.screenheight-1);
			}
		break;
		}

#if defined(__OS2__)
	if (hMtxListSync)
		{
		ClearSync();
		DosCloseMutexSem(hMtxListSync);
		hMtxListSync = 0;
		}
#elif defined(__WIN32__) && !defined(__DPMI32__)
	if (hMtxListSync)
		{
		ClearSync();
		CloseHandle(hMtxListSync);
		hMtxListSync = 0;
		}
#endif

	_setcursortype(_NORMALCURSOR);
	changeDir(gStartupDir);
	setdisk(toupper(gStartupDir[1]) - 'A');
}

/**
 * 
 */
void AddFileList(CPCHAR fileSpec, SList<Viewer>& viewedFiles)
{
	CString dir, file;

	if (splitPath(fileSpec, dir, file))
      {
		Directory d(dir);

		d.fill(file);

		for (int j=0; j < d.size(); j++)
			{
			if (!d[j].isDirectory() &&
				!(d[j].mode() & FILE_HIDDEN) &&
				!(d[j].mode() & FILE_SYSTEM) &&
				!isDupe(viewedFiles, d[j]))
				viewedFiles.Add(new Viewer(new FileInfo(d[j].dirName(), d[j].name(), d[j].fullName(),
												d[j].size(), d[j].time(), d[j].date())));
			}
		}
}

/**
 * 
 */
BOOL Edit(CString fileName)
{
	Style*	fileStyle = findStyleForFile(fileName);

	if (!fileStyle)
		{
		displayErrMsg("Can't edit, no style associated with file type");
		return FALSE;
		}

	if (*fileStyle->iEditor.GetItem() == "")
		{
		displayErrMsg("Can't edit, no editor assigned to file type");
		return FALSE;
		}

#if defined(__OS2__)
	MEnsure(DosSuspendThread(gTimeThread) == NO_ERROR);
#elif defined(__WIN32__) && !defined(__DPMI32__)
	MEnsure(SuspendThread(gTimeThread) != 0xFFFFFFFF);
#endif

	_setcursortype(_NORMALCURSOR);

#if defined(__OS2__)
	fileName = CString("\"") + fileName + CString("\"");
#elif defined(__WIN32__) && !defined(__DPMI32__)
	fileName = CString("\"") + fileName + CString("\"");
#elif defined(__GNUG__)
#elif defined(__MSDOS__) || defined(__DPMI32__)
#endif

	CString	cmdLine = *fileStyle->iEditor.GetItem() + " " + fileName;

	textcolor(WHITE);
	textbackground(BLACK);
	clrscr();
	system(cmdLine);

	_setcursortype(_NOCURSOR);

	// make the status line display function
	// clear the status line
	extern BOOL bFirstTime;
	bFirstTime = TRUE;

#if defined(__OS2__)
	MEnsure(DosResumeThread(gTimeThread) == NO_ERROR);
#elif defined(__WIN32__) && !defined(__DPMI32__)
	MEnsure(ResumeThread(gTimeThread) != 0xFFFFFFFF);
#endif

	return TRUE;
}
