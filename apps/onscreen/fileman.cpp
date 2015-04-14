/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop
#include <sys/stat.h>
#include <errno.h>
#include <malloc.h>

#pragma warn -use
static char rcsID[]="$Id: fileman.cpp 2.50 1996/03/17 22:34:12 jallen Exp jallen $";
#pragma warn +use

extern BOOL viewHelp(CString fileName);

int sortByName(const Dirent* d1, const Dirent* d2);
int sortByExt(const Dirent* d1, const Dirent* d2);
int sortByDate(const Dirent* d1, const Dirent* d2);
int sortBySize(const Dirent* d1, const Dirent* d2);

static direntCompareFunc* sortBy = sortByName;
static BOOL	sortAscending = TRUE;
static SList<CString>	directoryHistory;
static SList<CString>	mkdirHistory;
static SList<CString>	fileSpecHistory;

/**
 * 
 */
int sortByExt(const Dirent* d1, const Dirent* d2)
{
	static char* null = "";

	if (d1->isDirectory() && !d2->isDirectory())
		return -1;

	CString	s1 = d1->name();
	CString	s2 = d2->name();

	if (d1->isDirectory() && d2->isDirectory())
		return stricmp(s1, s2);

	char* e1;
	char* e2;

	if (s1 == "..")
		e1 = 0;
	else
		e1 = (char*)strrchr(s1, '.');

	if (s2 == "..")
		e2 = 0;
	else
		e2 = (char*)strrchr(s2, '.');

	if (e1 != 0)
		*e1++ = '\0';
	else
		e1 = null;

	if (e2 != 0)
		*e2++ = '\0';
	else
		e2 = null;

	int retval;

	if (sortAscending)
		{
		retval = stricmp(e1, e2);

		if (retval == 0)
			retval = stricmp(s1, s2);
		}
	else
		{
		retval = stricmp(e2, e1);

		if (retval == 0)
			retval = stricmp(s2, s1);
		}

	return retval;
}

/**
 * 
 */
int sortByName(const Dirent* d1, const Dirent* d2)
{
	if (d1->isDirectory() && !d2->isDirectory())
		return -1;

	CString	s1 = d1->name();
	CString	s2 = d2->name();

	if (sortAscending || (d1->isDirectory() && d2->isDirectory()))
		return stricmp(s1, s2);
	else
		return stricmp(s2, s1);
}

/**
 * 
 */
int sortBySize(const Dirent* d1, const Dirent* d2)
{
	int retval = 0;

	if (d1->isDirectory() && !d2->isDirectory())
		return -1;
	else
	if (d1->isDirectory() && d2->isDirectory())
		return stricmp(d1->name(), d2->name());

	if (sortAscending)
		{
		retval = d1->size() == d2->size() ? 0: d1->size() < d2->size() ? -1:1;

		if (retval == 0)
			{
			CString	s1 = d1->name();
			CString	s2 = d2->name();

			retval = stricmp(s1, s2);
			}
		}
	else
		{
		retval = d2->size() == d1->size() ? 0: d2->size() < d1->size() ? -1:1;

		if (retval == 0)
			{
			CString	s1 = d1->name();
			CString	s2 = d2->name();

			retval = stricmp(s2, s1);
			}
		}

	return retval;
}

/**
 * 
 */
int sortByDate(const Dirent* d1, const Dirent* d2)
{
	int retval = 0;

	if (d1->isDirectory() && !d2->isDirectory())
		return -1;
	else
	if (d1->isDirectory() && d2->isDirectory())
		return stricmp(d1->name(), d2->name());

	if (sortAscending)
		{
		if (d1->date() == d2->date())
			retval = d1->time() == d2->time() ? 0: d1->time() < d2->time() ? -1:1;
		else
			retval = d1->date() < d2->date() ? -1:1;

		if (retval == 0)
			{
			CString	s1 = d1->name();
			CString	s2 = d2->name();

			retval = stricmp(s1, s2);
			}
		}
	else
		{
		if (d1->date() == d2->date())
			retval = d2->time() == d1->time() ? 0: d2->time() < d1->time() ? -1:1;
		else
			retval = d2->date() < d1->date() ? -1:1;

		if (retval == 0)
			{
			CString	s1 = d1->name();
			CString	s2 = d2->name();

			retval = stricmp(s2, s1);
			}
		}

	return retval;
}

/**
 * 
 */
BOOL CtrlIsPressed()
{
#if defined(__OS2__)
	KBDINFO kbdinfo;

	kbdinfo.cb = sizeof(kbdinfo);

	KbdGetStatus(&kbdinfo, 0);

	if (kbdinfo.fsState & KBDSTF_CONTROL)
		return TRUE;
#elif defined(__DPMI32__)
	WORD kbdstate;

	union REGS	r;
	r.h.ah = 0x12;
	int386(0x16, &r, &r);

	kbdstate = r.h.ah;
	kbdstate = (kbdstate << 8) | r.h.al;

	if (kbdstate & 0x0004)
		return TRUE;
#elif defined(__WIN32__)
	extern DWORD	dwControlKeyState;

	if (dwControlKeyState & LEFT_CTRL_PRESSED ||
		dwControlKeyState & RIGHT_CTRL_PRESSED)
		return TRUE;
#elif defined(__GNUG__) && defined(__GO32__)
	WORD	kbdstate;

	dosmemget(1047, sizeof(kbdstate), &kbdstate);

	if (kbdstate & 0x0004)
		return TRUE;
#elif defined(__MSDOS__)
	// TODO: add Keyboard status code for plain DOS
#endif

	return FALSE;
}

/**
 * 
 */
BOOL ShiftIsPressed()
{
#if defined(__OS2__)
	KBDINFO kbdinfo;

	kbdinfo.cb = sizeof(kbdinfo);

	KbdGetStatus(&kbdinfo, 0);

	if (kbdinfo.fsState & KBDSTF_LEFTSHIFT ||
		kbdinfo.fsState & KBDSTF_RIGHTSHIFT)
		return TRUE;
#elif defined(__DPMI32__)
	WORD kbdstate;

	union REGS	r;
	r.h.ah = 0x12;
	int386(0x16, &r, &r);

	kbdstate = r.h.ah;
	kbdstate = (kbdstate << 8) | r.h.al;

	if (kbdstate & 0x0002 || kbdstate & 0x0001)
		return TRUE;
#elif defined(__WIN32__)
	extern DWORD	dwControlKeyState;

	if (dwControlKeyState & SHIFT_PRESSED)
		return TRUE;
#elif defined(__GNUG__) && defined(__GO32__)
	WORD	kbdstate;

	dosmemget(1047, sizeof(kbdstate), &kbdstate);

	if (kbdstate & 0x0002 || kbdstate & 0x0001)
		return TRUE;
#elif defined(__MSDOS__)
	// TODO: add Keyboard status code for plain DOS
#endif

	return FALSE;
}

