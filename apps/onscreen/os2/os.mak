#=============================================================
#
#	OS.MAK - Makefile for project \cpp\apps\onscreen\os2\os.prj
#		Created on 06/27/95 at 19:34
#
#=============================================================

.AUTODEPEND

#=============================================================
#		Translator Definitions
#=============================================================
CC = bcc $(CFLAGS) +OS.CFG
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
LINK_EXCLUDE =  \
 os.res

LINK_INCLUDE =  \
 ..\..\..\os2\lib\wf7lib10.lib \
 os.def \
 fileman.obj \
 fileinfo.obj \
 picklist.obj \
 pickbuf.obj \
 osview.obj \
 ostime.obj \
 osstyle.obj \
 osser.obj \
 osvideo.obj \
 osmisc.obj \
 osedit.obj \
 globals.obj \
 os.obj

#=============================================================
#		Explicit Rules
#=============================================================
os.exe: os.cfg $(LINK_INCLUDE) $(LINK_EXCLUDE)
  $(TLINK) /wdef /wimt /wmsk /wdpl /Toe /ap /L$(LIBPATH) @&&|
\BCOS2\LIB\C02.OBJ+
fileman.obj+
fileinfo.obj+
picklist.obj+
pickbuf.obj+
osview.obj+
ostime.obj+
osstyle.obj+
osser.obj+
osvideo.obj+
osmisc.obj+
osedit.obj+
globals.obj+
os.obj
os,os
..\..\..\os2\lib\wf7lib10.lib+
\BCOS2\LIB\C2MTI.LIB+
\BCOS2\LIB\OS2.LIB
os.def
|
  rc.exe os.res os.exe

#=============================================================
#		Individual File Dependencies
#=============================================================
os.res: os.cfg OS.RC
	brcc.exe -R -I$(INCLUDEPATH) -FO os.res OS.RC

fileman.obj: os.cfg ..\fileman.cpp
	$(CC) -c ..\fileman.cpp

fileinfo.obj: os.cfg ..\fileinfo.cpp
	$(CC) -c ..\fileinfo.cpp

picklist.obj: os.cfg ..\picklist.cpp
	$(CC) -c ..\picklist.cpp

pickbuf.obj: os.cfg ..\pickbuf.cpp
	$(CC) -c ..\pickbuf.cpp

osview.obj: os.cfg ..\osview.cpp
	$(CC) -c ..\osview.cpp

ostime.obj: os.cfg ..\ostime.cpp
	$(CC) -c ..\ostime.cpp

osstyle.obj: os.cfg ..\osstyle.cpp
	$(CC) -c ..\osstyle.cpp

osser.obj: os.cfg ..\osser.cpp
	$(CC) -c ..\osser.cpp

osvideo.obj: os.cfg ..\osvideo.cpp
	$(CC) -c ..\osvideo.cpp

osmisc.obj: os.cfg ..\osmisc.cpp
	$(CC) -c ..\osmisc.cpp

osedit.obj: os.cfg ..\osedit.cpp
	$(CC) -c ..\osedit.cpp

globals.obj: os.cfg ..\globals.cpp
	$(CC) -c ..\globals.cpp

os.obj: os.cfg ..\os.cpp
	$(CC) -c ..\os.cpp

#=============================================================
#		Compiler Configuration File
#=============================================================
os.cfg: os.mak
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
-H=\cpp\apps\onscreen\os2\os.CSM
-vi
-sm
-d
-O
-Ot
-y
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
| os.cfg


