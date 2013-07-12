/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#pragma warn -use
static char rcsID[]="$Id: conditio.cpp 1.9 1995/08/16 09:05:39 jallen Exp $";
#pragma warn +use

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

#if defined(__BORLANDC__)
#include <alloc.h>
#endif

/**
 * 
 */
void EXP_FUNC _MemCheck()
{
	static int entryCount;

	// prevent recursive calling
	if (++entryCount > 1)
		{
		--entryCount;

		return;
		}

#if defined(__OS2__)
	switch (heapcheck())
		{
		case _HEAPOK:
			{
			#if defined(DEBUG_HEAP_DUMP)
			struct _heapinfo hi;
			hi._pentry = NULL;
			long	heapBlocks = 0;
			long	memoryAllocatedTotal = 0;
			long	memoryAllocatedInuse = 0;
			long	memoryAllocatedUnused = 0;

			Trace(("*** Begin heap dump\n"));
			Trace(("Addr        Size   Status\n"));
			Trace(("----        ----   ------\n"));

			while (_heapwalk(&hi) == _HEAPOK)
				{
				heapBlocks++;

				Trace(("%p %7u    %s\n",
					hi._pentry,
					hi._size,
					hi._useflag ? "used" : "free"));

				memoryAllocatedTotal += hi._size;

				if (hi._useflag)
					memoryAllocatedInuse += hi._size;
				else
					memoryAllocatedUnused += hi._size;
				}

			Trace(("Memory allocated in use %i\n", memoryAllocatedInuse));
			Trace(("Memory allocated unused %i\n", memoryAllocatedUnused));
			Trace(("Total memory allocated  %i\n", memoryAllocatedTotal));
			Trace(("*** End heap dump\n"));
			#endif
			}
		break;

		case _HEAPEMPTY:
			{
			MTrace(("!!failure: Heap empty"));
			WinMessageBox(
				HWND_DESKTOP,
				HWND_DESKTOP,
				(PSZ)"Heap empty",
				0,
				0,
				MB_ENTER | MB_ERROR
				);

			MTerminate();
			}
		break;

		case _HEAPCORRUPT:
			{
			MTrace(("!!failure: Heap corrupt"));
			WinMessageBox(
				HWND_DESKTOP,
				HWND_DESKTOP,
				(PSZ)"Heap corrupt",
				0,
				0,
				MB_ENTER | MB_ERROR
				);

			MTerminate();
			}
		break;
		}
#else
#endif
		--entryCount;
}

/**
 * 
 */
void EXP_FUNC _FailIf(BOOL Condition, PCHAR ConditionStr, PCHAR File, long line)
{
	_MemCheck();
	if (Condition)
		{
		tracer::fileName = File;
		tracer::lineNo = line;
		tracer::log("FailIf(%s) failed", ConditionStr);
		MTerminate();
		}
}

/**
 * 
 */
void EXP_FUNC _Fail(PCHAR MsgStr, PCHAR File, long line)
{
	_MemCheck();
	tracer::fileName = File;
	tracer::lineNo = line;
	tracer::log("Fail(%s)", MsgStr);
	MTerminate();
}

/**
 * 
 */
int EXP_FUNC _Ensure(BOOL Condition, PCHAR ConditionStr, PCHAR File, long line)
{
	_MemCheck();
	if (!Condition)
		{
		tracer::fileName = File;
		tracer::lineNo = line;
		tracer::log("Ensure(%s) failed", ConditionStr);
		MTerminate();
		}

	return 0;
}
