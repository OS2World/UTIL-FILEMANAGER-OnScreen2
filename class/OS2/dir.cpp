/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ctype.h>
#include <dir.h>

#pragma warn -use
static char OS2rcsID[]="$Id: dir.cpp 1.10 1995/05/17 19:51:07 jallen Exp jallen $";
#pragma warn +use

#define BUF_SIZE	40960

/**
 * 
 */
CPCHAR pathSeparator()
{
	PCHAR	slash;

	if ((slash = getenv("PATHSEP")) == 0)
		{
		slash = "\\";
      }

   return slash;
}

/**
 * 
 */
int queryCurrentDisk()
{
	int   drive;
	ULONG driveNo, driveMap;

	if (DosQueryCurrentDisk(&driveNo, &driveMap) == 0)
		{
		drive = (driveNo-1) + 'A';
		}

	return drive;
}

/**
 * 
 */
const CString queryCurrentDir(char drive)
{
	char  tmp[256];

	if (drive == 0)
		drive = queryCurrentDisk();
	else
		drive = toupper(drive);

	ULONG size = sizeof(tmp);
	ULONG driveNo;

	driveNo = (drive - 'A')+1;

	*tmp = '\0';

	if (DosQueryCurrentDir(driveNo, (PSZ)tmp, &size) != 0)
		{
		return "";
		}

	return nativePathName(CString(drive) + ":" + pathSeparator() + tmp);
}

/**
 * 
 */
Dirent::Dirent():
lSize(0),
mtMode(0),
fileDate(0),
fileTime(0)
{
	MStackCall(__FILE__,__LINE__);
	fDate.day = 0;
	fDate.month = 0;
	fDate.year = 0;
	fTime.twosecs = 0;
	fTime.minutes = 0;
	fTime.hours = 0;
}

/**
 * 
 */
Dirent::Dirent(
	Directory* 		directory,
	const CString& fileName,
	FDATE 			fDate,
	FTIME 			fTime,
	ULONG 			lSize,
	attrib_t 		mtMode
	):
directory(directory),
fileName(fileName),
lSize(lSize),
mtMode(mtMode),
fDate(fDate),
fTime(fTime),
fileDate(0),
fileTime(0)
{
	MStackCall(__FILE__,__LINE__);
}

/**
 * 
 */
const Time& Dirent::time() const
{
	MStackCall(__FILE__,__LINE__);
	if (!fileTime)
		{
		(*(Time**)&fileTime) = new Time(	USHORT(fTime.hours),
													USHORT(fTime.minutes),
													USHORT(fTime.twosecs*2));
		}

	return *fileTime;
}

/**
 * 
 */
const Date& Dirent::date() const
{
	MStackCall(__FILE__,__LINE__);
	if (!fileDate)
		{
		(*(Date**)&fileDate) = new Date(	USHORT(fDate.day),
													USHORT(fDate.month),
													USHORT(fDate.year+1980));
		}

	return *fileDate;
}

/**
 * 
 */
const CString os2FullDir(const CString& name)
{
	char drive = ' ';

	if (name.length() > 1 && name[2] == ':')
		{
		drive = name[1];
		}
	else
		{
		drive = queryCurrentDisk();
		}

	CString   oldDir = queryCurrentDir(drive), newDir;

	if (oldDir != "")
		{
		DosSetCurrentDir((PSZ)(CPCHAR)name);
		newDir = queryCurrentDir(drive);
		DosSetCurrentDir((PSZ)(CPCHAR)oldDir);

		return nativePathName(newDir);
		}

	return "";
}

/**
 * 
 */
Directory::Directory(CPCHAR dirName):
dirName(dirName)
{
	MStackCall(__FILE__,__LINE__);

	if (!dirName)
		this->dirName = queryCurrentDir(0);

	this->dirName = os2FullDir(this->dirName);
}

/**
 * 
 */
int Directory::fill(CPCHAR fileSpec)
{
	MStackCall(__FILE__,__LINE__);

	if (dirName != "")
		{
		HDIR		hdir = HDIR_CREATE;
		ULONG		count = 1000;
		char*		buf = new char[BUF_SIZE];
		int			attrib = 	FILE_READONLY |
			 					FILE_HIDDEN |
								FILE_SYSTEM |
								FILE_DIRECTORY |
								FILE_ARCHIVED;
		Array<RegExp>	r;

		FileExp2RegExpArray(fileSpec, r);

		if (DosFindFirst((PSZ)(const char*)mergePath(dirName, "*"), &hdir, attrib, buf,
			BUF_SIZE, &count, FIL_STANDARD) == NO_ERROR)
			{
			do
				{
				MTrace(("%s, %i matching entries", (const char*)mergePath(dirName, "*"), count));
				FILEFINDBUF3*	pffb = (FILEFINDBUF3*)buf;
				entry.SetDynamic(TRUE);

				// fake . & .. entries in / dir under MS-DOS / FAT file systems

				if (strcmp(pffb->achName, ".") != 0)
					{
					if (match(".", r))
						entry[entry.Size()] = Dirent(this, ".", pffb->fdateLastWrite,
																pffb->ftimeLastWrite,
																0,
																FILE_READONLY | FILE_DIRECTORY);
					if (match("..", r))
						entry[entry.Size()] = Dirent(this, "..", pffb->fdateLastWrite,
																pffb->ftimeLastWrite,
																0,
																FILE_READONLY | FILE_DIRECTORY);			}

				do
					{
					if (match(pffb->achName, r))
						{
						Dirent aDirent(this, pffb->achName, pffb->fdateLastWrite,
											pffb->ftimeLastWrite, pffb->cbFile, pffb->attrFile);
						int size = entry.Size();

						entry[size] = aDirent;
						}

					if (pffb->oNextEntryOffset)
						pffb = (FILEFINDBUF3*)(((char*)pffb) + pffb->oNextEntryOffset);
					else
						pffb = 0;
					}
				while (pffb);
				}
			while (DosFindNext(hdir, buf, BUF_SIZE, &count) == NO_ERROR);

			DosFindClose(hdir);
			entry.SetDynamic(FALSE);
			}

		delete buf;
		}

	return entry.Size();
}