/**
 * 
 */
BOOL CopyFile(const char* src, const char* dst)
{
#if defined(__OS2__)
	APIRET rcDosCopy = DosCopy(src, dst, DCPY_EXISTING);

	if (rcDosCopy == NO_ERROR)
		return TRUE;

	CHAR	msg[128];
	ULONG	msgLen;

	APIRET rcDosGetMessage = DosGetMessage(0, 0, msg, sizeof(msg), rcDosCopy,
								"OSO001.MSG", &msgLen);

	if (rcDosGetMessage == NO_ERROR)
		{
		msg[msgLen-2] = '\0';	// kill the CRLF
		displayErrMsg(msg);
		}
	else
		{
		displayErrMsg("Could not retrieve system error message");
		}
#elif defined(__WIN32__)
	if (CopyFile(src, dst, FALSE) == TRUE)
		{
		return TRUE;
		}

	displayErrMsg("File copy failed");
#elif defined(__MSDOS__)
	int	hSource, hDest;

	hSource = open(src, O_RDONLY | O_BINARY);

	if (hSource != -1)
		{
		int	cb;
		hDest = open(dst, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE);

		if (hDest != -1)
			{
			char	buffer[10240];

			while ((cb = read(hSource, buffer, sizeof(buffer))) > 0)
				{
				if (write(hDest, buffer, cb) < cb)
					{
					// write error probably disk full!!
					close(hSource);
					close(hDest);
					displayErrMsg(sys_errlist[errno]);
					unlink(dst);

					return FALSE;
					}
				}

			struct ftime	ft;

			if (getftime(hSource, &ft) == -1)
				{
				displayErrMsg(sys_errlist[errno]);
				}
			else
				{
				if (setftime(hDest, &ft) == -1)
					displayErrMsg(sys_errlist[errno]);
				}

			if (close(hDest) == -1)
				{
				displayErrMsg(sys_errlist[errno]);
				}

			if (close(hSource) == -1)
				{
				displayErrMsg(sys_errlist[errno]);
				}

			struct stat st;

			if (stat(src, &st) == -1)
				{
				displayErrMsg(sys_errlist[errno]);
				}
			else
				{
				if (chmod(dst, st.st_mode) == -1)
					displayErrMsg(sys_errlist[errno]);
				}

			return TRUE;
			}
		else
			{
			close(hSource);
			displayErrMsg(sys_errlist[errno]);
			}
		}
	else
		{
		displayErrMsg(sys_errlist[errno]);
		}
#endif

	return FALSE;
}

/**
 * 
 */
BOOL OS_MoveFile(const char* src, const char* dst)
{
	if (rename(src, dst) == 0)
		return TRUE;

	if (errno == ENOTSAM)
		{
		if (CopyFile(src, dst))
			{
			if (unlink(src) == 0)
				return TRUE;
			else
				return FALSE;
			}
		}

	return FALSE;
}

/**
 * 
 */
FileManDir::FileManDir(CPCHAR dirName):
	Directory(dirName)
{
}

/**
 * 
 */
FileManDir::~FileManDir()
{
}

/**
 * 
 */
int FileManDir::fill(CPCHAR fileSpec)
{
	CString		fileName;

	displayMsg("Reading directory");
	messageTime = 0; // prevent this message from being cleared by timer display thread

	entry.SetDynamic(TRUE);

	{
        Directory d(dirName);
        d.fill("*");

        for (int i=0; i < d.size(); i++)
            {
            if (d[i].isDirectory())
                {
                if (d[i].name() != ".")
                    {
                    if (d[i].name() == "..")
                        {
                        fileName = "" + d[i].name();
                        }
                    else
                        {
                        fileName = "" + d[i].name();
                        }
                    }

                if (d[i].name() != ".")
                    {
                    Dirent aDirent(this,
                            fileName,
                            d[i].fDate,
                            d[i].fTime,
                            d[i].lSize,
                            d[i].mtMode);

                    int size = entry.Size();

                    entry[size] = aDirent;
                    }
                }
            }
        }

    	{
        Directory d(dirName);
        d.fill(fileSpec);

        for (int i=0; i < d.size(); i++)
            {
            if (!d[i].isDirectory())
                {
                fileName = d[i].name();

                Dirent aDirent(this,
                        fileName,
                        d[i].fDate,
                        d[i].fTime,
                        d[i].lSize,
                        d[i].mtMode);

                int size = entry.Size();

                entry[size] = aDirent;
                }
            }
        }

	entry.SetDynamic(FALSE);

	return entry.Size();
}

/**
 * 
 */
FileManager::FileManager():
	iCurDir(queryCurrentDir(0)),
	iFileSpec("*"),
	iCurFile(0),
	iCurColumn(0),
	iDirectory(new FileManDir()),
	iLinesPerColumn(di.screenheight-3),
	iColumns(3)
{
	do
		{
		iColumnWidth = di.screenwidth / --iColumns;
		}
	while (iColumnWidth < 40);
}

/**
 * 
 */
FileManager::FileManager(CString& dir):
	iCurDir(dir),
	iFileSpec("*"),
	iCurFile(0),
	iCurColumn(0),
	iDirectory(new FileManDir(dir)),
	iLinesPerColumn(di.screenheight-3),
	iColumns(3)
{
	do
		{
		iColumnWidth = di.screenwidth / --iColumns;
		}
	while (iColumnWidth < 40);
}

/**
 * 
 */
FileManager::~FileManager()
{
}

/**
 * 
 */
void FileManager::DisplayDisks()
{
	int 	disk;
	ULONG	diskBit;
	int	pos=2;

	WaitSync();

	MoveToXY(1, 2);
	setTextAttr(setupInfo.diskTextForeColor, setupInfo.diskTextBackColor);
	tprintf(" ");

	for (disk = 0, diskBit = 0x00000001;
			disk < 32;
			disk++, diskBit *= 2)
		{
		if ((iDriveMap & diskBit) == diskBit)
			{
			MoveToXY(pos, 2);

			if (disk == getdisk())
				{
				setTextAttr(setupInfo.curDiskTextForeColor, setupInfo.curDiskTextBackColor);
				MoveToXY(pos-1, 2);
				tprintf("(");
				}
			else
				{
				setTextAttr(setupInfo.diskTextForeColor, setupInfo.diskTextBackColor);
				}

			tprintf("%c", disk+'A');
			tprintf(":");
			setTextAttr(setupInfo.diskTextForeColor, setupInfo.diskTextBackColor);

			if (disk == getdisk())
				{
				setTextAttr(setupInfo.curDiskTextForeColor, setupInfo.curDiskTextBackColor);
				tprintf(")");
				}
			else
				tprintf(" ");

			pos+=3;
			}
		}

	setTextAttr(setupInfo.diskTextForeColor, setupInfo.diskTextBackColor);
	clrline();

	ClearSync();
}

