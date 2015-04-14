/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: os.cpp 2.50 1996/03/17 22:34:16 jallen Exp jallen $";
#pragma warn +use

CString prevSrch;
CString	manFile;
CString	helpFile;
CString	infFile;
CString	configFile;

#if defined(__OS2__)
volatile TID		gTimeThread;
volatile BOOL		gTimeThreadEnd;
#elif defined(__WIN32__) && !defined(__DPMI32__)
volatile HANDLE	gTimeThread;
volatile BOOL		gTimeThreadEnd;
#endif

const int   		gMessageClearTime = 3;
CPCHAR 				stdinName = "<stdin>";
CPCHAR 				appName = "OnScreen/2";
DisplayCell*		screenBuf;
DisplayBufferInfo 	di;
int         		messageTime = 0;

#if defined(__OS2__)
PSZ         listSyncSemName = (PSZ)"\\SEM32\\OSSYNC";
#elif defined(__WIN32__) && !defined(__DPMI32__)
const char* listSyncSemName = "OSSYNC";
#endif

SList<Viewer> App::cViewedFiles;

/**
 * 
 */
BOOL viewHelp(CString fileName)
{
  	CString 	dir, file;
	int		key;

	if (splitPath(fileName, dir, file))
		{
		Directory	d(dir);

		d.fill(file);

		if (d.size() == 1)
			{
			BOOL	finished = FALSE;
			Viewer helpViewer(new FileInfo(d[0].dirName(),
													d[0].name(),
													d[0].fullName(),
													d[0].size(),
													d[0].time(),
													d[0].date()));


			helpViewer.loadFile();
			helpViewer.displayFileInfo();
			helpViewer.displayData();

			for (;!finished;)
				{
				key = getKey();

				switch (key)
					{
					case 0x001B:
						{
						finished = TRUE;
						}
					break;

					default:
						{
						helpViewer.handleKey(key);
						}
					}
				}
			}
		else
			{
			displayErrMsg(sformat("help file %s not found", (CPCHAR)fileName));

			return FALSE;
			}
		}

	return TRUE;
}

/**
 * 
 */
enum {
	HELP,
	IGNORESTDIN,
	RAW,
	NOSYNTAX,
	SEARCH,
	TEXTWITHLAYOUT,
	HIGHBIT
};

option options[] =
{
	{option::NO_ARGS,       HELP, 			"?",	      		"?",             			"display this screen"},
	{option::NO_ARGS,       IGNORESTDIN,	"ignorestdin",		"ignorestdin",         		"ignore stdin"},
	{option::REQUIRES_ARGS, RAW,	   		"raw",  			"raw stylename",    		"disables filter for stylename"},
	{option::REQUIRES_ARGS, NOSYNTAX,  		"nosyntax",			"nosyntax stylename",  		"disables syntax highlighting for stylename"},
	{option::REQUIRES_ARGS, SEARCH,	   		"search",  			"search regexp|plain",     	"search using regular expression or plain text"},
	{option::REQUIRES_ARGS, TEXTWITHLAYOUT,	"textwithlayout", 	"textwithlayout on|off",  	"turn textwithlayout on/off"},
	{option::REQUIRES_ARGS, HIGHBIT,		"highbit", 			"highbit on|off",  			"turn highbit filter on/off"},
	{option::NO_ARGS, 0, 0, 0, 0}
};

/**
 * 
 */
void showHelp(void)
{
#if defined(__OS2__)
	char* os = "OS/2";
#elif defined(__DPMI32__)
	char* os ="DOS";
#elif defined(__WIN32__) && !defined(__DPMI32__)
	char* os ="Win32";
#endif

	CString	msg;

    msg = sformat("OnScreen/2%s for %s, v%s, %s\n",
    #if defined(BETA)
             "\xE1",
    #else
             "",
    #endif
             os,
             getRCSVersion(gOnScreenRevision),
             gOnScreenCopyright);

	fprintf(stderr, (const char*)msg);
	fprintf(stderr, "usage is: os {options} {path/filespec}\n");
	printOptions(options);
	fprintf(stderr, "\n");

	exit(EXIT_FAILURE);
}

/**
 * 
 */
Style* findStyle(const CString styleName)
{
	if (styles.Size() != 0)
		{
		SListIterator<Style>	styleIterator(styles);
		Style*					style;

		for (style = styleIterator.GoHead();
				style;
				style = styleIterator.GetNext())
			{
			if (strcmpi(styleName, style->iStyleName) == 0)
				return style;
			}
		}

	return 0;
}

