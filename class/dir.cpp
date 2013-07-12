/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

static char rcsID[]="$Id: dir.cpp 1.7 1995/02/12 13:51:19 jallen Exp jallen $";

#if !defined(__MACINTOSH__)
#include <sys/types.h>
#endif
#include <jac.h>    // include standard John Allen Computing stuff
#include <sysdep.hpp>
#include <conditio.hpp>
#include <array.hpp>
#include <slist.hpp>
#include <dlist.hpp>
#include <cstring.hpp>
#include <date.hpp>
#include <dir.hpp>
#include <getopt.hpp>
#include <refptr.hpp>
#include <regexp.hpp>
#include <time.hpp>
#include <tracer.hpp>
#include <types.hpp>
#pragma hdrstop

#if defined(__OS2__)
#include "../class/OS2/dir.cpp"
#elif defined(__WIN32__)
#include "../class/Win32/dir.cpp"
#elif defined(__MSDOS__)
#include "../class/DOS/dir.cpp"
#endif

/**
 * 
 */
void conv2NativePathSep(char* s)
{
	char*				p = s;
	char				ch = *pathSeparator();

	while (*p)
		{
		if (*p == '\\' || *p == '/')
			*p = ch;

		p++;
		}
}

/**
 * 
 */
CString mergePath(const CString& left, const CString& right)
{
	CString  mergedString;
	CString	ch = left[left.length()];
	CPCHAR	pRight = right;

	if (*pRight == *pathSeparator())
		pRight++;

    if (ch == pathSeparator())
        {
		mergedString = left + pRight;
		}
	else
		{
		mergedString = left + pathSeparator() + pRight;
        }

	return nativePathName(mergedString);
}

/**
 * 
 */
CString expandDir(CString base, CString matchExpr)
{
	CString   expanded;
	CString   expr, tmp;

	//fprintf(stderr, "base=%s   matchExpr=%s\n", (CPCHAR)base, (CPCHAR)matchExpr);

	if (matchExpr != "")
		{
		expr  = SubCString(matchExpr, 1, matchExpr.pos(pathSeparator())-1);
		tmp   = SubCString(matchExpr, matchExpr.pos(pathSeparator())+1, matchExpr.length());
		}

	//fprintf(stderr, "base=%s   expr=%s  tmp=%s\n", (CPCHAR)base, (CPCHAR)expr, (CPCHAR)tmp);

	if (expr != "")
		{
		Directory      d(base);
		CString   match = expr;

		//fprintf(stderr, "fill dir %s with %s\n", (CPCHAR)base, (CPCHAR)match);
		d.fill(match);

		//fprintf(stderr, "%d matches\n", d.size());

		for (int i=0; i < d.size(); i++)
			{
			if (ISDIR(d[i].mode()) && d[i].name() != "." && d[i].name() != "..")
				{
				CString subBase = mergePath(base, d[i].name());

				expanded = expandDir(subBase, tmp);

				if (expanded != "")
					{
					break;
					}
				}

			//fprintf(stderr, "base=%s, size=%d\n", (CPCHAR)base, d.size());
			}
		}
	else
		{
		expanded = base;
		}

	return expanded;
}

/**
 * 
 */
BOOL changeDir(const CString& dir)
{
	CString   tmp = dir;
	CString 	drive = " :";

	if (tmp.length() > 1 && tmp[2] == ':')
		{
		drive = SubCString(tmp, 1, 2);
		tmp = SubCString(tmp, 3, tmp.length());
		}
	else
		{
		drive[1] = (char)queryCurrentDisk();
		}

	if (tmp[tmp.length()] == *pathSeparator() && tmp != pathSeparator())
		{
		tmp = SubCString(tmp, 1, tmp.length()-1);
		}

	BOOL status = (chdir(drive + tmp) == -1) ? FALSE : TRUE;

	return status;
}

/**
 * 
 */
BOOL isDir(const CString& dir)
{
	CString	tmp;

	return isDir(dir, tmp);
}

/**
 * 
 */