/**
 * 
 */
void FileManager::DisplayDirent(const Dirent& d, int x, int y)
{
	CString	fname = d.name();
	CString	s;

	CString	dateStr;
	CString	timeStr;

	timeStr = sformat("%02i:%02i", d.time().hours(), d.time().minutes());
	dateStr = CString(d.date());

	if (fname.length() > (iColumnWidth-27))
		{
		fname = SubCString(fname, 1, (iColumnWidth-28));
		fname = fname + CString("");
		}
	else
		{
		fname = fname + CString(' ', (iColumnWidth-27)-fname.length());
		}

	if (d.isDirectory())
		s = dateStr + " " + timeStr + " " + sformat("%8s ", "<DIR>") + fname + "�";
	else
		s = dateStr + " " + timeStr + " " + sformat("%8li ", d.size()) + fname + "�";

	WaitSync();

	MoveToXY(x, y);
	setTextAttr(setupInfo.fileTextForeColor, setupInfo.fileTextBackColor);
	tprintf("%s", (const char*)s);

	ClearSync();
}

/**
 * 
 */
void FileManager::Hilight(int whichFile, BOOL bHighlight)
{
	int			whichColumn = whichFile / iLinesPerColumn;
	int			x, y;
	DisplayCell	buf[270];

	WaitSync();

	if (bHighlight)
		{
		setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
		MoveToXY(1, 1);
		tprintf("%5i �", iCurFile+1);
		setTextAttr(setupInfo.curFileTextForeColor, setupInfo.curFileTextBackColor);
		}
	else
		{
		setTextAttr(setupInfo.fileTextForeColor, setupInfo.fileTextBackColor);
		}

	y = (whichFile - (whichColumn*iLinesPerColumn))+3;

	if (whichColumn < iCurColumn)
		{
		iCurColumn = whichColumn;
		DisplayDirectory();
		}
	else
		{
		if (whichColumn >= iCurColumn+iColumns)
			{
			iCurColumn = whichColumn - (iColumns-1);

			if (iCurColumn < 0)
				iCurColumn = 0;

			DisplayDirectory();
			}
		}

   int i;

	for (i=0; i < iColumns; i++)
		{
		if (whichColumn == iCurColumn+i)
			{
			x = (iColumnWidth*i)+1;
			}
		}

	GetTextBuf(x, y, x+(iColumnWidth-1), y, buf);

	DisplayBufferInfo di;
	GetDisplayBufferInfo(&di);
	DisplayCell*	pt = buf;

	for (i=x; i < x+iColumnWidth; i++, pt++)
		{
		pt->Attributes = di.attribute;
		}

	PutTextBuf(x, y, x+(iColumnWidth-1), y, buf);

	ClearSync();
}

/**
 * 
 */
void FileManager::DisplayDirectory()
{
	int	start = iCurColumn * iLinesPerColumn;
	int	y, x;

	for (int col=0; col < iColumns; col++)
		{
		x = (col*iColumnWidth)+1;
		y = 0;

		while (y < iLinesPerColumn)
			{
			if (y+start < iDirectory->size())
				{
				DisplayDirent((*iDirectory)[y+start], x, y+3);
				}
			else
				{
				// clear
				WaitSync();
				setTextAttr(setupInfo.fileTextForeColor, setupInfo.fileTextBackColor);
				MoveToXY(x, y+3);
				tprintf("%*s", iColumnWidth, "");
				ClearSync();
				}

			y++;
			}

		start += iLinesPerColumn;
		}

	Hilight(iCurFile);
}

/**
 * 
 */
void FileManager::DisplayDirectoryInfo()
{
	WaitSync();

	setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
	MoveToXY(1, 1);
	clrline();
	tprintf("%5i � %5i%c� %-20s � %s",
		iCurFile,
		iDirectory->size(),
		iFileSpec != "*" ? '*':' ',
		(const char*)iCurText,
		(const char*)mergePath(iCurDir, iFileSpec));

	ClearSync();
}

/**
 * 
 */
BOOL FileManager::IsValidDisk(int disk)
{
	ULONG	bitMask = 0x00000001;

	bitMask <<= (disk);

	return (iDriveMap & bitMask) != 0;
}

/**
 * 
 */
void FileManager::Refresh()
{
	setdisk((toupper(iCurDir[1])-'A'));
	chdir(iCurDir);

	delete iDirectory;

	iDirectory = new FileManDir(iCurDir);

	iDirectory->fill(iFileSpec);
	iDirectory->sort(sortBy);

	if (iCurFile >= iDirectory->size())
		{
		iCurFile = iDirectory->size()-1;
		}

	DisplayDisks();
	DisplayDirectoryInfo();
	DisplayDirectory();
	clearMsg();
}

/**
 * 
 */
void FileManager::ChooseSortBy()
{
	BOOL	reverseSort = FALSE;
	int	key;

getAnotherKey:
	displayPrompt("Sort by: [~-] ~Name ~Ext ~Date ~Size");
	messageTime = 0; // prevent this message from being cleared by timer display thread
	key = getKey();
	clearMsg();

	switch (key)
		{
		case 0x001B:
			{
			messageTime = gMessageClearTime; // allow message to be cleared by timer display thread
			}
		break;

		case '-':
			{
			reverseSort = TRUE;
			goto getAnotherKey;
			}

		case 'n':
		case 'N':
			{
			sortBy = sortByName;
			}
		break;

		case 'e':
		case 'E':
			{
			sortBy = sortByExt;
			}
		break;

		case 'd':
		case 'D':
			{
			sortBy = sortByDate;
			}
		break;

		case 's':
		case 'S':
			{
			sortBy = sortBySize;
			}
		break;

		default:
			goto getAnotherKey;
		}

	if (reverseSort)
		sortAscending = FALSE;
	else
		sortAscending = TRUE;

	iDirectory->sort(sortBy);
	DisplayDisks();
	DisplayDirectoryInfo();
	DisplayDirectory();

	messageTime = gMessageClearTime; // allow message to be cleared by timer display thread
}

/**
 * 
 */
