/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#include <errno.h>

#if defined(__MSDOS__) && !defined(__DPMI32__)
#include <sys/stat.h>
#endif

DisplayBufferInfo di;

char* srcBasePath;

char OnScreenDest[128] = "C:\\WF7";
char configFile[128];
BOOL WantDLLVersion = FALSE;

#if defined(__OS2__)
char OnScreenDLLDest[128] = "C:\\WF7\\DLL";
#elif defined(__DPMI32__)
char OnScreenDLLDest[128] = "C:\\WF7\\DLL";
#elif defined(__WIN32__)
char OnScreenDLLDest[128] = "C:\\WF7\\DLL";
#endif

void chput(int x, int y, char ch, int count = 1);

#if defined(__OS2__)
typedef APIRET  APIENTRY (*PDosSetExtLIBPATH)(PCSZ  pszExtLIBPATH, ULONG flags);
#endif

/**
 * 
 */
class TextEdit
{
public:
	TextEdit(int len, int y, char* buffer);
	~TextEdit();

	int Go();
private:
	int 		iX;
	int 		iY;
	int 		iLen;
	int 		iCurPos;
	char*		iBuffer;
	char*		iCurText;
	DisplayCell*	iSavedArea;

	static BOOL		cInsertMode;
};

/**
 * 
 */
int getKey()
{
	int ch = getch();

	if (ch == 0x0)
		ch = getch() + 0xFF00;

	return ch;
}

BOOL	TextEdit::cInsertMode;

/**
 * 
 */
TextEdit::TextEdit(int len, int y, char* buffer):
	iX(((di.screenwidth - len) / 2)+1),
	iY(y),
	iLen(len),
	iBuffer(buffer),
	iCurPos(strlen(buffer)),
	iSavedArea(new DisplayCell[3*di.screenwidth])
{
	GetTextBuf(iX-1, iY-1, iX+iLen, iY+1, iSavedArea);

	textcolor(WHITE);
	textbackground(LIGHTGRAY);

	iCurText = new char[iLen+1];
	memset(iCurText, '\0', iLen+1);
	strcpy(iCurText, iBuffer);

	// draw box around edit
	chput(iX-1, iY-1, '�');
	chput(iX, iY-1, '�', iLen);
	chput(iX+iLen, iY-1, '�');
	chput(iX-1, iY, '�');
	chput(iX+iLen, iY, '�');
	chput(iX-1, iY+1, '�');
	chput(iX, iY+1, '�', iLen);
	chput(iX+iLen, iY+1, '�');

	gotoxy(iX, iY);
	cprintf("%-*s", iLen, iBuffer);

	if (!cInsertMode)
		_setcursortype(_NORMALCURSOR);
	else
		_setcursortype(_SOLIDCURSOR);

	gotoxy(iX, iY);
}

/**
 * 
 */
int TextEdit::Go()
{
	int 	key;

	do
		{
		gotoxy(iX, iY);
		cprintf("%-*s", iLen, iCurText);
		gotoxy(iX+iCurPos, iY);

		key = getKey();

		switch (key)
			{
			case 0xFF47: // home
				{
				iCurPos = 0;
				}
			break;

			case 0xFF4F: // end
				{
				iCurPos = strlen(iCurText);
				}
			break;

			case 0xFF4D: // right
				{
				if (iCurPos < iLen && iCurPos < strlen(iCurText))
					iCurPos++;
				}
			break;

			case 0xFF4B: // left
				{
				if (iCurPos > 0)
					iCurPos--;
				}
			break;

			case 0x0008:
				{
				if (iCurPos > 0)
					{
					--iCurPos;

					strcpy(&iCurText[iCurPos], &iCurText[iCurPos+1]);
					}
				}
			break;

			case 0xFF53: // DEL
				{
				if (iCurPos < iLen)
					{
					strcpy(&iCurText[iCurPos], &iCurText[iCurPos+1]);
					}
				}
			break;

			case 0xFF52: // INS
				{
				cInsertMode = !cInsertMode;
				if (!cInsertMode)
					_setcursortype(_NORMALCURSOR);
				else
					_setcursortype(_SOLIDCURSOR);
				}
			break;

			default:
				if (!(key & 0xFF00) && key != 0x000D && key != 0x000A)
					{
					if (cInsertMode)
						{
						if (strlen(iCurText) < iLen)
							{
							memmove(&iCurText[iCurPos+1], &iCurText[iCurPos], strlen(&iCurText[iCurPos])+1);
							iCurText[iCurPos++] = (char)key;
							}
						}
					else
						{
						if (iCurPos < iLen)
							{
							iCurText[iCurPos++] = (char)key;
							}
						}
					}
			}
		}
	while (key != 0x001B && key != 0x000D);

	if (key == 0x000D)
		{
		strcpy(iBuffer, iCurText);
		}

	PutTextBuf(iX-1, iY-1, iX+iLen, iY+1, iSavedArea);
	delete iSavedArea;

	return key;
}

