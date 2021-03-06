/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: osgetch.cpp 2.50 1996/03/17 22:34:52 jallen Exp jallen $";
#pragma warn +use

/**
 * WIN32 Implementation of kbhit(), getch() & ungetch()
 * for Borland C++ 4.02
 * 
 * NOTE: The origin of this code is unknown
 */

#if defined(__WIN32__) && !defined(__DPMI32__)

DWORD	dwControlKeyState;

#define KBD_TEXTMODE (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT)

unsigned char _cFlag = 0;       /* Flag presence of un-gotten char */
unsigned char _cChar = 0;       /* The ungotten char               */

/**
 * The following table maps NT virtual keycodes to PC BIOS keyboard
 * values.  For each virtual keycode there are four possible BIOS
 * values: normal, shift, Ctrl, and ALT.  Key combinations that have
 * no BIOS equivalent have a value of -1, and are ignored.  Extended
 * (non-ASCII) key values have bit 8 set to 1 using the EXT macro.
 */

#define EXT(key)    ((key)+0x100)
#define ISEXT(val)  ((val)&0x100)
#define EXTVAL(val) ((val)&0xff)

struct kbd
{
	 short keycode;            /* virtual keycode */
	 short normal;             /* BIOS keycode - normal */
	 short shift;              /* BIOS keycode - Shift- */
	 short ctrl;               /* BIOS keycode - Ctrl- */
	 short alt;                /* BIOS keycode - Alt- */
} kbdtab [] =
{
/*    Virtual key    Normal      Shift       Control     Alt */
	 { VK_BACK,      0x08,       0x08,       0x7f,       EXT(14)  },
	 { VK_TAB,       0x09,       EXT(15),    EXT(148),   EXT(165) },
	 { VK_RETURN,    0x0d,       0x0d,       0x0a,       EXT(166) },
	 { VK_ESCAPE,    0x1b,       0x1b,       0x1b,       EXT(1)   },
	 { VK_SPACE,     0x20,       0x20,       EXT(3),     0x20,    },
	 { '0',          '0',        ')',        -1,         EXT(129) },
	 { '1',          '1',        '!',        -1,         EXT(120) },
	 { '2',          '2',        '@',        EXT(0),     EXT(121) },
	 { '3',          '3',        '#',        -1,         EXT(122) },
	 { '4',          '4',        '$',        -1,         EXT(123) },
	 { '5',          '5',        '%',        -1,         EXT(124) },
	 { '6',          '6',        '^',        0x1e,       EXT(125) },
	 { '7',          '7',        '&',        -1,         EXT(126) },
	 { '8',          '8',        '*',        -1,         EXT(127) },
	 { '9',          '9',        '(',        -1,         EXT(128) },

	 { 'A',          'a',        'A',        0x01,       EXT(30)  },
	 { 'B',          'b',        'B',        0x02,       EXT(48)  },
	 { 'C',          'c',        'C',        0x03,       EXT(46)  },
	 { 'D',          'd',        'D',        0x04,       EXT(32)  },
	 { 'E',          'e',        'E',        0x05,       EXT(18)  },
	 { 'F',          'f',        'F',        0x06,       EXT(33)  },
	 { 'G',          'g',        'G',        0x07,       EXT(34)  },
	 { 'H',          'h',        'H',        0x08,       EXT(35)  },
	 { 'I',          'i',        'I',        0x09,       EXT(23)  },
	 { 'J',          'j',        'J',        0x0a,       EXT(36)  },
	 { 'K',          'k',        'K',        0x0b,       EXT(37)  },
	 { 'L',          'l',        'L',        0x0c,       EXT(38)  },
	 { 'M',          'm',        'M',        0x0d,       EXT(50)  },
	 { 'N',          'n',        'N',        0x0e,       EXT(49)  },
	 { 'O',          'o',        'O',        0x0f,       EXT(24)  },
	 { 'P',          'p',        'P',        0x10,       EXT(25)  },
	 { 'Q',          'q',        'Q',        0x11,       EXT(16)  },
	 { 'R',          'r',        'R',        0x12,       EXT(19)  },
	 { 'S',          's',        'S',        0x13,       EXT(31)  },
	 { 'T',          't',        'T',        0x14,       EXT(20)  },
	 { 'U',          'u',        'U',        0x15,       EXT(22)  },
	 { 'V',          'v',        'V',        0x16,       EXT(47)  },
	 { 'W',          'w',        'W',        0x17,       EXT(17)  },
	 { 'X',          'x',        'X',        0x18,       EXT(45)  },
	 { 'Y',          'y',        'Y',        0x19,       EXT(21)  },
	 { 'Z',          'z',        'Z',        0x1a,       EXT(44)  },

	 { VK_PRIOR,     EXT(73),    EXT(73),    EXT(132),   EXT(153) },
	 { VK_NEXT,      EXT(81),    EXT(81),    EXT(118),   EXT(161) },
	 { VK_END,       EXT(79),    EXT(79),    EXT(117),   EXT(159) },
	 { VK_HOME,      EXT(71),    EXT(71),    EXT(119),   EXT(151) },
	 { VK_LEFT,      EXT(75),    EXT(75),    EXT(115),   EXT(155) },
	 { VK_UP,        EXT(72),    EXT(72),    EXT(141),   EXT(152) },
	 { VK_RIGHT,     EXT(77),    EXT(77),    EXT(116),   EXT(157) },
	 { VK_DOWN,      EXT(80),    EXT(80),    EXT(145),   EXT(160) },
	 { VK_INSERT,    EXT(82),    EXT(82),    EXT(146),   EXT(162) },
	 { VK_DELETE,    EXT(83),    EXT(83),    EXT(147),   EXT(163) },
	 { VK_NUMPAD0,   '0',        EXT(82),    EXT(146),   -1       },
	 { VK_NUMPAD1,   '1',        EXT(79),    EXT(117),   -1       },
	 { VK_NUMPAD2,   '2',        EXT(80),    EXT(145),   -1       },
	 { VK_NUMPAD3,   '3',        EXT(81),    EXT(118),   -1       },
	 { VK_NUMPAD4,   '4',        EXT(75),    EXT(115),   -1       },
	 { VK_NUMPAD5,   '5',        EXT(76),    EXT(143),   -1       },
	 { VK_NUMPAD6,   '6',        EXT(77),    EXT(116),   -1       },
	 { VK_NUMPAD7,   '7',        EXT(71),    EXT(119),   -1       },
	 { VK_NUMPAD8,   '8',        EXT(72),    EXT(141),   -1       },
	 { VK_NUMPAD9,   '9',        EXT(73),    EXT(132),   -1       },
	 { VK_MULTIPLY,  '*',        '*',        EXT(150),   EXT(55)  },
	 { VK_ADD,       '+',        '+',        EXT(144),   EXT(78)  },
	 { VK_SUBTRACT,  '-',        '-',        EXT(142),   EXT(74)  },
	 { VK_DECIMAL,   '.',        '.',        EXT(83),    EXT(147) },
	 { VK_DIVIDE,    '/',        '/',        EXT(149),   EXT(164) },
	 { VK_F1,        EXT(59),    EXT(84),    EXT(94),    EXT(104) },
	 { VK_F2,        EXT(60),    EXT(85),    EXT(95),    EXT(105) },
	 { VK_F3,        EXT(61),    EXT(86),    EXT(96),    EXT(106) },
	 { VK_F4,        EXT(62),    EXT(87),    EXT(97),    EXT(107) },
	 { VK_F5,        EXT(63),    EXT(88),    EXT(98),    EXT(108) },
	 { VK_F6,        EXT(64),    EXT(89),    EXT(99),    EXT(109) },
	 { VK_F7,        EXT(65),    EXT(90),    EXT(100),   EXT(110) },
	 { VK_F8,        EXT(66),    EXT(91),    EXT(101),   EXT(111) },
	 { VK_F9,        EXT(67),    EXT(92),    EXT(102),   EXT(112) },
	 { VK_F10,       EXT(68),    EXT(93),    EXT(103),   EXT(113) },
	 { VK_F11,       EXT(133),   EXT(135),   EXT(137),   EXT(139) },
	 { VK_F12,       EXT(134),   EXT(136),   EXT(138),   EXT(140) },
	 { 0xdc,         '\\',       '|',        0x1c,       EXT(43)  },
	 { 0xbf,         '/',        '?',        -1,         EXT(53)  },
	 { 0xbd,         '-',        '_',        0x1f,       EXT(130) },
	 { 0xbb,         '=',        '+',        -1,         EXT(131) },
	 { 0xdb,         '[',        '{',        0x1b,       EXT(26)  },
	 { 0xdd,         ']',        '}',        0x1d,       EXT(27)  },
	 { 0xba,         ';',        ':',        -1,         EXT(39)  },
	 { 0xde,         '\'',       '"',        -1,         EXT(40)  },
	 { 0xbc,         ',',        '<',        -1,         EXT(51)  },
	 { 0xbe,         '.',        '>',        -1,         EXT(52)  },
	 { 0xc0,         '`',        '~',        -1,         EXT(41)  },

	 { -1,           -1,         -1       }  /** END **/
};

