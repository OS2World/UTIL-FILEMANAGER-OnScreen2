/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ctype.h>
#include <dir.h>

#pragma warn -use
static char Win32rcsID[]="$Id: dir.cpp 1.4 1995/05/17 19:57:39 jallen Exp jallen $";
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
	int   drive=' ';
	char	buf[256];

	if (GetCurrentDirectory(sizeof(buf), buf) > 0)
		{
		drive = buf[0];
		}

	return drive;
}

/**
 * 
 */
const CString queryCurrentDir(char drive)
{
	char  tmp[256];
	char  oldDir[256];
	char	disk[3];

	if (drive == 0)
		drive = queryCurrentDisk();
	else
		drive = toupper(drive);

	strcpy(disk, "?:");
	disk[0] = drive;
	*tmp = '\0';

	if (GetCurrentDirectory(sizeof(oldDir), oldDir) > 0)
		{
		SetCurrentDirectory(disk);

		if (GetCurrentDirectory(sizeof(tmp), tmp) == 0)
			{
			SetCurrentDirectory(oldDir);
			return "";
			}

		SetCurrentDirectory(oldDir);

		if (toupper(tmp[0]) != toupper(disk[0]))
			return "";
		}
	else
		{
		return "";
		}

	return nativePathName(tmp);
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
const CString Win32FullDir(const CString& name)
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
		if (SetCurrentDirectory((CPCHAR)name))
    		{
            newDir = queryCurrentDir(drive);
            SetCurrentDirectory((CPCHAR)oldDir);
            }
        else
            {
            if (GetFileAttributes((CPCHAR)name) & FILE_ATTRIBUTE_DIRECTORY)
                {
                newDir = name;
                }
            }

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

	this->dirName = Win32FullDir(this->dirName);
}

/**
 * 
 */
int Directory::fill(CPCHAR fileSpec)
{
	MStackCall(__FILE__,__LINE__);
	if (dirName != "")
		{
		HANDLE	hdir;
		WIN32_FIND_DATA	Win32FindData;
		FTIME		ftime;
		FDATE		fdate;
		Array<RegExp>	r;

	#if defined(__DPMI32__)
		const CString wildcard = "*.*";
	#elif defined(__WIN32__)
		const CString wildcard = "*";
	#endif

		FileExp2RegExpArray(fileSpec, r);

		if ((hdir = FindFirstFile((const char*)mergePath(dirName, wildcard), &Win32FindData)) != INVALID_HANDLE_VALUE)
			{
			entry.SetDynamic(TRUE);

			// convert Win32 file time to DOS date & time format.
			FileTimeToDosDateTime(&Win32FindData.ftLastWriteTime, (WORD*)&fdate, (WORD*)&ftime);

			// ensure both . & .. entries in under Win32

			if (strcmp(Win32FindData.cFileName, "..") == 0)
				{
				if (match(".", r))
					{
					if (match(".", r))
						entry[entry.Size()] = Dirent(this, ".", fdate, ftime,
																0,
																FILE_READONLY | FILE_DIRECTORY);
					}
				}
			else
			if (strcmp(Win32FindData.cFileName, ".") != 0)
				{
				if (match(".", r))
					entry[entry.Size()] = Dirent(this, ".", fdate, ftime,
															0,
															FILE_READONLY | FILE_DIRECTORY);
				if (match("..", r))
					entry[entry.Size()] = Dirent(this, "..", fdate, ftime,
															0,
															FILE_READONLY | FILE_DIRECTORY);
				}

			do
				{
				if (match(Win32FindData.cFileName, r))
					{
					// TODO: enable dealing with 64bit file sizes
					Dirent aDirent(this, Win32FindData.cFileName, fdate, ftime,
												Win32FindData.nFileSizeLow, Win32FindData.dwFileAttributes);
					int size = entry.Size();

					entry[size] = aDirent;
					}
				}
			while (FindNextFile(hdir, &Win32FindData));

			FindClose(hdir);
			entry.SetDynamic(FALSE);
			}
		}

	return entry.Size();
}