/**
 * 
 */
TextEdit::~TextEdit()
{
	_setcursortype(_NOCURSOR);
}

/**
 * 
 */
BOOL MakeTree(char* tree)
{
	char* sdir = new char[513];

	memset(sdir, '\0', 513);
	strcpy(sdir, tree);

	char* eod = strtok(sdir, "\/");

	while (eod)
		{
		if (!strchr(":\\/", *(strchr(sdir, '\0')-1)))
			{
			if (mkdir(sdir) != 0 && errno == ENOENT)
				{
				free(sdir);
				return FALSE;
				}
			}

		*strchr(eod, '\0') = '/';
		eod = strtok(NULL, "\\/");
		}

	free(sdir);

	return TRUE;
}

/**
 * 
 */
BOOL CopyFile(const char* name, const char* srcPath, const char* dstPath)
{
	char src[128], dst[128];

	strcpy(src, srcBasePath);
	strcat(src, "\\");

	if (srcPath)
		{
		strcat(src, srcPath);
		strcat(src, "\\");
		}

	strcat(src, name);

	strcpy(dst, dstPath);

	if (!MakeTree(dst))
		{
		cprintf("\r\nFailed to make target directory tree");
		return FALSE;
		}

	strcat(dst, "\\");
	strcat(dst, name);

	cprintf("\r\n%s => %s", src, dst);
#if defined(__OS2__)
	APIRET rcDosCopy = DosCopy(src, dst, DCPY_EXISTING);

	if (rcDosCopy == NO_ERROR)
		return TRUE;
#elif defined(__WIN32__) || defined(__DPMI32__)
	if (CopyFile(src, dst, FALSE) == TRUE)
		return TRUE;
#elif defined(__MSDOS__)
	int	hSource, hDest;

	hSource = open(src, O_RDONLY | O_BINARY);

	if (hSource != -1)
		{
		int	cb;
		hDest = open(dst, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE);

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
					unlink(dst);

					cprintf(", failed disk full?");
					return FALSE;
					}
				}

			struct ftime	ft;

			if (getftime(hSource, &ft) != -1)
				setftime(hDest, &ft);

			if (close(hDest) == -1)
				{
				close(hSource);

				cprintf(", failed to close destination file");
				return FALSE;
				}

			if (close(hSource) == -1)
				{
				cprintf(", failed to close source file");
				return FALSE;
				}

			struct stat st;

			if (stat(src, &st) == -1)
				{
				}
			else
				{
				chmod(dst, st.st_mode);
				}

			return TRUE;
			}
		else
			{
			cprintf(", failed to open destination file");
			close(hSource);
			}
		}
	else
		{
		cprintf(", failed to open source file");
		return FALSE;
		}
#endif

	cprintf(", failed");
	return FALSE;
}

/**
 * 
 */
void printc(int y, const char* s)
{
	int x = ((di.screenwidth - strlen(s)) / 2)+1;
	gotoxy(x, y);
	cprintf("%s", s);
}

/**
 * 
 */
