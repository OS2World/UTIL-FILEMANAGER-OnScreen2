/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#pragma warn -use
static char rcsID[]="$Id: TRACER.CPP 1.6 1995/08/16 09:05:44 jallen Exp jallen $";
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
#include <stdarg.h>
#include <alloc.h>

int		tracer::indent = 0;
BOOL	tracer::tracingIsOn = FALSE;
int		EXP_DATA tracer::lineNo = 0;
char*	EXP_DATA tracer::fileName = 0;

callStack* 	callStack::root = 0;

/**
 * 
 */
callStack::callStack(CPCHAR fileName, const long lineNo)
{
	if (heapcheck() != _HEAPOK)
		{
		fprintf(stderr, "Heap check failed on entry\n");
		dump();
		exit(3);
		}

	this->next = root;

	root = this;

	char tmp[128];

	sprintf(tmp, "%s @ %ld", fileName, lineNo);

	name = strdup(tmp);
}

/**
 * 
 */
callStack::~callStack()
{
	if (heapcheck() != _HEAPOK)
		{
		fprintf(stderr, "Heap check failed on exit\n");
		dump();
		exit(3);
		}

	if (root)
		{
		root = root->next;
		}

	free(name);
}

/**
 * 
 */
void callStack::dump()
{
#if defined(__OS2__) || defined(__WIN32__)
	char* dumpFile = "Call Stack";
#elif defined(__MSDOS__)
	char* dumpFile = "CallStck";
#else
#error Unknown operating system
#endif

	if (root)
		{
		FILE*	f = fopen(dumpFile, "wt");

		callStack* tmp = root;

		fprintf(f, "*** Call Stack Dump ***\n");

		while (tmp)
			{
			fprintf(f, "%s\n", tmp->name);
			tmp = tmp->next;
			}

		fclose(f);
		}
}

/**
 * 
 */
tracer::tracer()
{
   indent++;
}

/**
 * 
 */
tracer::~tracer()
{
   indent--;
}

/**
 * 
 */
void tracer::traceOn()
{
	tracingIsOn = TRUE;
}

/**
 * 
 */
void tracer::traceOff()
{
	tracingIsOn = FALSE;
}

/**
 * 
 */
void tracer::log(const char* format ...)
{
	static FILE* logFile;
	static int entryCount;

	entryCount++;

	if (tracingIsOn)
		{
		#if defined(DEBUG)		
		// handle the fact that _MemCheck() calls trace functions
		if (entryCount == 1)
			{
			_MemCheck();
			}
		#endif

		if (!logFile)
			{
			logFile = fopen("trace.log", "wt");
			time_t	t;

			time(&t);

			fprintf(logFile, "Started %s", ctime(&t));
			}

		for (int i=0; i < indent; i++)
			fprintf(logFile, "\t");

#if defined(__GNUG__)
		if (strrchr(fileName, '/'))
			fprintf(logFile, "%14s %4i: ", strrchr(fileName, '/')+1, lineNo);
		else
			fprintf(logFile, "%14s %4i: ", fileName, lineNo);
#else
		if (strrchr(fileName, '\\'))
			fprintf(logFile, "%14s %4i: ", strrchr(fileName, '\\')+1, lineNo);
		else
			fprintf(logFile, "%14s %4i: ", fileName, lineNo);
#endif

		va_list	ap;

		va_start(ap, format);

		vfprintf(logFile, format, ap);
		fprintf(logFile, "\n");
		fflush(logFile);
		// logFile = fopen("trace.log", "at");

		va_end(ap);
		}

	entryCount--;
}
