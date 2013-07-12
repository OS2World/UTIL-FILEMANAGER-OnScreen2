/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(DIR_HPP)
#define DIR_HPP

#include <jac.h>    // include standard John Allen Computing stuff
#include <sysdep.hpp>#include <types.hpp>#include <time.hpp>#include <date.hpp>#include <limits.h>#include <iostream.h>#if defined(__OS2__)	#include <dir.h>#elif defined(__WIN32__)	#include <dir.h>	#include <winbase.h>	#define FILE_NORMAL     FILE_ATTRIBUTE_NORMAL	#define FILE_READONLY	FILE_ATTRIBUTE_READONLY	#define FILE_HIDDEN     FILE_ATTRIBUTE_HIDDEN	#define FILE_SYSTEM     FILE_ATTRIBUTE_SYSTEM	#define FILE_DIRECTORY 	FILE_ATTRIBUTE_DIRECTORY	#define FILE_ARCHIVED	FILE_ATTRIBUTE_ARCHIVE	#define FERR_DISABLEHARDERR	#define FERR_ENABLEHARDERR	#define DosError(flag)#elif defined(__MSDOS__)	#include <dir.h>	#define FILE_NORMAL     FA_NORMAL	#define FILE_READONLY	FA_RDONLY	#define FILE_HIDDEN     FA_HIDDEN	#define FILE_SYSTEM     FA_SYSTEM	#define FILE_DIRECTORY 	FA_DIREC	#define FILE_ARCHIVED	FA_ARCH	#define FERR_DISABLEHARDERR	#define FERR_ENABLEHARDERR	#define DosError(flag)#endif#define FILE_SHAREABLE	0x0080#define	ISDIR(a)	((a & FILE_DIRECTORY) == FILE_DIRECTORY)class EXP_CLASS Directory;class EXP_CLASS Dirent;typedef int	(direntCompareFunc)(const Dirent* d1, const Dirent* d2);void EXP_FUNC FileExp2RegExpArray(const CString fileSpec, Array<RegExp>& re);RegExp EXP_FUNC FileExp2RegExp(const CString& fileSpec);void EXP_FUNC conv2NativePathSep(char* s);const CString EXP_FUNC nativePathName(const CString& s);const CString EXP_FUNC unixPathName(const CString& s);const CString EXP_FUNC dosPathName(const CString& s);const CString EXP_FUNC unixFullDir(const CString& s);const CString EXP_FUNC os2FullDir(const CString& s);CPCHAR			EXP_FUNC pathSeparator();const CString 	EXP_FUNC queryCurrentDir(char drive);CString 			EXP_FUNC mergePath(const CString& left, const CString& right);CString 			EXP_FUNC expandDir(CString base, CString matchExpr);int   			EXP_FUNC queryCurrentDisk();BOOL 				EXP_FUNC changeDir(const CString& dir);BOOL 				EXP_FUNC isDir(const CString& dir);
BOOL 				EXP_FUNC isDir(const CString& dir, CString& fullName);BOOL 				EXP_FUNC splitPath(CString path, CString& dir, CString& fileExpr);#if !defined(__GNUG__)ostream& operator<<(ostream& os, Dirent& de);#endif/**
 * 
 */
class EXP_CLASS Dirent{public:	Dirent();	Dirent(Directory* directory, const CString& fileName,				FDATE fDate, FTIME fTime, ULONG lSize = 0, attrib_t mtMode = 0);	virtual ~Dirent();	const CString&	name() const;	const CString	fullName() const;	const CString  dirName() const;	const long		size() const;	const attrib_t mode() const;	const Time&		time() const;	const	Date&		date() const;	const BOOL		isDirectory() const;	const BOOL		isReadOnly() const;	int operator< (const Dirent& d);	int operator> (const Dirent& d);	int operator== (const Dirent& d);private:	CString		fileName;	ULONG			lSize;	FDATE			fDate;	FTIME			fTime;	attrib_t		mtMode;	Directory*  directory;	Date*			fileDate;	Time*			fileTime;	friend ostream& operator<<(ostream& os, Dirent& de);	friend class FileManDir;};/**
 * 
 */
class EXP_CLASS Directory{public:	Directory(CPCHAR dirName = 0);	virtual ~Directory();	int 				fill(CPCHAR fileSpec = "*");	int				size() const;	const Dirent&	operator[] (int index);	void				sort(direntCompareFunc cmp = 0);protected:	bool	match(CString filename, Array<RegExp>& re);	CString			   		dirName;	SortableArray<Dirent>	entry;	friend class Dirent;};/**
 * 
 */
class EXP_CLASS FileTreeWalker: public Directory{public:	FileTreeWalker(CPCHAR dirName = 0);	virtual ~FileTreeWalker();	BOOL walk(CPCHAR fileSpec = "*", unsigned depth = UINT_MAX);	const CString queryBaseDir() const;private:	CString	baseDir;	virtual BOOL   found(const Dirent& dirent);	virtual void   searching(const CString& dirName) const;	virtual BOOL   dirFinished(const CString& dir) const;	BOOL           doWalk(const CString dirName, Array<RegExp>& r, unsigned& depth);	friend class Dirent;};

#endif
