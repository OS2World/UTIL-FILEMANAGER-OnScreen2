/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if defined(DEBUG)
#include <stdlib.h>
#include <sysdep.hpp>
#include <conditio.hpp>
#include <memcheck.hpp>

#pragma warn -use
static char rcsID[]="$Id: memcheck.cpp 1.2 1994/10/22 14:55:48 johna Exp $";
#pragma warn +use

#define MAXALLOCS	4096

void* allocs[MAXALLOCS];

/**
 * 
 */
#if defined(__BORLANDC__) && defined(__WIN32__)
void* operator new[](size_t size)
{
	_MemCheck();
	for (int i=0; i < MAXALLOCS; i++)
		{
		if (allocs[i] == 0)
			{
			allocs[i] = malloc(size);

			return allocs[i];
			}
		}

	return 0;
}
#endif

/**
 * 
 */
void* operator new(size_t size)
{
	_MemCheck();
	for (int i=0; i < MAXALLOCS; i++)
		{
		if (allocs[i] == 0)
			{
			allocs[i] = malloc(size);

			return allocs[i];
			}
		}

	return 0;
}

/**
 * 
 */
void operator delete(void* p)
{
	for (int i=0; i < MAXALLOCS; i++)
		{
		if (allocs[i] == p)
			{
			free(p);
			allocs[i] = 0;

			_MemCheck();
			return;
			}
		}

	MFail("bad pointer on delete");
}

#endif