void chput(int x, int y, char ch, int count)
{
	DisplayCell	buf[132];
	DisplayCell*	pb;
	int			i;
	DisplayBufferInfo di;

	GetDisplayBufferInfo(&di);
	#if defined(__WIN32__) && !defined(__DPMI32__)
	di.screenheight=25;
	#endif

	for (i=0, pb = buf; i < count; i++, pb++)
		{
		pb->Char.AsciiChar = ch;
		pb->Attributes = di.attribute;
		}

	PutTextBuf(x, y, (x+i)-1, y, buf);
}

/**
 * 
 */
void txtput(int x, int y, const char* s)
{
	DisplayCell	buf[132];
	DisplayCell*	pb;
	int			i;
	DisplayBufferInfo di;

	GetDisplayBufferInfo(&di);
	#if defined(__WIN32__) && !defined(__DPMI32__)
	di.screenheight=25;
	#endif

	for (i=0, pb = buf; *s; i++, s++, pb++)
		{
		pb->Char.AsciiChar = *s;
		pb->Attributes = di.attribute;
		}

	PutTextBuf(x, y, (x+i)-1, y, buf);
}

/**
 * 
 */
void DrawBackground()
{
	textcolor(BLUE);
	textbackground(BLACK);
	clrscr();

	for (int y=1; y <= di.screenheight; y++)
		{
		chput(1, y, '�', di.screenwidth);
		}
}

/**
 * 
 */
void displayDescription(const char* s)
{
	int x, y;

	textcolor(LIGHTGRAY);
	textbackground(BLUE);

	window(1, di.screenheight-5, di.screenwidth, di.screenheight);
	x=1; y=1;
	gotoxy(x, y++);

	cprintf("�������������������������������� Description ���������������������������������Ŀ");
	cprintf("�                                                                              �");
	cprintf("�                                                                              �");
	cprintf("�                                                                              �");
	cprintf("��������������������������������������������������������������������������������");

	window(3, di.screenheight-4, di.screenwidth-1, di.screenheight-2);
	gotoxy(1, 1);
	cprintf("%s", s);

	window(1, 1, di.screenwidth, di.screenheight);
}

//------------------------------------oOo-----------------------------------//

int Welcome()
{
	int y = 0;

	textcolor(BLACK);
	textbackground(LIGHTGRAY);
#if defined(__OS2__)
	printc(++y, "��������������������������������������������������Ŀ");
	printc(++y, "�                 OnScreen/2 for OS/2 Installation �");
	printc(++y, "����������������������������������������������������");
#elif defined(__MSDOS__) || defined(__DPMI32__)
	printc(++y, "�������������������������������������������������Ŀ");
	printc(++y, "�                 OnScreen/2 for DOS Installation �");
	printc(++y, "���������������������������������������������������");
#elif defined(__WIN32__)
	printc(++y, "���������������������������������������������������Ŀ");
	printc(++y, "�                 OnScreen/2 for Win32 Installation �");
	printc(++y, "�����������������������������������������������������");
#endif

	y++;
	printc(++y, "����������������������������������������������������������Ŀ");
	printc(++y, "� Copyright (c) 1995 by John J. Allen, GPL v2 or later     �");
	printc(++y, "������������������������������������������������������������");

	y++;
	printc(++y, "��������������������� Installation Utility �������������������Ŀ");
#if defined(__OS2__)
	printc(++y, "� Welcome to OnScreen/2 for OS/2 installation                  �");
#elif defined(__MSDOS__) || defined(__DPMI32__)
	printc(++y, "� Welcome to OnScreen/2 for DOS installation                   �");
#elif defined(__WIN32__)
	printc(++y, "� Welcome OnScreen/2 for Win32 installation                    �");
#endif
	printc(++y, "� program. This program will install the OnScreen/2 fileviewer �");
	printc(++y, "� onto your system. The install requires up to 450k of disk    �");
	printc(++y, "� space. Press ENTER to continue or <ESC> to cancel.           �");
	printc(++y, "����������������������������������������������������������������");

	gotoxy(1, di.screenheight);
	textbackground(LIGHTGRAY);
	clreol();
	textcolor(RED);
	cprintf(" ENTER");
	textcolor(BLACK);
	cprintf("-Continue ");

	textcolor(RED);
	cprintf(" ESC");
	textcolor(BLACK);
	cprintf("-Cancel");

	int ch;

	for (;;)
		{
		ch = getKey();

		if (ch == VKESCAPE || ch == '\r')
			break;
		}

	return ch;
}