/**
 * 
 */
App::App(int argc, char* argv[]):
	iArgc(argc),
	iArgv(argv),
	iStartedBrowsing(FALSE),
	iAbort(FALSE)
{
}

/**
 * 
 */
App::~App()
{
}

/**
 * 
 */
SList<Viewer>& App::GetViewedFiles()
{
	return cViewedFiles;
}

/**
 * 
 */
int App::Init()
{
	GetDisplayBufferInfo(&di);

	if (di.screenwidth < 80)
		{
		cprintf("%s: display must be at least 80 columns wide\r\n", appName);

		return 3;
		}

	gStartupDir = queryCurrentDir(0);
#if defined(DEBUG)
	// tracer::traceOn();
#endif

#if defined(__OS2__)
	DosError(FERR_DISABLEHARDERR);	// disable OS/2,DOS hard error handling
#elif defined(__WIN32__) && !defined(__DPMI32__)
	// disable Win32 hard error handling
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
#endif

	CString	exePath = lowerCase(iArgv[0]);
	MTrace(("exePath=%s", (const char*)exePath));

	helpFile = nativePathName(exePath);
	strcpy((PCHAR)strrchr(helpFile, '.'), ".hlp");
	MTrace(("HelpFile=%s", (CPCHAR)helpFile));

	manFile = nativePathName(exePath);
	strcpy((PCHAR)strrchr(manFile, '.'), ".man");
	MTrace(("ManualFile=%s", (CPCHAR)manFile));

#if defined(__OS2__)
	infFile = nativePathName(exePath);
	strcpy((PCHAR)strrchr(infFile, '.'), ".inf");
	MTrace(("infFile=%s", (CPCHAR)infFile));
#endif

	configFile = nativePathName(exePath);
	strcpy((PCHAR)strrchr(configFile, '.'), ".set");
	MTrace(("configFile=%s", (CPCHAR)configFile));

	// setup default style
	gDefaultStyle = new Style("Default");
	styles.Add(gDefaultStyle);

	gDefaultStyle->AddExtensions(CString("*"));

	gDefaultStyle->iForeGndColor.SetItem(LIGHTGRAY);
	gDefaultStyle->iBackGndColor.SetItem(BLACK);
	gDefaultStyle->iSelectedForeGndColor.SetItem(WHITE);
	gDefaultStyle->iSelectedBackGndColor.SetItem(LIGHTGRAY);
	gDefaultStyle->iBoldColor.SetItem(WHITE);
	gDefaultStyle->iUnderlineColor.SetItem(BROWN);
	gDefaultStyle->iBoldUnderlineColor.SetItem(YELLOW);
	gDefaultStyle->iExpandTabs.SetItem(TRUE);
	gDefaultStyle->iHighBitFilter.SetItem(TRUE);
	gDefaultStyle->iTextWithLayout.SetItem(TRUE);
	gDefaultStyle->iTabWidth.SetItem(4);
	gDefaultStyle->iDisplayMode.SetItem(DM_TEXT);
	gDefaultStyle->iExternalFilterCmd.SetItem("");
	gDefaultStyle->iFilterEnabled.SetItem(TRUE);
	gDefaultStyle->iTopLineFormat.SetItem(1);
	gDefaultStyle->iWordBreak.SetItem(FALSE);
	gDefaultStyle->iSyntaxHighlightEnabled.SetItem(FALSE);
	gDefaultStyle->iSymbolsColor.SetItem(YELLOW);
	gDefaultStyle->iCommentColor.SetItem(LIGHTGRAY);
	gDefaultStyle->iStringColor.SetItem(LIGHTRED);
	gDefaultStyle->iReservedColor.SetItem(WHITE);
	gDefaultStyle->iPreprocessorColor.SetItem(YELLOW);
	gDefaultStyle->iNumberColor.SetItem(BROWN);
	gDefaultStyle->iIdentColor.SetItem(GREEN);
	gDefaultStyle->iSymbols.SetItem("");
	gDefaultStyle->iString.SetItem("");
	gDefaultStyle->iEscape.SetItem('\0');
	gDefaultStyle->iCaseSensitive.SetItem(TRUE);
	gDefaultStyle->iCaseConvert.SetItem(FALSE);
	gDefaultStyle->iCommentColumn.SetItem(0);
	gDefaultStyle->iLineContinuation.SetItem('\0');

#if defined(__OS2__)
	gDefaultStyle->iEditor.SetItem("e.exe");
#elif defined(__WIN32__)
	gDefaultStyle->iEditor.SetItem("notepad.exe");
#else
	gDefaultStyle->iEditor.SetItem("edit.com");
#endif

	loadConfig(configFile);

	// process command line arguments
	int   argno=1, optno;
	BOOL	ignoreStdin = FALSE;
	getoptErr(TRUE);

	while ((optno = getopt(argno, iArgv, options)) != -1)
		{
		switch (optno)
			{
			case HELP:
				{
				showHelp();
				}
			break;

			case IGNORESTDIN:
				{
				ignoreStdin = TRUE;
				}
			break;

			case SEARCH:
				{
				if (strcmpi(getoptArg(), "regexp") == 0)
					{
					setupInfo.regexpSearch = TRUE;
					}
				else
				if (strcmpi(getoptArg(), "plain") == 0)
					{
					setupInfo.regexpSearch = FALSE;
					}
				else
					{
					cprintf("%s: search setting must be either REGEXP or PLAIN\r\n", appName);

					return EXIT_FAILURE;
					}
				}
			break;

			case TEXTWITHLAYOUT:
				{
				if (strcmpi(getoptArg(), "on") == 0)
					{
					gDefaultStyle->iTextWithLayout.SetItem(TRUE);
					}
				else
				if (strcmpi(getoptArg(), "off") == 0)
					{
					gDefaultStyle->iTextWithLayout.SetItem(FALSE);
					}
				else
					{
					cprintf("%s: TextWithLayout setting must be either ON or OFF\r\n", appName);

					return EXIT_FAILURE;
					}
				}
			break;

			case HIGHBIT:
				{
				if (strcmpi(getoptArg(), "on") == 0)
					{
					gDefaultStyle->iHighBitFilter.SetItem(TRUE);
					}
				else
				if (strcmpi(getoptArg(), "off") == 0)
					{
					gDefaultStyle->iHighBitFilter.SetItem(FALSE);
					}
				else
					{
					cprintf("%s: HighBitFilter setting must be either ON or OFF\r\n", appName);

					return EXIT_FAILURE;
					}
				}
			break;

			case RAW:
				{
				CString	styleName = getoptArg();
				Style*	foundStyle = findStyle(styleName);

				if (foundStyle)
					{
					foundStyle->iFilterEnabled.SetItem(FALSE);
					}
				else
					{
					cprintf("%s: /raw undefined style '%s'\r\n", appName, (const char*)styleName);

					return EXIT_FAILURE;
					}
				}
			break;

			case NOSYNTAX:
				{
				CString	styleName = getoptArg();
				Style*	foundStyle = findStyle(styleName);

				if (foundStyle)
					{
					foundStyle->iSyntaxHighlightEnabled.SetItem(FALSE);
					}
				else
					{
					cprintf("%s: /nosyntax undefined style '%s'\r\n", appName, (const char*)styleName);

					return EXIT_FAILURE;
					}
				}
			break;
			}
		}

#if defined(__OS2__)
	VIOINTENSITY vioIntensity;

	vioIntensity.cb = sizeof(vioIntensity);
	vioIntensity.type = 2;
	vioIntensity.fs = 1;
	VioSetState((PVIOINTENSITY)&vioIntensity, 0);
#elif defined(__DPMI32__)
	union REGS	r;
	r.w.ax = 0x1003;
	r.h.bl = 0x00;
	int386(0x10, &r, &r);
#elif defined(__WIN32__)
	// Bright backgrounds is the default under Win32 (NT anyway)
#endif

	screenBuf = new DisplayCell[di.screenwidth * di.screenheight];
	MFailIf(screenBuf == 0);
	GetTextBuf(1, 1, di.screenwidth, di.screenheight, screenBuf);

#if defined(__OS2__)
	if (DosCreateMutexSem(listSyncSemName, &hMtxListSync, 0, FALSE) != NO_ERROR)
		{
		if (DosOpenMutexSem(listSyncSemName, &hMtxListSync) != NO_ERROR)
			{
			tprintf("%s: failed to initialise synchronization semaphore\r\n", appName);
			return EXIT_FAILURE;
			}
		}
#elif defined(__WIN32__) && !defined(__DPMI32__)
	if ((hMtxListSync = CreateMutex(0, FALSE, listSyncSemName)) == NULL)
		{
		if ((hMtxListSync = OpenMutex(MUTEX_ALL_ACCESS, FALSE, listSyncSemName)) == NULL)
			{
			cprintf("%s: failed to initialise synchronization semaphore\r\n", appName);
			return EXIT_FAILURE;
			}
		}
#endif

	_setcursortype(_NOCURSOR);

	if (!ignoreStdin && !isatty(0)) // handle redirected stdin
		{
		cViewedFiles.Add(new Viewer(new FileInfo("", stdinName, "", 0, systemTime(), systemDate(), TRUE)));
		}

	gCurrFile = -1;

	displayStdStatusLine();

	if (iArgc-argno > 0)
		{
		CString	arg = iArgv[argno];
		CString fullName;

		if (arg.length() == 2 && arg[2] == ':')
			{
			// if only drive specified then assume current directory
			// for that drive
			arg = arg + ".";
			}

		if (iArgc-argno == 1 && isDir(arg, fullName))
			{
			iStartedBrowsing = TRUE;
			gFileManager = new FileManager(fullName);
#if defined(__OS2__)
		if (gTimeThread == 0)
			gTimeThread = _beginthread(dispTime, 4096, 0);
#elif defined(__WIN32__) && !defined(__DPMI32__)
		if (gTimeThread == 0)
			gTimeThread = (HANDLE)_beginthread(dispTime, 4096, 0);
#endif
			gFileManager->Activate(cViewedFiles, gCurrFile, TRUE);
			}
		else
			{
			for (int i=argno; i < iArgc; i++)
				{
				AddFileList(iArgv[i], cViewedFiles);
				}
			}
		}
	else
		{
		if (isatty(0)) // don't browse for files if using redirected stdin
			{
			iStartedBrowsing = TRUE;
			gFileManager = new FileManager();
#if defined(__OS2__)
		if (gTimeThread == 0)
			gTimeThread = _beginthread(dispTime, 4096, 0);
#elif defined(__WIN32__) && !defined(__DPMI32__)
		if (gTimeThread == 0)
			gTimeThread = (HANDLE)_beginthread(dispTime, 4096, 0);
#endif
			gFileManager->Activate(cViewedFiles, gCurrFile, TRUE);
			}
		}

	return TRUE;
}