BOOL FileManager::EditFile(SList<Viewer>& viewedFiles, int& currFile)
{
	const Dirent& d = (*iDirectory)[iCurFile];

	// don't edit directories
	if (d.name()[1] != '' && d.name()[1] != '')
		{
		if (Edit(d.fullName()))
			{
			int tmpCurrFile = findDirent(viewedFiles, d);

			if (tmpCurrFile != -1)
				{
				// if we just edited the currently loaded
				// file we need to reload it.
				if (tmpCurrFile == currFile)
					viewedFiles[tmpCurrFile]->loadFile();
				}

			return TRUE;
			}
		else
			{
			return FALSE;
			}
		}
	else
		{
		displayErrMsg("Can't edit directories");
		}

	return FALSE;
}

/**
 * 
 */
BOOL FileManager::ViewFile(SList<Viewer>& viewedFiles, int& currFile)
{
	// view this file
	const Dirent& d = (*iDirectory)[iCurFile];

	if (d.name()[1] == '' || d.name()[1] == '')
		return FALSE;

	if (!isDupe(viewedFiles, d))
		{
		viewedFiles.Add(new Viewer(new FileInfo(
											d.dirName(),
											d.name(),
											d.fullName(),
											d.size(),
											d.time(),
											d.date())));

		if (currFile != -1)
			{
			viewedFiles[currFile]->dumpFile();
			}

		currFile = viewedFiles.Size()-1;
		viewedFiles[currFile]->loadFile();
		viewedFiles[currFile]->displayFileInfo();
		viewedFiles[currFile]->displayData();
		}
	else
		{
		int newFile = findDirent(viewedFiles, d);

		if (newFile != -1)
			{
			if (currFile == -1)
				{
				currFile = newFile;
				viewedFiles[currFile]->loadFile();
				}
			else
				{
				viewedFiles[currFile]->dumpFile();
				currFile = newFile;
				viewedFiles[currFile]->loadFile();
				}

			viewedFiles[currFile]->displayFileInfo();
			viewedFiles[currFile]->displayData();
			}

		}

	return TRUE;
}

/**
 * 
 */
BOOL FileManager::CopyFile(SList<Viewer>& viewedFiles, int& currFile)
{
	const Dirent& d = (*iDirectory)[iCurFile];
	char s[58];
	CString tmp = d.name();
	const char* name = tmp;

	if (*name == '' || *name == '')
		{
		displayErrMsg("Directory copy not implemented in this release");
		return FALSE;
		}

	strcpy(s, "");

	LineEdit	l(sizeof(s)-1,
					"Copy To",
					s, 0
					);

	if (l.Go() != 0x001B)
		{
		CString sourceName = mergePath(iCurDir, name);
		CString targetName, fullDir;

		if (isDir(s, fullDir))
			{
			targetName = mergePath(fullDir, name);
			}
		else
			{
			CString	dir, file;

			if (splitPath(s, dir, file))
				{
				targetName = mergePath(dir, file);
				}
			else
				{
				targetName = mergePath(iCurDir, s);
				}
			}

 		::CopyFile(sourceName, targetName);

		int foundFile = findFile(viewedFiles, targetName);

		if (foundFile != -1)
			{
			// we've copied to a file that we are viewing

			viewedFiles[foundFile]->dumpFile();

			if (foundFile == currFile)
				viewedFiles[foundFile]->loadFile();
			}
		}

	return FALSE;
}

/**
 * 
 */
BOOL FileManager::DeleteFile(SList<Viewer>& viewedFiles, int& currFile)
{
	const Dirent& d = (*iDirectory)[iCurFile];

	if (d.name() == "..")
		{
		displayErrMsg("Cannot delete parent directory");
		return FALSE;
		}

	BOOL 	bFinished = FALSE;
	BOOL	bDeleted = FALSE;

	do
		{
		if (d.name()[1] == '' || d.name()[1] == '')
			{
			displayPrompt(sformat("Delete directory \"%s\" ~Yes/~No",
										((const char*)d.name())+1));
			}
		else
			{
			displayPrompt(sformat("Delete file \"%s\" ~Yes/~No",
										(const char*)d.name()));
			}

		messageTime = 0; // prevent this message from being cleared by timer display thread

		int	key = getKey();

		clearMsg();

		bFinished = TRUE;

		switch (key)
			{
			case 'Y':
			case 'y':
				{
				if (d.name()[1] == '' || d.name()[1] == '')
					{
					CString	dirName = d.name();

					dirName = mergePath(iCurDir, SubCString(dirName, 2, dirName.length()));

					Directory d(dirName);

					d.fill();

					for (int i=0; i < d.size(); i++)
						{
						unlink(d[i].fullName());
						}

					if (rmdir(dirName) != -1)
						bDeleted = TRUE;
					else
						displayErrMsg(sys_errlist[errno]);
					}
				else
					{
					if (unlink(d.fullName()) != -1)
						{
						int tmpCurrFile = findDirent(viewedFiles, d);

						if (tmpCurrFile != -1)
							{
							// we have deleted a file that is in the
							// buffer list, so dump it

							viewedFiles[tmpCurrFile]->dumpFile();

							if (viewedFiles.Remove(viewedFiles[tmpCurrFile]))
								{
								if (viewedFiles.Size() > 0)
									{
									if (tmpCurrFile >= currFile)
										currFile--;

									if (currFile >= viewedFiles.Size())
										{
										currFile--;
										}
									}
								else
									{
									currFile = -1;
									}
								}
							}

							bDeleted = TRUE;
						}
					else
						displayErrMsg(sys_errlist[errno]);
					}
				}
			break;

			case 'N':
			case 'n':
				{
				}
			break;

			case 0x001B:
				{
				clearMsg();
				}
			break;

			default:
				bFinished = FALSE;
			}
		}
	while (!bFinished);

	messageTime = gMessageClearTime; // allow message to be cleared by timer display thread

	return bDeleted;
}

/**
 * 
 */
