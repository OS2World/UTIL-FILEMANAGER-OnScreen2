/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#pragma warn -use
static char DOSrcsID[]="$Id: dir.cpp 1.3 1995/05/17 19:57:37 jallen Exp jallen $";
#pragma warn +use

#include <ctype.h>
#include <dir.h>

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

	drive = getdisk() + 'A';

	return drive;
}

/**
 * 
 */
#if defined(__GNUG__) && defined(__GO32__)
#include <go32.h>
int getcurdir(int drive, char* dest)
{
	union REGS		r;
	struct SREGS	s;

	memset(&s, 0, sizeof(s));
	memset(&r, 0, sizeof(r));

	s.ds   = _go32_my_ds();
	r.x.si = (int)dest;
	r.h.dl = drive;
	r.h.ah = 0x47;

	intdosx(&r, &r, &s);

	if (r.x.cflag)
		{
		return r.x.ax;
		}

	strupr(dest);
	conv2NativePathSep(dest);

	return 0;
}
#endif

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

	unsigned driveNo;

	driveNo = (drive - 'A')+1;

	if (getcurdir(driveNo, tmp) != 0)
		{
		return "";
		}

	Trace(("queryCurrentDir(): %s",
		         (const char*)(nativePathName(CString(drive) + ":" + pathSeparator() + tmp))));

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
	StackCall(__FILE__,__LINE__);
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
	FDATE				fDate,
	FTIME				fTime,
	ULONG				lSize,
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
	StackCall(__FILE__,__LINE__);
}

/**
 * 
 */
const Time& Dirent::time() const
{
	StackCall(__FILE__,__LINE__);
	if (!fileTime)
		{
		Trace(("Dirent time = %i:%i:%i", fTime.hours, fTime.minutes, fTime.twosecs*2));
		(*(Time**)&fileTime) = new Time(	fTime.hours,
													fTime.minutes,
													fTime.twosecs*2);
		}

	return *fileTime;
}

/**
 * 
 */
const Date& Dirent::date() const
{
	StackCall(__FILE__,__LINE__);
	if (!fileDate)
		{
		Trace(("Dirent date = %i/%i/%i", fDate.day, fDate.month, fDate.year));
		(*(Date**)&fileDate) = new Date(	fDate.day,
													fDate.month, fDate.year + 1980);
		}

	return *fileDate;
}

/**
 * 
 */
const CString dosFullDir(const CString& name)
{
	CString		result;

#if (defined(__GNUG__) && defined(__GO32__)) || defined(__DPMI32__)
	char drive = ' ';

	if (name.length() > 1 && name[2] == ':')
		{
		drive = name[1];
		}
	else
		{
		drive = queryCurrentDisk();
		}

	CString   oldDir = queryCurrentDir(drive);

	if (oldDir != "")
		{
		Trace(("dosFullDir(): chdir(%s)", (CPCHAR)name));
		chdir((CPCHAR)name);
		result = queryCurrentDir(drive);
		chdir((CPCHAR)oldDir);
		}
#else
	char 			dest[128];
	CString 		tmp = leftTrim(rightTrim(name));
	char far*	d = (char far*)dest;
	char far*	s = (char far*)(CPCHAR)tmp;

	asm {
	push	di
	push	si
	push	ds
	push	es
	les	di, d
	lds	si, s
	mov	ah, 0x60
	int	0x21
	pop	es
	pop	ds
	pop	si
	pop	di
	jc		error
	}

	result = dest;
error:
#endif

	result = nativePathName(strupr((PCHAR)(CPCHAR)result));

	Trace(("dosFullDir(): %s", (const char*)result));

	return result;
}

/**
 * 
 */
Directory::Directory(CPCHAR dirName):
dirName(dirName)
{
	StackCall(__FILE__,__LINE__);

	if (!dirName)
		this->dirName = queryCurrentDir(0);

	this->dirName = dosPathName(this->dirName);
	this->dirName = dosFullDir(this->dirName);
}

/**
 * 
 */
int Directory::fill(CPCHAR fileSpec)
{
	StackCall(__FILE__,__LINE__);

	if (dirName != "")
		{
		struct ffblk 	ff;
		int				attrib = FA_RDONLY |
										FA_HIDDEN |
										FA_SYSTEM |
										FA_DIREC |
										FA_ARCH;
		Array<RegExp>	r;

		FileExp2RegExpArray(fileSpec, r);

		if (findfirst(mergePath(dirName, "*.*"), &ff, attrib) == 0)
			{
			entry.SetDynamic(TRUE);

			// fake . & .. entries in / dir under MS-DOS

			if (strcmp(ff.ff_name, ".") != 0)
				{
				if (r == ".")
					entry[entry.Size()] = Dirent(this, ".", (*(FDATE*)&ff.ff_fdate), (*(FTIME*)&ff.ff_ftime),
																				0, FA_RDONLY | FA_DIREC);

				if (r == "..")
					entry[entry.Size()] = Dirent(this, "..", (*(FDATE*)&ff.ff_fdate), (*(FTIME*)&ff.ff_ftime),
																				0, FA_RDONLY | FA_DIREC);
				}

			do
				{
				if (r == ff.ff_name)
					{
					Dirent aDirent(this, ff.ff_name, (*(FDATE*)&ff.ff_fdate), (*(FTIME*)&ff.ff_ftime), ff.ff_fsize, ff.ff_attrib);
					int size = entry.Size();

					entry[size] = aDirent;
					}
				}
			while (findnext(&ff) == 0);

			entry.SetDynamic(FALSE);
			}
		else
			{
			Trace(("findfirst(%s): failed", (const char*)dirName));
			}
		}

	return entry.Size();
}
