/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(TRACER_HPP)
#define TRACER_HPP

#include <sysdep.hpp>#include <types.hpp>#if defined(DEBUG)	#define MStackCall(file,line)	callStack CallStack(file,line)	#define MTrace(arg) tracer::fileName = __FILE__; \							 tracer::lineNo = __LINE__; \							 tracer::log arg	#define MTraceIf(condition, arg) if (condition) \														{ \														tracer::fileName = __FILE__; \														tracer::lineNo = __LINE__; \														tracer::log arg ;\														}#else	#define MStackCall(file,line)	#define MTrace(arg)	#define MTraceIf(condition, arg)#endifclass EXP_CLASS tracer{public:	tracer();	~tracer();	static void traceOn();	static void traceOff();	static void log(const char* ...);	static char* fileName;	static int   lineNo;private:	static int 		indent;	static BOOL    tracingIsOn;};class EXP_CLASS callStack{public:	callStack(CPCHAR fileName, const long lineNo);	~callStack();	static void dump();private:	static callStack* root;	static int 			indent;	PCHAR   		name;	callStack* 	next;};#endif