BOOL FileManager::RenameFile(SList<Viewer>& viewedFiles, int& currFile)
{
	const Dirent& d = (*iDirectory)[iCurFile];
	char s[58];
	CString tmp = d.name();
	const char* name = tmp;

	if (d.name() == "..")
		{
		displayErrMsg("Cannot rename parent directory");
		return FALSE;
		}

	if (*name == '' || *name == '')
		{
		name++;
		}

	strcpy(s, name);

	LineEdit	l(sizeof(s)-1,
					"Rename",
					s, 0
					);

	if (l.Go() != 0x001B)
		{
		if (rename(name, s) == 0)
			{
			int tmpCurrFile = findDirent(viewedFiles, d);

			if (tmpCurrFile != -1)
				{
				// we have renamed a file that is in the
				// buffer list, so dump it

				viewedFiles[tmpCurrFile]->dumpFile();

				if (viewedFiles.Remove(viewedFiles[tmpCurrFile]))
					{
					if (viewedFiles.Size() > 0)
						{
						if (tmpCurrFile >= currFile)
							currFile--;

						if (currFile >= viewedFiles.Size())
							{
							currFile--;
							}
						}
					else
						{
						currFile = -1;
						}
					}
				}

			return TRUE;
			}
		else
			displayErrMsg(sys_errlist[errno]);
		}

	return FALSE;
}

/**
 * 
 */
BOOL FileManager::MoveFile(SList<Viewer>& viewedFiles, int& currFile)
{
	const Dirent& d = (*iDirectory)[iCurFile];
	char s[58];
	CString tmp = d.name();
	const char* name = tmp;

	if (d.name() == "..")
		{
		displayErrMsg("Cannot move parent directory");
		return FALSE;
		}

	if (*name == '' || *name == '')
		{
		name++;
		}

	strcpy(s, "");

	LineEdit	l(sizeof(s)-1,
					"Move To",
					s, 0
					);

	if (l.Go() != 0x001B)
		{
		CString 	oldName = mergePath(iCurDir, name),
					newName = mergePath(s, name);

		if (OS_MoveFile(oldName, newName))
			{
			int tmpCurrFile = findDirent(viewedFiles, d);

			if (tmpCurrFile != -1)
				{
				// we have moved a file that is in the
				// buffer list, so dump it

				viewedFiles[tmpCurrFile]->dumpFile();

				if (viewedFiles.Remove(viewedFiles[tmpCurrFile]))
					{
					if (viewedFiles.Size() > 0)
						{
						if (tmpCurrFile >= currFile)
							currFile--;

						if (currFile >= viewedFiles.Size())
							{
							currFile--;
							}
						}
					else
						{
						currFile = -1;
						}
					}
				}

			return TRUE;
			}
		else
			{
			displayErrMsg(sys_errlist[errno]);
			}
		}

	return FALSE;
}

/**
 * 
 */
void FileManager::ChangeDirectory()
{
	char s[58];

	strcpy(s, "");

	LineEdit	l(sizeof(s)-1,
					"Directory",
					s, &directoryHistory, FALSE
					);

	if (l.Go() != 0x001B)
		{
		int slen = strlen(s);

		if (slen > 2 && s[slen-1] == '\\' && s[slen-2] != ':')
			{
			s[slen-1] = '\0';
			}
		else
		if (slen == 2 && s[slen-1] == ':')
			{
			s[slen] = '.';
			s[slen+1] = '\0';
			}

      int oldDisk = getdisk();
		char*	dirp = s;
		int disk = oldDisk;

		if (s[1] == ':')
			{
			disk = (toupper(s[0])-'A');

			if (IsValidDisk(disk))
				{
				setdisk(disk);
				CString	tmpDir = queryCurrentDir(0);

				if (tmpDir == "" || toupper(tmpDir[1]) != 'A'+disk)
					{
					setdisk(oldDisk);
					displayErrMsg(sformat("%c: drive not ready", disk+'A'));

					return;
					}
				}
			else
				{
				displayErrMsg(sformat("%c: invalid disk", disk+'A'));

				return;
				}
			}

		if (chdir(dirp) != -1)
			{
			CString	tmpDir = queryCurrentDir(0);

			if (tmpDir != "" && toupper(tmpDir[1]) == 'A'+disk)
				{
				directoryHistory.Add(new CString(iCurDir));

				iCurDir = queryCurrentDir(0);
				iCurFile = 0;
				iCurTextFile = "";
				iCurTextDir = "";
				iCurText = "";
				Refresh();
				}
			else
				{
				setdisk(oldDisk);
				displayErrMsg(sformat("%c: drive not ready", disk+'A'));
				}
			}
		else
			{
			displayErrMsg(sys_errlist[errno]);
			setdisk(oldDisk);
			}
		}
}

/**
 * 
 */
void FileManager::MakeDirectory()
{
	char s[58];

	strcpy(s, "");

	LineEdit	l(sizeof(s)-1,
					"MakeDir",
					s, &mkdirHistory
					);

	if (l.Go() != 0x001B)
		{
#if defined(__GNUG__)
		if (mkdir(s, 0) != -1)
#else
		if (mkdir(s) != -1)
#endif
			{
			/*
			iCurFile = 0;
			iCurTextFile = "";
			iCurTextDir = "";
			iCurText = "";
			*/
			Refresh();
			}
		else
			displayErrMsg(sys_errlist[errno]);
		}
}

/**
 * 
 */
void FileManager::ChangeFileSpec()
{
	char s[58];

	strcpy(s, "");

	LineEdit	l(sizeof(s)-1,
					"FileSpec",
					s, &fileSpecHistory
					);

	if (l.Go() != 0x001B)
		{
		iFileSpec = s;
		iCurFile = 0;
		iCurTextFile = "";
		iCurTextDir = "";
		iCurText = "";
		Refresh();
		}
}

/**
 * 
 */
BOOL FileManager::MatchSelect(BOOL forward, BOOL directories)
{
	const Dirent& d = (*iDirectory)[iCurFile];

	if (directories)
		{
		iCurText = iCurTextDir;
		if (d.name()[1] != '' && d.name()[1] != '')
			{
			Hilight(iCurFile, FALSE);
			iCurFile=0;
			Hilight(iCurFile);
			}
		}
	else
		{
		iCurText = iCurTextFile;
		}

	const char* curText = iCurText;
	const char* curFile;

	if (iCurText == "")
		{
		Hilight(iCurFile, FALSE);
		iCurFile = 0;
		Hilight(iCurFile);

		return TRUE;
		}

	if (forward)
		{
      int   i;

		for (i=iCurFile+1; i < iDirectory->size(); i++)
			{
			const Dirent& d = (*iDirectory)[i];

			if (d.isDirectory() == directories)
				{
				char*	p;

				curFile = d.name();

				if (directories)
					curFile++;

				p = strstri(curFile, curText);

				if (p == curFile)
					{
					// found a match
					Hilight(iCurFile, FALSE);
					iCurFile = i;
					Hilight(iCurFile);

					return TRUE;
					}
				}
			}

		for (i=iCurFile; i >= 0; i--)
			{
			const Dirent& d = (*iDirectory)[i];

			if (d.isDirectory() == directories)
				{
				char*	p;

				curFile = d.name();

				if (directories)
					curFile++;

				p = strstri(curFile, curText);

				if (p == curFile)
					{
					// found a match
					Hilight(iCurFile, FALSE);
					iCurFile = i;
					Hilight(iCurFile);

					return TRUE;
					}
				}
			}
		}
	else
		{
		for (int i=iCurFile-1; i >= 0; i--)
			{
			const Dirent& d = (*iDirectory)[i];

			if (d.isDirectory() == directories)
				{
				char*	p;

				curFile = d.name();

				if (directories)
					curFile++;

#if defined(__UNIX__)
				p = strstr(curFile, curText);
#else
				p = strstri(curFile, curText);
#endif

				if (p == curFile)
					{
					// found a match
					Hilight(iCurFile, FALSE);
					iCurFile = i;
					Hilight(iCurFile);

					return TRUE;
					}
				}
			}
		}

	return FALSE;
}