BOOL isDir(const CString& dir, CString& fullName)
{
	CString CDir;
	char	drive=0;
	BOOL    isdir = FALSE;

	if (dir[2] == ':')
		{
		drive = (char)toupper(dir[1]);
		}

	CDir = queryCurrentDir(drive);

	if (changeDir(dir))
		{
		isdir = TRUE;
		fullName = queryCurrentDir(drive);

		changeDir(CDir);
		}
    else
        {
        #if defined(__WIN32__)
        // handle the Windows NT situation where a file is
        // a directory but you cannot change into it.

        if (strcspn("*?", (const char*)dir) == dir.length() &&
            GetFileAttributes(dir) & FILE_ATTRIBUTE_DIRECTORY)
            {
    		isdir = TRUE;
            fullName = dir;
            }
        #endif
        }

	return isdir;
}

/**
 * 
 */
BOOL splitPath(CString path, CString& dir, CString& fileExpr)
{
   MStackCall(__FILE__,__LINE__);
	BOOL  status;

	CString drive = " :";

	dir      = CString();
	fileExpr = CString();

	if (path.length() > 1 && path[2] == ':')
		{
		drive = SubCString(path, 1, 2);
		path = SubCString(path, 3, path.length());
		}
	else
		{
		drive[1] = (char)queryCurrentDisk();
		}

	if (path.length() == 0 || path[1] != *pathSeparator())  // have we a partial path
		{
        dir = queryCurrentDir(drive[1]);
        path = mergePath(dir, path);
		}
    else
        {
        path = drive + path;
        }

	for (int i=path.length(); i > 0; i--)
		{
		if (path[i] == *pathSeparator())
            {
			dir=SubCString(path, 1, i);
			fileExpr=SubCString(path, i+1, path.length());
            break;
			}
        }

	if ((status = isDir(path, dir)) == TRUE)
        {
		fileExpr = "*";
		}
    else
        {
		status = isDir(dir, dir);

		if (!status) // the directory is not a directory
            {
			CString root = SubCString(dir, 1, 3);
			CString match = SubCString(dir, 4, dir.length());

			CString tmp = expandDir(root, match);

             if (tmp != "")
                {
                dir = tmp;
                status = TRUE;
    			}
             }

		if (fileExpr == "..")
            {
            status = FALSE;
			}

		if (fileExpr == ".")
            {
            status = FALSE;
            }
		}

   return status;
}

/**
 * 
 */
#if !defined(__GNUG__)
ostream& operator<<(ostream& os, Dirent& de)
{
	os << (const char*)de.fileName;

	return os;
}
#endif

/**
 * 
 */
Dirent::~Dirent()
{
	MStackCall(__FILE__,__LINE__);
	if (fileDate)
		{
		delete fileDate;
		fileDate = 0;
		}

	if (fileTime)
		{
		delete fileTime;
		fileTime = 0;
		}
}

/**
 * 
 */
const BOOL Dirent::isDirectory() const
{
	return (mode() & FILE_DIRECTORY) == FILE_DIRECTORY;
}

/**
 * 
 */
const BOOL Dirent::isReadOnly() const
{
	return (mode() & FILE_READONLY) == FILE_READONLY;
}

/**
 * 
 */
const CString Dirent::fullName() const
{
	MStackCall(__FILE__,__LINE__);
   CString s;

	if (directory->dirName.length() &&
		directory->dirName[directory->dirName.length()] == *pathSeparator())
		{
      s = directory->dirName + fileName;
      }
	else
		{
		s = directory->dirName + pathSeparator() + fileName;
      }

	return nativePathName(s);
}

/**
 * 
 */
const CString& Dirent::name() const
{
	MStackCall(__FILE__,__LINE__);
	return fileName;
}

/**
 * 
 */
const CString Dirent::dirName() const
{
	MStackCall(__FILE__,__LINE__);

	return nativePathName(directory->dirName);
}

/**
 * 
 */
const long Dirent::size() const
{
	MStackCall(__FILE__,__LINE__);
	return lSize;
}

/**
 * 
 */
const attrib_t Dirent::mode() const
{
	MStackCall(__FILE__,__LINE__);
	return mtMode;
}