//------------------------------------oOo-----------------------------------//
//
// Name            kbhit - checks for recent keystrokes
//
// Usage           int kbhit(void);
//
// Prototype in    conio.h
//
// Description     kbhit checks to see if a keystroke is currently
// 					 available. Any available keystrokes can be retrieved with
//                 getch or getche.
//
// Return value    If a keystroke is available, kbhit returns a
// 					 non-zero integer. If not, it returns 0.
//
//------------------------------------oOo-----------------------------------//

int kbhit(void)
{
	 INPUT_RECORD inp;
	 DWORD nread, nevents;
	 HANDLE hin;

    if (_cFlag)             /* has a character been ungetch'd? */
        return (1);

    hin = GetStdHandle(STD_INPUT_HANDLE);

    /* Discard console events until we come to a keydown event,
	  * or the event queue is empty.
	  */
	 while (GetNumberOfConsoleInputEvents(hin, &nevents) == TRUE && nevents > 0)
	 {
		  /* Take a look at the first input event in the queue.
			* If we can't peek at it, something bad happened.
			*/
		  if (PeekConsoleInput(hin, &inp, 1, &nread) != TRUE)
				break;

		  /* If the event is a key-down, a key has been hit.
			*/
		  if ((inp.EventType & KEY_EVENT) != 0 &&
				 inp.Event.KeyEvent.bKeyDown != 0)
				{
				dwControlKeyState = inp.Event.KeyEvent.dwControlKeyState;
				return (1);
				}

		  /* It's not a key-down event, discard it.
			* NOTE: this discards all other console input events, including
			* mouse events!  Thus, kbhit() should not be used in programs
			* that track the mouse or window resizing.
			*/
		  if (ReadConsoleInput(hin, &inp, 1, &nread) != TRUE)
				break;
	 }
	 return (0);
}