/**
 * 
 */
void FileManager::Activate(SList<Viewer>& viewedFiles,
									int& currFile,
									BOOL escapeAbort)
{
	int		key = 0;
	iDriveMap = 0x0000;

#if defined(__OS2__)
	ULONG driveNo;

	DosQueryCurrentDisk(&driveNo, &iDriveMap);
#elif defined(__WIN32__)
	iDriveMap = GetLogicalDrives();
#elif defined(__MSDOS__)
	int curDisk = getdisk();
	int maxdisk = setdisk(curDisk);

	ULONG diskBit = 0x00000001;

	iDriveMap = 0;	// clear iDriveMap bitmask

	for (int disk = 0; disk < maxdisk; disk++, diskBit *= 2)
		{
		setdisk(disk);

		if (getdisk() == disk)
			iDriveMap |= diskBit;
		}

	setdisk(curDisk);
#endif

	setTextAttr(setupInfo.fileTextForeColor, setupInfo.fileTextBackColor);

#if 0
	for (int y=2; y < ti.screenheight; y++)
		{
		MoveToXY(1, y);
		clrline();
		}
#endif

	// clear the arrows that the viewer may left on the status line
	WaitSync();
	MoveToXY(di.screenwidth - 9, di.screenheight);
	setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
	tprintf("  ");
	ClearSync();

	Refresh();
	messageTime = gMessageClearTime; // allow message to be cleared by timer display thread

	do
		{
		key = getKey();

		if (key >= 0x0001 && key <= 0x001A && CtrlIsPressed())
			{
			int disk = key-1;

			if (IsValidDisk(disk))
				{
				int oldDisk = queryCurrentDisk()-'A';

				setdisk(disk);
				iCurDir = queryCurrentDir((char)(disk+'A'));
				if (iCurDir != "" && toupper(iCurDir[1]) == 'A'+disk)
					{
					iCurFile=0;
					iCurTextFile = "";
					iCurTextDir = "";
					iCurText = "";

					Refresh();
					DisplayDisks();
					}
				else
					{
					setdisk(oldDisk);
					iCurDir = queryCurrentDir(0);
					displayErrMsg(sformat("%c: drive not ready", disk+'A'));
					}
				}
			else
				{
				displayErrMsg(sformat("%c: invalid disk", disk+'A'));
				}
			}
		else
			{
			switch (key)
				{
		   	case 0x002A:	 // '*'
					{
					Refresh();
					}
				break;

				case 0xFF3B: // F1
					{
					extern CString manFile;

					if (viewHelp(manFile))
						{
						clearMsg();

						// clear the arrows that the viewer may have left on the status line
						WaitSync();
						MoveToXY(di.screenwidth - 9, di.screenheight);
						setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
						tprintf("  ");
						ClearSync();

						DisplayDisks();
						DisplayDirectoryInfo();
						DisplayDirectory();
						}
					}
				break;

				case 0xFF54: // Shift+F1
					{
#if defined(__OS2__)
					extern CString infFile;

					CString cmd = "start view "+infFile;
					system(cmd);
					_setcursortype(_NOCURSOR);
#elif defined(__DPMI32__)
#elif defined(__WIN32__)
#endif
					}
				break;

				case '?':
					{
					extern CString helpFile;

					if (viewHelp(helpFile))
						{
						clearMsg();

						// clear the arrows that the viewer may have left on the status line
						WaitSync();
						MoveToXY(di.screenwidth - 9, di.screenheight);
						setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
						tprintf("  ");
						ClearSync();

						DisplayDisks();
						DisplayDirectoryInfo();
						DisplayDirectory();
						}
					}
				break;

				// FileManager navigation commands

				case 0xFF47:	// home
					{
					if (iCurFile != 0)
						{
						if (iCurFile == (iCurColumn * iLinesPerColumn))
							{
							iCurFile = iCurColumn = 0;
							DisplayDirectory();
							}
						else
							{
							Hilight(iCurFile, FALSE);
							iCurFile = (iCurColumn * iLinesPerColumn);
							Hilight(iCurFile, TRUE);
							}
						}
					}
				break;

				case 0xFF4F:	// end
					{
					if (iCurFile == ((iCurColumn+iColumns) * iLinesPerColumn)-1 &&
						iCurFile != (iDirectory->size()-1))
						{
						iCurFile = iDirectory->size()-1;
						iCurColumn = ((iDirectory->size()-1) / iLinesPerColumn) - (iColumns-1);

						if (iCurColumn < 0)
							iCurColumn = 0;

						DisplayDirectory();
						}
					else
						{
						if (iCurFile != iDirectory->size()-1)
							{
							Hilight(iCurFile, FALSE);
							iCurFile = ((iCurColumn+iColumns) * iLinesPerColumn)-1;

							if (iCurFile > iDirectory->size()-1)
								{
								iCurFile = iDirectory->size()-1;
								iCurColumn = (iDirectory->size() / iLinesPerColumn) - (iColumns-1);

								if (iCurColumn < 0)
									iCurColumn = 0;

								DisplayDirectory();
								}
							else
								Hilight(iCurFile, TRUE);
							}
						}
					}
				break;

				case 0xFF4B:	// left
					{
					if (iCurFile-iLinesPerColumn >= 0)
						{
						Hilight(iCurFile, FALSE);

						iCurFile -= iLinesPerColumn;

						if (iCurFile < (iCurColumn * iLinesPerColumn))
							{
							iCurColumn--;
							DisplayDirectory();
							}
						else
							{
							Hilight(iCurFile);
							}
						}
					}
				break;

				case 0xFF4D:	// right
					{
					if (((iCurFile / iLinesPerColumn)+1) * iLinesPerColumn
							< iDirectory->size()
						)
						{
						Hilight(iCurFile, FALSE);

						iCurFile += iLinesPerColumn;

						if (iCurFile > iDirectory->size()-1)
							iCurFile = iDirectory->size()-1;

						if (iCurFile >= ((iCurColumn+iColumns) * iLinesPerColumn))
							{
							iCurColumn++;
							DisplayDirectory();
							}
						else
							{
							Hilight(iCurFile);
							}
						}
					}
				break;

				case 0xFF48:	// up
					{
					if (iCurFile > 0)
						{
						Hilight(iCurFile, FALSE);
						iCurFile--;

						if (iCurFile < (iCurColumn)*iLinesPerColumn)
							{
							iCurColumn--;
							DisplayDirectory();
							}
						else
							{
							Hilight(iCurFile);
							}
						}
					}
				break;

				case 0xFF50:	// down
					{
					if (iCurFile < iDirectory->size()-1)
						{
						Hilight(iCurFile, FALSE);
						iCurFile++;

						if (iCurFile >= (iCurColumn+iColumns)*iLinesPerColumn)
							{
							iCurColumn++;
							DisplayDirectory();
							}
						else
							{
							Hilight(iCurFile);
							}
						}
					}
				break;

				case 0xFF49:	// page-up
					{
					if (iCurFile > 0)
						{
						iCurFile -= iLinesPerColumn;
						iCurColumn--;

						if (iCurFile < 0)
							iCurFile = 0;

						if (iCurColumn < 0)
							iCurColumn = 0;

						DisplayDirectory();
						}
					}
				break;

				case 0xFF51:	// page-down
					{
					if (iCurFile < iDirectory->size()-1)
						{
						Hilight(iCurFile, FALSE);
						iCurFile += iLinesPerColumn;
						iCurColumn++;

						if (((iCurFile / iLinesPerColumn)+1) * iLinesPerColumn
							>= iDirectory->size())
							{
							iCurColumn--;
							}

						if (iCurFile > iDirectory->size()-1)
							iCurFile = iDirectory->size()-1;

						DisplayDirectory();
						}
					}
				break;

				case '\r':
					{
					const Dirent& d = (*iDirectory)[iCurFile];

					if (d.name()[1] == '' || d.name()[1] == '')
						{
						CString dirName = (((const char*)d.name())+1);
						CString prevDir = queryCurrentDir(0);

						directoryHistory.Add(new CString(iCurDir));
						chdir(dirName);

						iCurDir = queryCurrentDir(0);
						iCurFile = 0;
						iCurTextFile = "";
						iCurTextDir = "";
						iCurText = "";

						Refresh();

						if (dirName == "..")
							{
							CString	dir;
							const char* pslash = strrchr(prevDir, *pathSeparator());

							if (pslash)
								{
								pslash++;
								dir = pslash;

								Hilight(iCurFile, FALSE);

								for (int i=0; i < iDirectory->size(); i++)
									{
									const Dirent& d = (*iDirectory)[i];

									if (d.name()[1] == '' && dir == (((const char*)d.name())+1))
										{
										iCurFile=i;
										break;
										}
									}

								Hilight(iCurFile);
								}
							}
						}
					else
						{
						if (ViewFile(viewedFiles, currFile))
							return;
						}
					}
				break;

				// FileManager color settings

				case 0xFF3C: // F2
					{
					setupInfo.fileTextForeColor++;

					if (setupInfo.fileTextForeColor > 15)
						setupInfo.fileTextForeColor = 0;

					DisplayDirectory();
					}
				break;

				case 0xFF55: // SHIFT+F2
					{
					setupInfo.fileTextBackColor++;

					if (setupInfo.fileTextBackColor > 15)
						setupInfo.fileTextBackColor = 0;

					DisplayDirectory();
					}
				break;

				case 0xFF3D: // F3
					{
					setupInfo.curFileTextForeColor++;

					if (setupInfo.curFileTextForeColor > 15)
						setupInfo.curFileTextForeColor = 0;

					Hilight(iCurFile, TRUE);
					}
				break;

				case 0xFF56: // SHIFT+F3
					{
					setupInfo.curFileTextBackColor++;

					if (setupInfo.curFileTextBackColor > 15)
						setupInfo.curFileTextBackColor = 0;

					Hilight(iCurFile, TRUE);
					}
				break;

				case 0xFF3E: // F4
					{
					setupInfo.statusTextForeColor++;

					if (setupInfo.statusTextForeColor > 15)
						setupInfo.statusTextForeColor = 0;

					clearMsg();
					DisplayDirectoryInfo();
					}
				break;

				case 0xFF57: // SHIFT+F4
					{
					setupInfo.statusTextBackColor++;

					if (setupInfo.statusTextBackColor > 15)
						setupInfo.statusTextBackColor = 0;

					clearMsg();
					DisplayDirectoryInfo();
					}
				break;

				case 0xFF3F: // F5
					{
					setupInfo.diskTextForeColor++;

					if (setupInfo.diskTextForeColor > 15)
						setupInfo.diskTextForeColor = 0;

					DisplayDisks();
					}
				break;

				case 0xFF58: // SHIFT+F5
					{
					setupInfo.diskTextBackColor++;

					if (setupInfo.diskTextBackColor > 15)
						setupInfo.diskTextBackColor = 0;

					DisplayDisks();
					}
				break;

				case 0xFF40: // F6
					{
					setupInfo.curDiskTextForeColor++;

					if (setupInfo.curDiskTextForeColor > 15)
						setupInfo.curDiskTextForeColor = 0;

					DisplayDisks();
					}
				break;

				case 0xFF59: // SHIFT+F6
					{
					setupInfo.curDiskTextBackColor++;

					if (setupInfo.curDiskTextBackColor > 15)
						setupInfo.curDiskTextBackColor = 0;

					DisplayDisks();
					}
				break;

				// FileManager commands

				case '/':
				case ':':		// file manager menu
					{
					int 	key;
					BOOL	bFinished;

					do
						{
						displayPrompt("~View ~Edit ~Copy ~Del ~Ren ~Move ~Sort C~hdir M~kdir ~FileSpec");
						messageTime = 0; // prevent this message from being cleared by timer display thread
						key = getKey();
						clearMsg();

						bFinished = TRUE;

						switch (key)
							{
							case 'v':
							case 'V':
								{
								if (ViewFile(viewedFiles, currFile))
									return;
								}
							break;

							case 'e':
							case 'E':
								{
								EditFile(viewedFiles, currFile);
								clearMsg();
								DisplayDisks();
								DisplayDirectoryInfo();
								DisplayDirectory();
								}
							break;

							case 'c':
							case 'C':
								{
								CopyFile(viewedFiles, currFile);
								}
							break;

							case 'd':
							case 'D':
								{
								if (DeleteFile(viewedFiles, currFile))
									{
									/*
									iCurFile = 0;
									*/
									iCurTextFile = "";
									iCurTextDir = "";
									iCurText = "";
									Refresh();
									}
								}
							break;

							case 'r':
							case 'R':
								{
								if (RenameFile(viewedFiles, currFile))
									{
									/*
									iCurFile = 0;
									iCurTextFile = "";
									iCurTextDir = "";
									iCurText = "";
									*/
									Refresh();
									}
								}
							break;

							case 'm':
							case 'M':
								{
								if (MoveFile(viewedFiles, currFile))
									{
									/*
									iCurFile = 0;
									iCurTextFile = "";
									iCurTextDir = "";
									iCurText = "";
									*/
									Refresh();
									}
								}
							break;

							case 's':
							case 'S':
								{
								iCurFile = 0;
								iCurTextFile = "";
								iCurTextDir = "";
								iCurText = "";
								ChooseSortBy();
								}
							break;

							case 'h':
							case 'H':
								{
								ChangeDirectory();
								}
							break;

							case 'k':
							case 'K':
								{
								MakeDirectory();
								}
							break;

							case 'f':
							case 'F':
								{
								ChangeFileSpec();
								}
							break;

							case 0x001B:
								{
								clearMsg();
								}
							break;

							default:
								bFinished = FALSE;
							}
						}
					while (!bFinished);

					messageTime = gMessageClearTime; // allow message to be cleared by timer display thread
					}
				break;

				case 0xFF2E:	// Alt+C copy file
					{
					if (CopyFile(viewedFiles, currFile))
						{
						/*
						iCurFile = 0;
						iCurTextFile = "";
						iCurTextDir = "";
						iCurText = "";
						*/
						Refresh();
						}
					}
				break;

				case 0xFF53:	// DEL
				case 0xFF20:	// Alt+D delete file
					{
					if (DeleteFile(viewedFiles, currFile))
						{
						/*
						iCurFile = 0;
						*/
						iCurTextFile = "";
						iCurTextDir = "";
						iCurText = "";
						Refresh();
						}
					}
				break;

				case 0xFF12:	// Alt+E edit file
					{
					EditFile(viewedFiles, currFile);
					clearMsg();
					DisplayDisks();
					DisplayDirectoryInfo();
					DisplayDirectory();
					}
				break;

				case 0xFF21:	// Alt+F change filespec
					{
					ChangeFileSpec();
					}
				break;

				case 0xFF23:	// Alt+H change directory
					{
					ChangeDirectory();
					}
				break;

				case 0xFF52:	// INS
				case 0xFF25:	// Alt+K make directory
					{
					MakeDirectory();
					}
				break;

				case 0xFF32:	// Alt+M move file
					{
					MoveFile(viewedFiles, currFile);
					/*
					iCurFile = 0;
					iCurTextFile = "";
					iCurTextDir = "";
					iCurText = "";
					*/
					Refresh();
					}
				break;

				case 0xFF13:	// Alt+R rename file
					{
					if (RenameFile(viewedFiles, currFile))
						{
						/*
						iCurFile = 0;
						iCurTextFile = "";
						iCurTextDir = "";
						iCurText = "";
						*/
						Refresh();
						}
					}
				break;

				case 0xFF1F:	// Alt+S sort by
					{
					ChooseSortBy();
					}
				break;

				case 0xFF2F:	// Alt+V view file
					{
					if (ViewFile(viewedFiles, currFile))
						return;
					}
				break;

				case 0x0008:
					{
					if (ShiftIsPressed())
						{
						if (iCurTextDir != "")
							{
							if (!MatchSelect(FALSE, TRUE))
								{
								iCurText = iCurTextDir = SubCString(iCurTextDir, 1, iCurTextDir.length()-1);
								MatchSelect(FALSE, TRUE);
								}
							}

						DisplayDirectoryInfo();
						}
					else
						{
						if (iCurTextFile != "")
							{
							if (!MatchSelect(FALSE))
								{
								iCurText = iCurTextFile = SubCString(iCurTextFile, 1, iCurTextFile.length()-1);
								MatchSelect(FALSE);
								}
							}

						DisplayDirectoryInfo();
						}
					}
				break;

				case '\t':
					{
					if (iCurTextFile != "")
						{
						MatchSelect(TRUE);
						}

					DisplayDirectoryInfo();
					}
				break;

				case 0xFF0F:	// shift+tab
					{
					if (iCurTextDir != "")
						{
						MatchSelect(TRUE, TRUE);
						}

					DisplayDirectoryInfo();
					}
				break;

				case VKALT_1:
				case VKALT_2:
				case VKALT_3:
					{
					int oldColumns = iColumns;
					iColumns=(key-VKALT_1)+2;

					do
						{
						iColumnWidth = di.screenwidth / --iColumns;
						}
					while (iColumnWidth < 40);

					if (oldColumns != iColumns)
						DisplayDirectory();
					}
				break;

				case VKALT_Q:
				case VKALT_X:
				case VKALT_A:
					{
					cleanup(key);
					exit(EXIT_SUCCESS);
					}
				break;

				case 0xFF0E:
					{
					iCurText = iCurTextDir = iCurTextFile = "";
					Hilight(iCurFile, FALSE);
					iCurFile=0;
					Hilight(iCurFile);
					DisplayDirectoryInfo();
					}
				break;

				default:
					{
					if (!(key & 0xFF00) && key != 0x001B)
						{
						key = tolower(key);

						if (ShiftIsPressed())
							{
							iCurText = iCurTextDir = iCurTextDir + (char)key;

							if (!MatchSelect(TRUE, TRUE))
								{
								iCurText = iCurTextDir = SubCString(iCurTextDir, 1, iCurTextDir.length()-1);
								}

							DisplayDirectoryInfo();
							}
						else
							{
							iCurText = iCurTextFile = iCurTextFile + (char)key;

							if (!MatchSelect())
								{
								iCurText = iCurTextFile = SubCString(iCurTextFile, 1, iCurTextFile.length()-1);
								}

							DisplayDirectoryInfo();
							}
						}
					}
				}
			}
		}
	while (key != 0x001B);

	if (escapeAbort)
		{
		cleanup(0x001B);
		exit(EXIT_SUCCESS);
		}
	else
		{
		viewedFiles[currFile]->displayFileInfo();
		viewedFiles[currFile]->displayData();
		}
}