/**
 * 
 */
int isRootDir(CPCHAR s)
{
	if (*s)
		{
		s++;

		if (*s == ':')
			{
			s++;

			if (*s == *pathSeparator() && *(s+1) == '\0')
				return 1;
			}
		}

	return 0;
}

/**
 * 
 */
int Dirent::operator< (const Dirent& d)
{
	MStackCall(__FILE__,__LINE__);
#if defined(__UNIX__)
	// case sensitive comparison
	return name() < d.name();
#else
	// case sensitive comparison
	return stricmp(name(), d.name()) < 0;
#endif
}

/**
 * 
 */
int Dirent::operator> (const Dirent& d)
{
	MStackCall(__FILE__,__LINE__);
#if defined(__UNIX__)
	// case sensitive comparison
	return name() > d.name();
#else
	// case sensitive comparison
	return stricmp(name(), d.name()) > 0;
#endif
}

/**
 * 
 */
int Dirent::operator== (const Dirent& d)
{
	MStackCall(__FILE__,__LINE__);
#if defined(__UNIX__)
	// case sensitive comparison
	return name() == d.name();
#else
	// case sensitive comparison
	return stricmp(name(), d.name()) == 0;
#endif
}

/**
 * 
 */
#if defined(__UNIX__)

#include <unistd.h>

const CString unixFullDir(const CString& name)
{
	char 	dest[256];
	char  currentDir[256];
	PCHAR	d = (PCHAR)dest;
	PCHAR	s = (PCHAR)(CPCHAR)leftTrim(rightTrim(name));

	getcwd(&currentDir[1], sizeof(currentDir)-1);
	currentDir[0]=*pathSeparator();
 
   if (chdir(name) != -1)
      {
      getcwd(&dest[1], sizeof(dest)-1);
		dest[0]=*pathSeparator();
   	chdir(currentDir);

		if (strcmp(dest, pathSeparator()) == 0)
         dest[0] = 0;

		return nativePathName(dest);
      }

	return 0;
}
#endif

/**
 * 
 */
Directory::~Directory()
{
	MStackCall(__FILE__,__LINE__);
}

/**
 * 
 */
bool Directory::match(CString filename, Array<RegExp>& re)
{
	for (int i=0; i < re.Size(); i++)
		{
		if (re[i] == filename)
			return true;
		}

	return false;
}

/**
 * 
 */
void Directory::sort(direntCompareFunc cmp)
{
	MStackCall(__FILE__,__LINE__);
	entry.sort(cmp);
}

/**
 * 
 */
const Dirent& Directory::operator[] (int index)
{
	MStackCall(__FILE__,__LINE__);
	return entry[index];
}

/**
 * 
 */
int Directory::size() const
{
	MStackCall(__FILE__,__LINE__);
	return entry.Size();
}

/**
 * 
 */
void FileExp2RegExpArray(const CString fileSpec, Array<RegExp>& re)
{
	char* tok = strtok((char*)(const char*)fileSpec, ";");

	re.Erase();
	re.SetDynamic(true);

	while (tok)
		{
		re[re.Size()] = FileExp2RegExp(tok);
		tok = strtok(NULL, ";");
		}

	re.SetDynamic(false);
}

/**
 * 
 */
RegExp FileExp2RegExp(const CString& fileSpec)
{
	MStackCall(__FILE__,__LINE__);
	CString   res = "^";
	const char 		*sp = fileSpec;
	char     		lastchar='\0';

   while (*sp)
	  {
	  switch (*sp)
		 {
		 case '*':
			{
				res = res + "[^\\/:]*";
            }
         break;

			case '+':
            {
            if (lastchar == ']')
               {
               if (isdigit(*(sp+1)))
                  {
                  int min=1, max=1;

                  sp++;

                  max = atoi(sp); 
                  while (*sp && isdigit(*sp))
                     sp++;

                  if (*sp && *sp == '-')
                     {
                     sp++;

                     min = atoi(sp);

                     while (*sp && isdigit(*sp))
                     	sp++;
                     }

                  sp--;
                  
				  res = res + sformat("\\{%d,%d\\}", min, max);
                  }
               else
                  {
                  res = res + "\\{1\\}";
                  }
               }
            else
					{
					res = res + "\\+";
               }
            }
         break;

         case '?':
            {
            res = res + ".";
            }
         break;

			case '.':
         case '$':
            {
            res = res + CString("\\") + CString(*sp);
            }
         break;

         default:
				{
				res = res + CString((char)(*sp));
				}
			}

      lastchar = *sp++;
	  }

   res = res + "$";

#if defined(TEST)
	fprintf(stderr, "%s\n", (CPCHAR)res);
#endif

#if defined(__OS2__) || (__MSDOS__) || defined(__WIN32__)
	return RegExp(res, TRUE);
#else
	return RegExp(res);
#endif
}