//------------------------------------oOo-----------------------------------//
//
// Name            getch - gets character from console
//
// Usage           int getch(void);
//
// Prototype in    conio.h
//
// Description     getch is a function that reads a single character directly
//                 from the console, without echoing.
//
// Return value    getch and getche return the character read. There is no
//                 error return for these two functions.
//
//------------------------------------oOo-----------------------------------//

int getch()
{
	HANDLE			hConsole = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD	buf[1];
	DWORD 			kbdmode;
	DWORD				nchars;
	int 				c = -1;
   static int extend = -1;

    /* If a previous ungetch() call has been performed, return
     * the key that it saved.
     */
	if (_cFlag)                     /* Previous ungetch? */
		{
		_cFlag = 0;                 /* Reset the flag   */

		return (unsigned char)(_cChar & 0xff);     /* Get the un-got char  */
		}

	 /* If a previous call returned an extended code, return
	  * the scan code for the key.
	  */
	if (extend != -1)
		{
		c = extend;
		extend = -1;                /* Reset the flag */

		return (unsigned char)c;    /* Return the extended scancode */
	 	}

	if (GetConsoleMode(hConsole, &kbdmode) != TRUE)
		return -1;

	if ((kbdmode & KBD_TEXTMODE) != 0)
		if (SetConsoleMode(hConsole, kbdmode & ~KBD_TEXTMODE) != TRUE)
			return -1;

	while (c == -1)
		{
		if (ReadConsoleInput(hConsole, buf, 1, &nchars))
			{
			if (buf[0].EventType & KEY_EVENT)
				{
				KEY_EVENT_RECORD*	kEvt = &buf[0].Event.KeyEvent;

				switch (kEvt->bKeyDown)
					{
					case 0:
						{
						if (kEvt->wVirtualKeyCode == VK_MENU && kEvt->uChar.AsciiChar != 0)
							{
							dwControlKeyState = kEvt->dwControlKeyState;
							c = kEvt->uChar.AsciiChar;
							}
						}
					break;

					case 1:
						{
						if (kEvt->dwControlKeyState & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED))
							{
							struct kbd *k;

							for (k = &kbdtab[0]; kEvt->wVirtualKeyCode != k->keycode && k->keycode != -1; k++)
								 ;

							if (k->keycode != -1 && k->alt != -1)
								{
								switch (k->keycode)
									{
									case VK_INSERT:
									case VK_DELETE:
									case VK_HOME:
									case VK_END:
									case VK_PRIOR:
									case VK_NEXT:
									case VK_LEFT:
									case VK_UP:
									case VK_RIGHT:
									case VK_DOWN:
									case VK_RETURN:
									case VK_DIVIDE:
										{
										if (kEvt->dwControlKeyState & ENHANCED_KEY)
											{
											dwControlKeyState = kEvt->dwControlKeyState;
											c = k->alt;
											}
										}
									break;

									default:
										{
										dwControlKeyState = kEvt->dwControlKeyState;
										c = k->alt;
										}
									}

								if (c != -1 && ISEXT(c))
									{
									extend = EXTVAL(c);
									c = 0;
									}
								}
							}
						else
						if (kEvt->dwControlKeyState & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED))
							{
							struct kbd *k;

							for (k = &kbdtab[0]; kEvt->wVirtualKeyCode != k->keycode && k->keycode != -1; k++)
								 ;

							if (k->keycode != -1 && k->ctrl != -1)
								{
								dwControlKeyState = kEvt->dwControlKeyState;
								c = k->ctrl;

								if (ISEXT(c))
									{
									extend = EXTVAL(c);
									c = 0;
									}
								}
							}
						else
							{
							if ((kEvt->uChar.AsciiChar & 0x00FF) != 0)
								{
								dwControlKeyState = kEvt->dwControlKeyState;
								c = kEvt->uChar.AsciiChar & 0x00FF;
								}
							else
								{
								struct kbd *k;

								for (k = &kbdtab[0]; kEvt->wVirtualKeyCode != k->keycode && k->keycode != -1; k++)
									 ;

								if (k->keycode != -1 && k->ctrl != -1)
									{
									dwControlKeyState = kEvt->dwControlKeyState;
									c = k->ctrl;
									}

								if (kEvt->dwControlKeyState & SHIFT_PRESSED)
									{
									dwControlKeyState = kEvt->dwControlKeyState;
									c = k->shift;

									if (ISEXT(c))
										{
										extend = EXTVAL(c);
										c = 0;
										}
									}
								else
									{
									dwControlKeyState = kEvt->dwControlKeyState;
									c = k->normal;

									if (ISEXT(c))
										{
										extend = EXTVAL(c);
										c = 0;
										}
									}
								}
							}
						}
					break;
					}
				}
			}
		}

	 if ((kbdmode & KBD_TEXTMODE) != 0)
		  if (SetConsoleMode(hConsole, kbdmode) != TRUE)
				return -1;

	 return (unsigned char)c;
}

