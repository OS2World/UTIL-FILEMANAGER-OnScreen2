/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(TYPES_HPP)
#define TYPES_HPP

#include <sysdep.hpp>

// use instead of NULL in C++
#if !defined(NIL)
   #define NIL 	(0)
#endif

#ifndef FALSE
   #define FALSE	(0)
   #define TRUE	(!FALSE)
#endif

#if !defined(SET)
   #define SET		(1)
   #define CLEAR	(0)
#endif

#if !defined(__OS2__)
   typedef unsigned short USHORT;
#endif

#if defined(__OS2__) && !defined(WORD) && !defined(_ZAPPGENERAL_INCLUDED)
   typedef unsigned short  WORD;
#endif

#if defined(__OS2__)
   // TODO: is OS/2 attrib ULONG or USHORT
   typedef ULONG				attrib_t;
#elif defined(__WIN32__)
   typedef char*           PCHAR;
   typedef unsigned long	ULONG;
   typedef unsigned int		UINT;
   typedef DWORD				attrib_t;

	struct FTIME
	{
		unsigned twosecs:5;
		unsigned minutes:6;
		unsigned hours:5;
	};

	struct FDATE
	{
		unsigned day:5;
		unsigned month:4;
		unsigned year:7;
	};

#elif defined(__MSDOS__)
   typedef char*           PCHAR;
   typedef unsigned long	ULONG;
   typedef unsigned int		UINT;
   typedef USHORT				attrib_t;

	struct FTIME
	{
		unsigned twosecs:5;
		unsigned minutes:6;
		unsigned hours:5;
	};

	struct FDATE
	{
		unsigned day:5;
		unsigned month:4;
		unsigned year:7;
	};

#endif

#if defined(__MSDOS__) && !defined(__WINDOWS__)
   typedef int					BOOL;
#endif

#if !defined(__OS2__)
   typedef char*			PSZ;
#endif

#if defined(__MSDOS__) && !(defined(__WINDOWS__) || defined(__OS2__))
   typedef short				SHORT;
   typedef unsigned short	USHORT;
   typedef unsigned short	WORD;
   typedef unsigned long	DWORD;
   typedef unsigned long	ULONG;
   typedef long				LONG;
   typedef unsigned char	BYTE;
#endif

#if defined(__MACINTOSH__)
   typedef long						LONG;
   typedef unsigned short			WORD;
   typedef int							BOOL;
   typedef unsigned int		      UINT;

   char* 	strupr(char* s);
   int 		strcmpi(const char* s1, const char* s2);
   int 		stricmp(const char* s1, const char* s2);
   char* 	strdup(const char* s);
#endif

#if defined(__WIN32__) && !defined(__WINDOWS__)
   typedef short				      SHORT;
   typedef unsigned short	      USHORT;
   typedef unsigned short	      WORD;
   typedef long				      LONG;
   typedef unsigned char	      BYTE;
   typedef long						LONG;
   typedef int							BOOL;
   typedef unsigned int		      UINT;
#endif

typedef unsigned char   		UCHAR;
typedef char*           		PCHAR;
typedef UCHAR*   					PUCHAR;
typedef const char*     		CPCHAR;
typedef const unsigned char	CPUCHAR;
typedef LONG	         		FPOS;
typedef WORD						FLAG;

typedef const unsigned char*	CPUSZ;
typedef const char*				CPSZ;
typedef unsigned char*  		PUSZ;

#if defined(__BCPLUSPLUS__) && defined(__WIN32__)
	#if (__BORLANDC__ >= 0x460)
	#else
		#include <bool.h>
		// Placement new operator not provided by Borland.
		inline void* operator new(size_t size, void* p) { return p; }
	#endif

	#include <dos.h>
#else
	#if !defined(BOOL_H)
		#define BOOL_H
		typedef int	bool;
		const int false = 0;
		const int true = 1;
		#endif
	#endif
#endif