/**
 * 
 */
int App::Run()
{
	int   key = 0x001B;

	if (cViewedFiles.Size() > 0)
		{
#if defined(__OS2__)
		if (gTimeThread == 0)
			gTimeThread = _beginthread(dispTime, 4096, 0);
#elif defined(__WIN32__) && !defined(__DPMI32__)
		if (gTimeThread == 0)
			gTimeThread = (HANDLE)_beginthread(dispTime, 4096, 0);
#endif

		gCurrFile = 0;
		cViewedFiles[gCurrFile]->loadFile();

		cViewedFiles[gCurrFile]->displayFileInfo();
		cViewedFiles[gCurrFile]->displayData();

		while (!iAbort)
			{
			key = getKey();

			switch (key)
				{
				case 0xFF3B: // F1
					{
					if (viewHelp(manFile))
						{
						clearMsg();
						cViewedFiles[gCurrFile]->displayFileInfo();
						cViewedFiles[gCurrFile]->displayData();
						}
					}
				break;

				case 0xFF54: // Shift+F1
					{
#if defined(__OS2__)
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
					if (viewHelp(helpFile))
						{
						clearMsg();
						cViewedFiles[gCurrFile]->displayFileInfo();
						cViewedFiles[gCurrFile]->displayData();
						}
					}
				break;

				case 0x0009: // tab
					{
					if (gCurrStyle)
						{
						gCurrStyle->iTabWidth.SetItem((*gCurrStyle->iTabWidth.GetItem())+1);

						if (*gCurrStyle->iTabWidth.GetItem() > 20)
							{
							gCurrStyle->iTabWidth.SetItem(20);
							}
						else
							{
							cViewedFiles[gCurrFile]->displayData();

							displayMsg(sformat("%s tab width set to %i",
								(const char*)gCurrStyle->iStyleName,
								*gCurrStyle->iTabWidth.GetItem()));
							}
						}
					}
				break;

				case 0xFF0F: // shift-tab
					{
					if (gCurrStyle)
						{
						gCurrStyle->iTabWidth.SetItem((*gCurrStyle->iTabWidth.GetItem())-1);
						if (*gCurrStyle->iTabWidth.GetItem() < 1)
							{
							gCurrStyle->iTabWidth.SetItem(1);
							}
						else
							{
							cViewedFiles[gCurrFile]->displayData();

							displayMsg(sformat("%s tab width set to %i",
								(const char*)gCurrStyle->iStyleName,
								*gCurrStyle->iTabWidth.GetItem()));
							}
						}
					}
				break;

				case 0x0014: // CTRL+T
					{
					if (gCurrStyle)
						{
						gCurrStyle->iExpandTabs.SetItem(!*gCurrStyle->iExpandTabs.GetItem());
						cViewedFiles[gCurrFile]->displayData();

						displayMsg(sformat("%s tab expansion %s",
							(const char*)gCurrStyle->iStyleName,
							*gCurrStyle->iExpandTabs.GetItem() ? "on":"off"));
						}
					}
				break;

				case 'r':
				case 'R':
					{
					setupInfo.regexpSearch = !setupInfo.regexpSearch;
					cViewedFiles[gCurrFile]->clearSelected();

					displayMsg(sformat("Regular expression searches %s",
						setupInfo.regexpSearch ? "on":"off"));
					}
				break;

				case 'l':
				case 'L':
					{
					if (gCurrStyle)
						{
						gCurrStyle->iTextWithLayout.SetItem(!*gCurrStyle->iTextWithLayout.GetItem());
						cViewedFiles[gCurrFile]->displayData();

						displayMsg(sformat("%s text with layout %s",
							(const char*)gCurrStyle->iStyleName,
							*gCurrStyle->iTextWithLayout.GetItem() ? "on":"off"));
						}
				   }
				break;

				case 'c':
				case 'C':
					{
					cViewedFiles[gCurrFile]->clearSelected();
				   }
				break;

				case '.':
					{
					if (gCurrStyle)
						{
						gCurrStyle->iHighBitFilter.SetItem(!*gCurrStyle->iHighBitFilter.GetItem());
						cViewedFiles[gCurrFile]->displayData();

						displayMsg(sformat("%s highbit filter %s",
							(const char*)gCurrStyle->iStyleName,
							*gCurrStyle->iHighBitFilter.GetItem() ? "on":"off"));
						}
				   }
				break;

				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					{
					int fileSel = key - '1';

					if (fileSel >= 0 && fileSel < cViewedFiles.Size())
						{
						cViewedFiles[gCurrFile]->dumpFile();
						gCurrFile = fileSel;
						cViewedFiles[gCurrFile]->loadFile();
						cViewedFiles[gCurrFile]->displayFileInfo();
						cViewedFiles[gCurrFile]->displayData();
						}
					else
						{
						displayErrMsg(sformat("Cannot jump to file %i", fileSel+1));
						}
					}
				break;

				case 0x002D: // - remove file from buffer list
				case 0x001F: // CTRL- remove file from buffer list
					{
					if (cViewedFiles.Size() > 1)
						{
						if (cViewedFiles.Remove(cViewedFiles[gCurrFile]))
							{
							if (gCurrFile >= cViewedFiles.Size())
								{
								gCurrFile--;
								}

							cViewedFiles[gCurrFile]->loadFile();
							cViewedFiles[gCurrFile]->displayFileInfo();
							cViewedFiles[gCurrFile]->displayData();
							}
						}
					else
						{
						if (iStartedBrowsing)
							{
							cViewedFiles.Remove(cViewedFiles[gCurrFile]);
							gCurrFile = -1;
							clearMsg();
							gFileManager->Activate(cViewedFiles, gCurrFile, TRUE);
							}
						else
							{
							displayErrMsg("can't delete, no other files loaded");
							}
						}
					}
				break;

				case 0x001B: // escape
					{
					if (iStartedBrowsing)
						gFileManager->Activate(cViewedFiles, gCurrFile, TRUE);
					else
						iAbort = TRUE;
					}
				break;

				case 0xFF31: // ALT+N
				case 0xFF76: // CTRL+PGDN
					// next file
					{
					if (gCurrFile < cViewedFiles.Size()-1)
						{
						cViewedFiles[gCurrFile]->dumpFile();
						gCurrFile++;
						cViewedFiles[gCurrFile]->loadFile();
						cViewedFiles[gCurrFile]->displayFileInfo();
						cViewedFiles[gCurrFile]->displayData();
						}
					else
						{
						cViewedFiles[gCurrFile]->dumpFile();
						gCurrFile = 0;
						cViewedFiles[gCurrFile]->loadFile();
						cViewedFiles[gCurrFile]->displayFileInfo();
						cViewedFiles[gCurrFile]->displayData();
                  }
					}
            break;

				case 0xFF19: // ALT+P
            case 0xFF84: // CTRL+PGUP
					// previous file
               {
               if (gCurrFile > 0)
				  {
						cViewedFiles[gCurrFile]->dumpFile();
						gCurrFile--;
						cViewedFiles[gCurrFile]->loadFile();
						cViewedFiles[gCurrFile]->displayFileInfo();
						cViewedFiles[gCurrFile]->displayData();
						}
					else
						{
						cViewedFiles[gCurrFile]->dumpFile();
						gCurrFile = cViewedFiles.Size()-1;
						cViewedFiles[gCurrFile]->loadFile();
						cViewedFiles[gCurrFile]->displayFileInfo();
						cViewedFiles[gCurrFile]->displayData();
                  }
               }
				break;

				case 0xFF3C: // F2
					{
					if (gCurrStyle)
						{
						gCurrStyle->iForeGndColor.SetItem(BYTE((*gCurrStyle->iForeGndColor.GetItem())+1));

						if (*gCurrStyle->iForeGndColor.GetItem() > 15)
							gCurrStyle->iForeGndColor.SetItem(0);
						}

					cViewedFiles[gCurrFile]->displayData();
					}
				break;

				case 0xFF55: // SHIFT+F2
					{
					if (gCurrStyle)
						{
						gCurrStyle->iBackGndColor.SetItem(BYTE((*gCurrStyle->iBackGndColor.GetItem())+1));

						if (*gCurrStyle->iBackGndColor.GetItem() > 15)
							gCurrStyle->iBackGndColor.SetItem(0);
						}

					cViewedFiles[gCurrFile]->displayData();
					}
				break;

				case 0xFF3D: // F3
					{
					if (gCurrStyle)
						{
						gCurrStyle->iSelectedForeGndColor.SetItem(BYTE((*gCurrStyle->iSelectedForeGndColor.GetItem())+1));

						if (*gCurrStyle->iSelectedForeGndColor.GetItem() > 15)
							gCurrStyle->iSelectedForeGndColor.SetItem(0);
						}

					cViewedFiles[gCurrFile]->displayData();
					}
				break;

				case 0xFF56: // SHIFT+F3
					{
					if (gCurrStyle)
						{
						gCurrStyle->iSelectedBackGndColor.SetItem(BYTE((*gCurrStyle->iSelectedBackGndColor.GetItem())+1));

						if (*gCurrStyle->iSelectedBackGndColor.GetItem() > 15)
							gCurrStyle->iSelectedBackGndColor.SetItem(0);
						}

					cViewedFiles[gCurrFile]->displayData();
					}
				break;

				case 0xFF3E: // F4
					{
					setupInfo.statusTextForeColor++;

					if (setupInfo.statusTextForeColor > 15)
						setupInfo.statusTextForeColor = 0;

					clearMsg();
					cViewedFiles[gCurrFile]->displayFileInfo();
					}
				break;

				case 0xFF57: // SHIFT+F4
					{
					setupInfo.statusTextBackColor++;

					if (setupInfo.statusTextBackColor > 15)
						setupInfo.statusTextBackColor = 0;

					clearMsg();
					cViewedFiles[gCurrFile]->displayFileInfo();
					}
				break;

				case 0xFF3F: // F5
					{
					if (gCurrStyle)
						{
						gCurrStyle->iBoldColor.SetItem(BYTE((*gCurrStyle->iBoldColor.GetItem())+1));

						if (*gCurrStyle->iBoldColor.GetItem() > 15)
							gCurrStyle->iBoldColor.SetItem(0);
						}

					clearMsg();
					cViewedFiles[gCurrFile]->displayData();
					}
				break;

				case 0xFF40: // F6
					{
					if (gCurrStyle)
						{
						gCurrStyle->iUnderlineColor.SetItem(BYTE((*gCurrStyle->iUnderlineColor.GetItem())+1));

						if (*gCurrStyle->iUnderlineColor.GetItem() > 15)
							gCurrStyle->iUnderlineColor.SetItem(0);
						}

					clearMsg();
					cViewedFiles[gCurrFile]->displayData();
					}
				break;

				case 0xFF41: // F7
					{
					if (gCurrStyle)
						{
						gCurrStyle->iBoldUnderlineColor.SetItem(BYTE((*gCurrStyle->iBoldUnderlineColor.GetItem())+1));

						if (*gCurrStyle->iBoldUnderlineColor.GetItem() > 15)
							gCurrStyle->iBoldUnderlineColor.SetItem(0);
						}

					clearMsg();
					cViewedFiles[gCurrFile]->displayData();
					}
				break;

				case 'o':
				case 'O':
					{
					setupInfo.soundOn = !setupInfo.soundOn;

					displayMsg(sformat("Sound is %s", setupInfo.soundOn ? "on":"off"));
					}
				break;

				case 0x0013: // CTRL+S
					{
					// TODO: save the style file
					saveConfig(configFile);
					displayMsg("Settings file updated with current configuration");
					}
				break;

				case 'k':
				case 'K': // toggle keep files loaded
					{
					setupInfo.keepFilesLoaded = !setupInfo.keepFilesLoaded;

					displayMsg(sformat("Keep files loaded is %s",
						setupInfo.keepFilesLoaded ? "on":"off"));
					}
				break;

				case 'b':
				case 'B': // choose buffer
					{
					chooseBuffer(cViewedFiles, gCurrFile);
					}
				break;

				case 0x002B: // +, bring up file manager
				case 0xFF2F: // Alt+V, bring up file manager
					{
					if (!gFileManager)
						gFileManager = new FileManager();

					gFileManager->Activate(cViewedFiles, gCurrFile);
					}
				break;

				case 0xFF18:	// Alt+O spawn a shell
					{
					char* comspec = getenv("COMSPEC");
#if defined(__OS2__)
					APIRET rc = DosSuspendThread(gTimeThread);
					MFailIf(rc != NO_ERROR);
#elif defined(__WIN32__) && !defined(__DPMI32__)
					MEnsure(SuspendThread(gTimeThread) != 0xFFFFFFFF);
#endif

					_setcursortype(_NORMALCURSOR);

					textcolor(WHITE);
					textbackground(BLACK);
					clrscr();
					MoveToXY(1,1);
					cprintf("Use the EXIT command to return to %s\r\n\r\n", appName);
					spawnlp(P_WAIT, comspec, comspec, NULL);

					_setcursortype(_NOCURSOR);

					// make the status line display function
					// clear the status line
					extern BOOL bFirstTime;
					bFirstTime = TRUE;

#if defined(__OS2__)
					rc = DosResumeThread(gTimeThread);
					MFailIf(rc != NO_ERROR);
#elif defined(__WIN32__) && !defined(__DPMI32__)
					MEnsure(ResumeThread(gTimeThread) != 0xFFFFFFFF);
#endif

					Directory d(cViewedFiles[gCurrFile]->iFileInfo->iDirName);

					d.fill(cViewedFiles[gCurrFile]->iFileInfo->iName);

					if (cViewedFiles[gCurrFile]->fileHasChanged())
						{
						cViewedFiles[gCurrFile]->dumpFile(TRUE);
						cViewedFiles[gCurrFile]->iFileInfo->iFileSize = d[0].size();
						cViewedFiles[gCurrFile]->iFileInfo->iFileDate = d[0].date();
						cViewedFiles[gCurrFile]->iFileInfo->iFileTime = d[0].time();
						cViewedFiles[gCurrFile]->loadFile();
						}

					displayStdStatusLine();
					cViewedFiles[gCurrFile]->displayFileInfo(FALSE);
					cViewedFiles[gCurrFile]->displayData();
					}
				break;

				case 'x':
				case 'X':
				case 'q':
				case 'Q':
				case VKALT_Q:
				case VKALT_X:
				case VKCTRL_X:
                case VKALT_A:
					{
					iAbort = TRUE;
					}
			break;

				default:
					cViewedFiles[gCurrFile]->handleKey(key);
				}
			}
		}
	else
		{
		WaitSync();
		cleanup(VKALT_X);
		cprintf("No matching files");
		return EXIT_FAILURE;
		}

	WaitSync();
	cleanup(key);

	return EXIT_SUCCESS;
}

/**
 * 
 */
int main(int argc, char* argv[])
{
	App	anApp(argc, argv);

	if (anApp.Init())
		return anApp.Run();
	else
		return EXIT_FAILURE;
}