//------------------------------------oOo-----------------------------------//
//
// Name            ungetch - ungets character to console
//
// Usage           int ungetch(int c);
//
// Prototype in    conio.h
//
// Description     ungetch pushes the character c back to the console,
// 					 causing c to be the next character read. The ungetch
// 					 function fails if it is called more than once before
// 					 the next read.
//
// Return value    ungetch returns the character c if it is successful.
// 					 return value of EOF indicates an error.
//
//------------------------------------oOo-----------------------------------//

int ungetch(int c)
{
	 if (_cFlag)
		  return(EOF);

	 _cFlag = 1;
	 return ((_cChar = c) & 0xff);
}

//------------------------------------oOo-----------------------------------//

#endif	// __WIN32__

//------------------------------------oOo-----------------------------------//

#if defined(__DPMI32__)
#include <bios.h>

// Replacement for getch() and kbhit() under DPMI32.

unsigned char _cFlag = 0;       /* Flag presence of un-gotten char */
unsigned char _cChar = 0;       /* The ungotten char               */

//------------------------------------oOo-----------------------------------//
//
// Name            kbhit - checks for recent keystrokes
//
// Usage           int kbhit(void);
//
// Prototype in    conio.h
//
// Description     kbhit checks to see if a keystroke is currently
// 					 available. Any available keystrokes can be retrieved with
//                 getch or getche.
//
// Return value    If a keystroke is available, kbhit returns a
// 					 non-zero integer. If not, it returns 0.
//
//------------------------------------oOo-----------------------------------//