/**
 * 
 */
int ChooseOptions()
{
	int x=0, y = 0;

	gotoxy(1, di.screenheight);
	textbackground(LIGHTGRAY);
	clreol();

	textcolor(RED);
	cprintf(" I");
	textcolor(BLACK);
	cprintf("-Start Installation ");

	textcolor(RED);
	cprintf(" ENTER");
	textcolor(BLACK);
	cprintf("-Select ");

	textcolor(RED);
	cprintf(" ESC");
	textcolor(BLACK);
	cprintf("-Cancel");

	textcolor(BLUE);
	textbackground(BLACK);

	for (y=4; y < di.screenheight; y++)
		{
		chput(1, y, '�', di.screenwidth);
		}

	textcolor(BLACK);
	textbackground(LIGHTGRAY);

	int ch;

	enum Item
		{
		IT_FIRST,
		IT_DESTDIR,
#if (defined(__OS2__) || defined(__WIN32__)) && !defined(__DPMI32__)
		IT_WANTDLLS,
		IT_SYSDIR,		// eg. C:\OS2\APPS\DLL, C:\WINDOWS etc...
							// skipped for MS-DOS installation
#endif
		IT_STARTINSTALL,
		IT_LAST
		} curItem = IT_DESTDIR, prevItem;

	x=7;
	y=5;
	gotoxy(x, y++);
	cprintf("�������������������������������������������������������������������ͻ");
	gotoxy(x, y++);
	cprintf("� OnScreen/2 directory...       [ %s ]%*s�", OnScreenDest, 32-strlen(OnScreenDest), "");

#if (defined(__OS2__) || defined(__WIN32__)) && !defined(__DPMI32__)
	gotoxy(x, y++);
	cprintf("� DLL Installation              [ No ]                              �");
#endif

#if defined(__OS2__)
	gotoxy(x, y++);
	cprintf("� DLL Directory...              [ %s ]%*s�", OnScreenDLLDest, 32-strlen(OnScreenDLLDest), "");
#elif defined(__WIN32__) && !defined(__DPMI32__)
	gotoxy(x, y++);
	cprintf("� DLL Directory...              [ %s ]%*s�", OnScreenDLLDest, 32-strlen(OnScreenDLLDest), "");
#endif

	gotoxy(x, y++);
	cprintf("�������������������������������������������������������������������ĺ");
	gotoxy(x, y++);
	cprintf("� Start Installation                                                �");
	gotoxy(x, y++);
	cprintf("�������������������������������������������������������������������ͼ");

	for (;;)
		{
		textcolor(WHITE);
		textbackground(BLACK);

		switch (curItem)
			{
			case IT_DESTDIR:
				{
				gotoxy(8, 6);
				cprintf(" OnScreen/2 directory...       [ %s ]%*s",
					OnScreenDest, 32-strlen(OnScreenDest), "");

				displayDescription(
					"Press ENTER to specify the directory where the OnScreen/2 "
					"2.50 files should  be placed."
					);
				}
			break;

#if (defined(__OS2__) || defined(__WIN32__)) && !defined(__DPMI32__)
			case IT_WANTDLLS:
				{
				gotoxy(8, 7);
				if (WantDLLVersion)
					{
					cprintf(" DLL Installation              [ Yes ]                             ");
					displayDescription(
						"Press ENTER to select the non-DLL based version of OnScreen/2 2.50"
						);
					}
				else
					{
					cprintf(" DLL Installation              [ No ]                              ");
					displayDescription(
						"Press ENTER to select the DLL based version of OnScreen/2 2.50"
						);
					}
				}
			break;

			case IT_SYSDIR:
				{
				gotoxy(8, 8);

				cprintf(" DLL Directory...              [ %s ]%*s",
					OnScreenDLLDest, 32-strlen(OnScreenDLLDest), "");

				displayDescription(
					"Press ENTER to specify the directory where the OnScreen/2 "
					"2.50 DLL files     should be placed."
					);
				}
			break;
#endif

			case IT_STARTINSTALL:
				{
				#if defined(__MSDOS__) || defined(__DPMI32__)
					gotoxy(8, 8);
				#else
					gotoxy(8, 10);
				#endif

			cprintf(" Start Installation                                                ");

				displayDescription(
					"Press ENTER to begin installation of OnScreen/2 2.50. This will "
					"copy the     relevant files to the directories specified above."
					);
				}
			break;
			}

		ch = getKey();

		switch (ch)
			{
			case 0xFF50:
				{
				prevItem = curItem++;

				if (curItem == IT_LAST)
					curItem = IT_FIRST+1;
				}
			break;

			case 0xFF48:
				{
				prevItem = curItem--;

				if (curItem == IT_FIRST)
					curItem = IT_LAST-1;
				}
			break;

			case 'i':
			case 'I':
				{
				return '\r';
				}
			break;

			case '\r':
				{
				switch (curItem)
					{
					case IT_DESTDIR:
						{
						char buf[128];

						strcpy(buf, OnScreenDest);

						TextEdit	l(64, 6, buf);

						if (l.Go() != VKESCAPE)
							{
							strcpy(OnScreenDest, buf);
							}
						}
					break;

#if (defined(__OS2__) || defined(__WIN32__)) && !defined(__DPMI32__)
					case IT_WANTDLLS:
						{
						WantDLLVersion = !WantDLLVersion;
						}
					break;

					case IT_SYSDIR:
						{
						char buf[128];

						strcpy(buf, OnScreenDLLDest);

						TextEdit	l(64, 8, buf);

						if (l.Go() != VKESCAPE)
							{
							strcpy(OnScreenDLLDest, buf);
							}
						}
					break;
#endif

					case IT_STARTINSTALL:
						{
						return ch;
						}
					break;
					}
				}
			break;

			case VKESCAPE:
				{
				return ch;
				}
			break;
			}

		textcolor(BLACK);
		textbackground(LIGHTGRAY);

		switch (prevItem)
			{
			case IT_DESTDIR:
				{
				gotoxy(8, 6);
				cprintf(" OnScreen/2 directory...       [ %s ]%*s",
					OnScreenDest, 32-strlen(OnScreenDest), "");
				}
			break;

#if (defined(__OS2__) || defined(__WIN32__)) && !defined(__DPMI32__)
			case IT_WANTDLLS:
				{
				gotoxy(8, 7);
				if (WantDLLVersion)
					{
					cprintf(" DLL Installation              [ Yes ]                             ");
					}
				else
					{
					cprintf(" DLL Installation              [ No ]                              ");
					}
				}
			break;

			case IT_SYSDIR:
				{
				gotoxy(8, 8);
            cprintf(" DLL Directory...              [ %s ]%*s",
				OnScreenDLLDest, 32-strlen(OnScreenDLLDest), "");
				}
			break;
#endif

			case IT_STARTINSTALL:
				{
				#if defined(__MSDOS__) || defined(__DPMI32__)
					gotoxy(8, 8);
				#else
					gotoxy(8, 10);
				#endif

            cprintf(" Start Installation                                                ");
				}
			break;
			}
		}

	return ch;
}