/**
 * 
 */
const CString unixPathName(const CString& s)
{
	CString	tmp = copyOf(s);

	for (int i=1; i <= tmp.length(); i++)
		{
		if (tmp[i] == '\\')
			tmp[i] = '/';
		}

	return tmp;
}

/**
 * 
 */
const CString nativePathName(const CString& s)
{
	CString	tmp = copyOf(s);
	char*				p = (char*)(const char*)tmp;
	char				ch = *pathSeparator();

	while (*p)
		{
		if (*p == '\\' || *p == '/')
			*p = ch;

		p++;
		}

	return tmp;
}

/**
 * 
 */
const CString dosPathName(const CString& s)
{
	CString	tmp = copyOf(s);

	for (int i=1; i <= tmp.length(); i++)
		{
		if (tmp[i] == '/')
			tmp[i] = '\\';
		}

	return tmp;
}

/**
 * 
 */
FileTreeWalker::FileTreeWalker(CPCHAR dirName):
Directory(dirName)
{
	MStackCall(__FILE__,__LINE__);
}

/**
 * 
 */
FileTreeWalker::~FileTreeWalker()
{
	MStackCall(__FILE__,__LINE__);
}

/**
 * 
 */
const CString FileTreeWalker::queryBaseDir() const
{
	return baseDir;
}

/**
 * 
 */
BOOL FileTreeWalker::walk(CPCHAR fileSpec, unsigned depth)
{
	MStackCall(__FILE__,__LINE__);

	CString file;
	CString path = nativePathName(fileSpec);

	if (splitPath(path, dirName, file))
		{
		Array<RegExp>	r;
		FileExp2RegExpArray(file, r);

		baseDir = dirName;

		return doWalk(dirName, r, depth);
		}
    else
        {
        cerr << "Failed to splitpath " << path << endl;
        }

	return FALSE;
}

/**
 * 
 */
BOOL FileTreeWalker::found(const Dirent& /*dirent*/)
{
	MStackCall(__FILE__,__LINE__);
	return TRUE;
}

/**
 * 
 */
void FileTreeWalker::searching(const CString& /*dirName*/) const
{
	MStackCall(__FILE__,__LINE__);
}

/**
 * 
 */
BOOL FileTreeWalker::dirFinished(const CString& /*dir*/) const
{
	MStackCall(__FILE__,__LINE__);
	return TRUE;
}

/**
 * 
 */
BOOL FileTreeWalker::doWalk(const CString _dirName,
										Array<RegExp>& r, unsigned& depth)
{
	MStackCall(__FILE__,__LINE__);
	CString oldDir = dirName;
	BOOL     		Ok = TRUE;
	Directory		d(_dirName);

	dirName = _dirName;

	searching(dirName);
	d.fill("*");

	for (int i=0; i < d.size() && Ok; i++)
		{
		const Dirent&	aDirent = d[i];

		if (match(aDirent.name(), r))
			{
			Ok = found(aDirent);
			}

		if (Ok && ISDIR(aDirent.mode()) && aDirent.name() != "." &&
			aDirent.name() != "..")
			{
			depth--;

			if (depth)
				{
				Ok = doWalk(mergePath(dirName, aDirent.name()), r, depth);
				}

			depth++;
			}
		}

	dirFinished(dirName);

	dirName = oldDir;

	return Ok;
}
