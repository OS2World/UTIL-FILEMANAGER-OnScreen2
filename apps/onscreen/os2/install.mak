#=============================================================
#
#	INSTALL.MAK - Makefile for project E:\cpp\apps\onscreen\os2\install.prj
#		Created on 03/08/96 at 23:24
#
#=============================================================

.AUTODEPEND

#=============================================================
#		Translator Definitions
#=============================================================
CC = bcc +INSTALL.CFG
TASM = tasm.exe
TLIB = tlib.exe
TLINK = tlink
RC = brcc.exe
RB = rc.exe
LIBPATH = \BCOS2\LIB
INCLUDEPATH = \BCOS2\INCLUDE;\cpp\include;..\;


#=============================================================
#		Implicit Rules
#=============================================================
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

.asm.obj:
  $(TASM) -Mx $*.asm,$*.obj

.rc.res:
  $(RC) -r $*.rc

#=============================================================
#		List Macros
#=============================================================


EXE_DEPENDENCIES =  \
 globals.obj \
 regexp.obj \
 conditio.obj \
 date.obj \
 time.obj \
 tracer.obj \
 cstring.obj \
 osstyle.obj \
 osvideo.obj \
 install.obj

#=============================================================
#		Explicit Rules
#=============================================================
install.exe: install.cfg $(EXE_DEPENDENCIES)
  $(TLINK) /wdef /wimt /wmsk /wdpl /Toe /ap /L$(LIBPATH) @&&|
\BCOS2\LIB\C02.OBJ+
globals.obj+
regexp.obj+
conditio.obj+
date.obj+
time.obj+
tracer.obj+
cstring.obj+
osstyle.obj+
osvideo.obj+
install.obj
install,install
\BCOS2\LIB\C2MT.LIB+
\BCOS2\LIB\OS2.LIB

|


#=============================================================
#		Individual File Dependencies
#=============================================================
globals.obj: install.cfg ..\globals.cpp 
	$(CC) -c ..\globals.cpp

regexp.obj: install.cfg ..\..\..\class\regexp.cpp 
	$(CC) -c ..\..\..\class\regexp.cpp

conditio.obj: install.cfg ..\..\..\class\conditio.cpp 
	$(CC) -c ..\..\..\class\conditio.cpp

date.obj: install.cfg ..\..\..\class\date.cpp
	$(CC) -c ..\..\..\class\date.cpp

time.obj: install.cfg ..\..\..\class\time.cpp 
	$(CC) -c ..\..\..\class\time.cpp

TRACER.obj: install.cfg ..\..\..\class\TRACER.CPP 
	$(CC) -c ..\..\..\class\TRACER.CPP

cstring.obj: install.cfg ..\..\..\class\cstring.cpp 
	$(CC) -c ..\..\..\class\cstring.cpp

osstyle.obj: install.cfg ..\osstyle.cpp 
	$(CC) -c ..\osstyle.cpp

osvideo.obj: install.cfg ..\osvideo.cpp 
	$(CC) -c ..\osvideo.cpp

install.obj: install.cfg ..\install.CPP 
	$(CC) -c ..\install.CPP

#=============================================================
#		Compiler Configuration File
#=============================================================
install.cfg: install.mak
  copy &&|
-RT-
-xd-
-x-
-R
-Oi
-Oz
-Ob
-Oe
-Oc
-L$(LIBPATH)
-I$(INCLUDEPATH)
-P
-H=E:\cpp\apps\onscreen\os2\install.CSM
-sm
-d
-O
-Ot
-y
-v
-w
-wnod
-wuse
-wucp
-wstv
-wsig
-wdef
-wcln
-wasm
-wamp
-wamb
-wbbf
-wpin
| install.cfg


