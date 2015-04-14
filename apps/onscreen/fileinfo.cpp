/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: fileinfo.cpp 2.50 1996/03/17 22:34:12 jallen Exp jallen $";
#pragma warn +use

/**
 * 
 */
FileInfo::FileInfo(CString dirName, CString name,
						 CString fileName, long fileSize,
						 Time fileTime, Date fileDate, BOOL fromStdin):
	iName(name),
	iDirName(dirName),
	iFileName(fileName),
	iFileSize(fileSize),
	iFileTime(fileTime),
	iFileDate(fileDate),
	iFromStdin(fromStdin)
{
}

/**
 * 
 */
FileInfo::~FileInfo()
{
}