/**
 * 
 */
BOOL UserAbort()
{
	if (kbhit() && getKey() == VKESCAPE)
		return TRUE;
	else
		return FALSE;
}

/**
 * 
 */
BOOL CopyFiles()
{
	BOOL ok = TRUE;
	int x, y;

	gotoxy(1, di.screenheight);
	textbackground(LIGHTGRAY);
	clreol();

	textcolor(RED);
	cprintf(" ESC");
	textcolor(BLACK);
	cprintf("-Cancel");

	window(1, di.screenheight-9, di.screenwidth, di.screenheight);
	x=1; y=1;
	gotoxy(x, y++);

	textcolor(LIGHTGRAY);
	textbackground(BLUE);
	cprintf("���������������������������������� Copying �����������������������������������Ŀ");
	cprintf("�                                                                              �");
	cprintf("�                                                                              �");
	cprintf("�                                                                              �");
	cprintf("�                                                                              �");
	cprintf("�                                                                              �");
	cprintf("�                                                                              �");
	cprintf("�                                                                              �");
	cprintf("��������������������������������������������������������������������������������");

	window(3, di.screenheight-8, di.screenwidth-1, di.screenheight-2);
	gotoxy(1, 1);

	// first copy files which are not specific to a particular OS
	if (!(ok=CopyFile("file_id.diz", "OnScreen", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("license.doc", "OnScreen", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("os.doc", "OnScreen", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("os.hlp", "OnScreen", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("os.man", "OnScreen", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("readme.txt", "OnScreen", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("usa.reg", "OnScreen", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("row.reg", "OnScreen", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	// now copy the OS specific ones
#if defined(__OS2__)
	if (!(ok=CopyFile("os.ico", "OnScreen\\OS2", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("os.inf", "OnScreen\\OS2", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("osreg.exe", "OnScreen\\OS2", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (WantDLLVersion)
		{
		if (!(ok=CopyFile("os.exe", "OnScreen\\OS2\\DLL", OnScreenDest)))
			{
			goto finishCopyFiles;
			}

		if (UserAbort())
			goto Abort;

		if (!(ok=CopyFile("c215mt.dll", "OnScreen\\OS2\\DLL", OnScreenDLLDest)))
			{
			goto finishCopyFiles;
			}

		if (UserAbort())
			goto Abort;

		if (!(ok=CopyFile("wf7lib10.dll", "OnScreen\\OS2\\DLL", OnScreenDLLDest)))
			{
			goto finishCopyFiles;
			}

		if (UserAbort())
			goto Abort;
		}
	else
		{
		if (!(ok=CopyFile("os.exe", "OnScreen\\OS2", OnScreenDest)))
			{
			goto finishCopyFiles;
			}

		if (UserAbort())
			goto Abort;
		}

#elif defined(__DPMI32__) || defined(__MSDOS__)
	if (!(ok=CopyFile("32rtm.exe", "OnScreen\\DOS", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("dpmi32vm.ovl", "OnScreen\\DOS", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("windpmi.386", "OnScreen\\DOS", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("osreg.exe", "OnScreen\\DOS", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (!(ok=CopyFile("os.exe", "OnScreen\\DOS", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

#elif defined(__WIN32__)
	if (!(ok=CopyFile("osreg.exe", "OnScreen\\Win32", OnScreenDest)))
		{
		goto finishCopyFiles;
		}

	if (UserAbort())
		goto Abort;

	if (WantDLLVersion)
		{
		if (!(ok=CopyFile("os.exe", "OnScreen\\Win32\\DLL", OnScreenDest)))
			{
			goto finishCopyFiles;
			}

		if (UserAbort())
			goto Abort;

		if (!(ok=CopyFile("cw3215mt.dll", "OnScreen\\Win32\\DLL", OnScreenDLLDest)))
			{
			goto finishCopyFiles;
			}

		if (UserAbort())
			goto Abort;

		if (!(ok=CopyFile("wf7lib10.dll", "OnScreen\\Win32\\DLL", OnScreenDLLDest)))
			{
			goto finishCopyFiles;
			}

		if (UserAbort())
			goto Abort;
		}
	else
		{
		if (!(ok=CopyFile("os.exe", "OnScreen\\Win32", OnScreenDest)))
			{
			goto finishCopyFiles;
			}

		if (UserAbort())
			goto Abort;
		}
#endif

goto finishCopyFiles;
Abort:
	ok = FALSE;
	cprintf(", user abort");

finishCopyFiles:
	window(1, 1, di.screenwidth, di.screenheight);

	return ok;
}

/**
 * 
 */
int findPreviousInstallation()
{
	char* exeDir = searchpath("os.exe");

	if (exeDir)
		{
		*strrchr(exeDir, '\\') = '\0';

		strcpy(OnScreenDest, exeDir);

		#if defined(__OS2__)
		char* dllDir = searchpath("wf7lib10.dll");

		if (dllDir)
			{
			*strrchr(dllDir, '\\') = '\0';
			strcpy(OnScreenDLLDest, dllDir);
			}
		else
			{
			// need to get LIBPATH from config,sys
			}
		#elif defined(__WIN32__)
		char* dllDir = searchpath("wf7lib10.dll");

		if (dllDir)
			{
			*strrchr(dllDir, '\\') = '\0';
			strcpy(OnScreenDLLDest, dllDir);
			}
		#endif
		}

	return exeDir != 0;
}

/**
 * 
 */
int main(int argc, char* argv[])
{
#if defined(DEBUG)
	// tracer::traceOn();
#endif

	srcBasePath = strdup(argv[0]);
	*strrchr(srcBasePath, '\\') = '\0';

	clrscr();
	gotoxy(1, 1);

	GetDisplayBufferInfo(&di);
	#if defined(__WIN32__) && !defined(__DPMI32__)
	di.screenheight=25;
	#endif

	DrawBackground();

	if (Welcome() != VKESCAPE)
		{
		findPreviousInstallation();

		if (ChooseOptions() != VKESCAPE)
			{
			char srcConfig[128];

			strcpy(srcConfig, srcBasePath);
			strcat(srcConfig, "\\OnScreen\\os.set");

			strcpy(configFile, OnScreenDest);
			strcat(configFile, "\\os.set");

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

			window(1, di.screenheight-9, di.screenwidth, di.screenheight);
			int x=1; int y=1;
			gotoxy(x, y++);

			textcolor(LIGHTGRAY);
			textbackground(BLUE);
			cprintf("������������������������������������������������������������������������������Ŀ");
			cprintf("�                                                                              �");
			cprintf("�                                                                              �");
			cprintf("�                                                                              �");
			cprintf("�                                                                              �");
			cprintf("�                                                                              �");
			cprintf("�                                                                              �");
			cprintf("�                                                                              �");
			cprintf("��������������������������������������������������������������������������������");

			window(3, di.screenheight-8, di.screenwidth-1, di.screenheight-2);
			gotoxy(1, 1);

			cprintf("Loading factory defaults\r\n");
			loadConfig(srcConfig);
			cprintf("Merging current user configuration\r\n");
			loadConfig(configFile);

			if (CopyFiles())
				{
				saveConfig(configFile);

				// view readme.
				char cmd[128];
#if defined(__OS2__)
				if (WantDLLVersion)
					{
					PDosSetExtLIBPATH pDosSetExtLIBPATH = 0;
					HMODULE hModule = 0;

					APIRET rc = DosQueryModuleHandle("DOSCALLS", &hModule);

					if (rc == 0)
						{
						rc = DosQueryProcAddr(hModule, 873, NULL, (PFN*)&pDosSetExtLIBPATH);

						if (rc == 0)
							{
							pDosSetExtLIBPATH(OnScreenDLLDest, 1);
							sprintf(cmd, "%s\\os %s\\readme.txt", OnScreenDest, OnScreenDest);
							}
						}

					if (pDosSetExtLIBPATH == 0)
						{
						// DosSetExtLIBPATH API not available so use the E editor to show
						// the Readme
						sprintf(cmd, "e %s\\readme.txt", OnScreenDest);
						}
					}
				else
					{
					sprintf(cmd, "%s\\os %s\\readme.txt", OnScreenDest, OnScreenDest);
					}
#elif defined(__DPMI32__) || defined(__MSDOS__)
					chdir(OnScreenDest);
					setdisk(toupper(OnScreenDest[0])-'A');
					sprintf(cmd, "%s\\os %s\\readme.txt", OnScreenDest, OnScreenDest);
#elif defined(__WIN32__)
					chdir(OnScreenDest);
					sprintf(cmd, "%s\\os %s\\readme.txt", OnScreenDest, OnScreenDest);
#endif
				system(cmd);
				}
			}
		}

	gotoxy(1, di.screenheight);

	return 0;
}
