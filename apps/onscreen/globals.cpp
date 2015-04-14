/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: globals.cpp 2.50 1996/03/17 22:34:14 jallen Exp jallen $";
#pragma warn +use

CPCHAR 				gOnScreenCopyright = "Copyright (c) 1995-2006 John J. Allen, Release under the GPL v2 or later.";
PCHAR				gOnScreenRevision = "$Revision: 2.50 $";
Style*				gDefaultStyle;
Style*				gCurrStyle;
int					gCurrFile;
FileManager*		gFileManager;
CString				gStartupDir;

struct SetupInfo setupInfo =
{
	FALSE,			// soundOn
	FALSE,			// regexpSearch
	BLACK,			// statusTextForeColor
	LIGHTGRAY, 		// statusTextBackColor
	FALSE,			// caseSensitive
	FALSE,			// keepFilesLoaded
	BLACK,			// diskTextForeColor
	LIGHTGRAY, 		// diskTextBackColor
	WHITE,			// curDiskTextForeColor
	LIGHTGRAY,		// curDiskTextBackColor
	LIGHTGRAY,		// fileTextForeColor
	BLACK,			// fileTextBackColor
	BLACK,			// curFileTextForeColor
	LIGHTGRAY		// curFileTextBackColor
};
