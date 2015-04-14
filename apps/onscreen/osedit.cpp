/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: osedit.cpp 2.50 1996/03/17 22:34:52 jallen Exp jallen $";
#pragma warn +use

BOOL	LineEdit::cInsertMode;

/**
 * 
 */
LineEdit::LineEdit(int len, char* prompt, char* buffer,
						SList<CString>* history, BOOL updateHistory):
	iX(1),
	iY(di.screenheight),
	iLen(len),
	iHistory(history),
	iUpdateHistory(updateHistory),
	iPrompt(prompt),
	iBuffer(buffer),
	iCurPos(0)
{
	iPrompt = iPrompt + ":[" + CString(iLen) + "]";

	iCurText = new char[iLen+1];
	memset(iCurText, '\0', iLen+1);
	strcpy(iCurText, iBuffer);

	WaitSync();
	messageTime=0;
	MoveToXY(iX, iY);
	setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
	clrline();
	tprintf("%s", (CPCHAR)iPrompt);

	if (!cInsertMode)
		_setcursortype(_NORMALCURSOR);
	else
		_setcursortype(_SOLIDCURSOR);

	iX += strlen(prompt)+2;
	MoveToXY(iX, iY);

	ClearSync();
}

/**
 * 
 */
int LineEdit::Go()
{
	int 	key;
	int   histPos = 0;

	if (iHistory)
		histPos = iHistory->Size();

	do
		{
		WaitSync();
		MoveToXY(iX, iY);
		tprintf("%-*s", iLen, iCurText);
		MoveToXY(iX+iCurPos, iY);
		ClearSync();

		key = getKey();

		switch (key)
			{
			case VKALT_X:
			case VKCTRL_X:
         case VKALT_A:
				{
				cleanup(key);
				exit(EXIT_SUCCESS);
				}
			break;

			case 0xFF48: // up history
				{
				if (iHistory && histPos > 0)
					{
					histPos--;

					if (iHistory->Size() > 0)
						{
						memset(iCurText, '\0', iLen+1);
						MTrace(("Restoring %s from history", (CPCHAR)*(*iHistory)[histPos]));
						strncpy(iCurText, *(*iHistory)[histPos], iLen);
						iCurPos = 0;
						}
					}
				}
			break;

			case 0xFF50: // down history
				{
				if (iHistory && histPos < iHistory->Size()-1)
					{
					histPos++;

					if (iHistory->Size() > 0)
						{
						memset(iCurText, '\0', iLen+1);
						MTrace(("Restoring %s from history", (CPCHAR)*(*iHistory)[histPos]));
						strncpy(iCurText, *(*iHistory)[histPos], iLen);
						iCurPos = 0;
						}
					}
				}
			break;

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

		if (iHistory && iUpdateHistory)
			{
			MTrace(("Saving %s to history", iCurText));
			iHistory->Add(new CString(iCurText));
			}
		}

	clearMsg();
	messageTime = gMessageClearTime;

	return key;
}

/**
 * 
 */
LineEdit::~LineEdit()
{
	_setcursortype(_NOCURSOR);
}