int kbhit(void)
{
	return bioskey(1);
}

//------------------------------------oOo-----------------------------------//
//
// Name            getch - gets character from console
//
// Usage           int getch(void);
//
// Prototype in    conio.h
//
// Description     getch is a function that reads a single character directly
//                 from the console, without echoing.
//
// Return value    getch and getche return the character read. There is no
//                 error return for these two functions.
//
//------------------------------------oOo-----------------------------------//

int getch(void)
{
    int c;
    static int extend = -1;

    /* If a previous ungetch() call has been performed, return
     * the key that it saved.
     */
    if (_cFlag)                     /* Previous ungetch? */
    	{
        _cFlag = 0;                 /* Reset the flag   */
        return (_cChar & 0xff);     /* Get the un-got char  */
		}

	 /* If a previous call returned an extended code, return
     * the scan code for the key.
     */
    if (extend != -1)
	 	{
		c = extend;
		extend = -1;                /* Reset the flag */
		return c;                   /* Return the extended scancode */
		}

	int bk = bioskey(0);

	c = bk & 0x00FF;

	if (c == 0)
		{
		extend = (bk >> 8) & 0x00FF;
		}

	return c;
}

//------------------------------------oOo-----------------------------------//
//
// Name            ungetch - ungets character to console
//
// Usage           int ungetch(int c);
//
// Prototype in    conio.h
//
// Description     ungetch pushes the character c back to the console,
// 					 causing c to be the next character read. The ungetch
// 					 function fails if it is called more than once before
// 					 the next read.
//
// Return value    ungetch returns the character c if it is successful.
// 					 return value of EOF indicates an error.
//
//------------------------------------oOo-----------------------------------//

int ungetch(int c)
{
	 if (_cFlag)
		  return(EOF);

	 _cFlag = 1;
	 return ((_cChar = c) & 0xff);
}

//------------------------------------oOo-----------------------------------//

#endif // __DPMI32__

//------------------------------------oOo-----------------------------------//
