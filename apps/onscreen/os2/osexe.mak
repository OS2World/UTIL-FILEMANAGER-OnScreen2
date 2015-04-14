#=============================================================
#
#	OSEXE.MAK - Makefile for project E:\cpp\apps\onscreen\os2\osexe.prj
#		Created on 06/27/95 at 19:34
#
#=============================================================

.AUTODEPEND

#=============================================================
#		Translator Definitions
#=============================================================
CC = bcc +OSEXE.CFG
TASM = tasm.exe
TLIB = tlib.exe
TLINK = tlink
RC = brcc.exe
RB = rc.exe
LIBPATH = \BCOS2\LIB
INCLUDEPATH = \BCOS2\INCLUDE;..\..\..\include;..\;


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
 os.def \
 getopt.obj \
 strsrch.obj \
 strstri.obj \
 strrchri.obj \
 strchri.obj \
 rcsver.obj \
 tracer.obj \
 time.obj \
 regexp.obj \
 dir.obj \
 date.obj \
 cstring.obj \
 conditio.obj \
 picklist.obj \
 pickbuf.obj \
 osview.obj \
 ostime.obj \
 osstyle.obj \
 osser.obj \
 osmisc.obj \
 osvideo.obj \
 osedit.obj \
 fileman.obj \
 fileinfo.obj \
 globals.obj \
 os.obj

#=============================================================
#		Explicit Rules
#=============================================================
osexe.exe: osexe.cfg $(LINK_INCLUDE) $(LINK_EXCLUDE)
  $(TLINK) /wdef /wimt /wmsk /wdpl /Toe /ap /L$(LIBPATH) @&&|
\BCOS2\LIB\C02.OBJ+
getopt.obj+
strsrch.obj+
strstri.obj+
strrchri.obj+
strchri.obj+
rcsver.obj+
tracer.obj+
time.obj+
regexp.obj+
dir.obj+
date.obj+
cstring.obj+
conditio.obj+
picklist.obj+
pickbuf.obj+
osview.obj+
ostime.obj+
osstyle.obj+
osser.obj+
osmisc.obj+
osvideo.obj+
osedit.obj+
fileman.obj+
fileinfo.obj+
globals.obj+
os.obj
osexe,osexe
\BCOS2\LIB\C2MT.LIB+
\BCOS2\LIB\OS2.LIB
os.def
|
  rc.exe os.res osexe.exe

#=============================================================
#		Individual File Dependencies
#=============================================================
os.res: osexe.cfg OS.RC 
	brcc.exe -R -I$(INCLUDEPATH) -FO os.res OS.RC

getopt.obj: osexe.cfg ..\..\..\func\getopt.cpp 
	$(CC) -c ..\..\..\func\getopt.cpp

strsrch.obj: osexe.cfg ..\..\..\func\strsrch.cpp 
	$(CC) -c ..\..\..\func\strsrch.cpp

strstri.obj: osexe.cfg ..\..\..\func\strstri.cpp 
	$(CC) -c ..\..\..\func\strstri.cpp

strrchri.obj: osexe.cfg ..\..\..\func\strrchri.cpp 
	$(CC) -c ..\..\..\func\strrchri.cpp

strchri.obj: osexe.cfg ..\..\..\func\strchri.cpp
	$(CC) -c ..\..\..\func\strchri.cpp

rcsver.obj: osexe.cfg ..\..\..\func\rcsver.cpp 
	$(CC) -c ..\..\..\func\rcsver.cpp

tracer.obj: osexe.cfg ..\..\..\class\tracer.cpp 
	$(CC) -c ..\..\..\class\tracer.cpp

time.obj: osexe.cfg ..\..\..\class\time.cpp 
	$(CC) -c ..\..\..\class\time.cpp

regexp.obj: osexe.cfg ..\..\..\class\regexp.cpp 
	$(CC) -c ..\..\..\class\regexp.cpp

dir.obj: osexe.cfg ..\..\..\class\dir.cpp 
	$(CC) -c ..\..\..\class\dir.cpp

date.obj: osexe.cfg ..\..\..\class\date.cpp 
	$(CC) -c ..\..\..\class\date.cpp

cstring.obj: osexe.cfg ..\..\..\class\cstring.cpp 
	$(CC) -c ..\..\..\class\cstring.cpp

conditio.obj: osexe.cfg ..\..\..\class\conditio.cpp 
	$(CC) -c ..\..\..\class\conditio.cpp

picklist.obj: osexe.cfg ..\picklist.cpp
	$(CC) -c ..\picklist.cpp

pickbuf.obj: osexe.cfg ..\pickbuf.cpp 
	$(CC) -c ..\pickbuf.cpp

osview.obj: osexe.cfg ..\osview.cpp 
	$(CC) -c ..\osview.cpp

ostime.obj: osexe.cfg ..\ostime.cpp 
	$(CC) -c ..\ostime.cpp

osstyle.obj: osexe.cfg ..\osstyle.cpp 
	$(CC) -c ..\osstyle.cpp

osser.obj: osexe.cfg ..\osser.cpp 
	$(CC) -c ..\osser.cpp

osmisc.obj: osexe.cfg ..\osmisc.cpp 
	$(CC) -c ..\osmisc.cpp

osvideo.obj: osexe.cfg ..\osvideo.cpp 
	$(CC) -c ..\osvideo.cpp

osedit.obj: osexe.cfg ..\osedit.cpp 
	$(CC) -c ..\osedit.cpp

fileman.obj: osexe.cfg ..\fileman.cpp
	$(CC) -c ..\fileman.cpp

fileinfo.obj: osexe.cfg ..\fileinfo.cpp 
	$(CC) -c ..\fileinfo.cpp

globals.obj: osexe.cfg ..\globals.cpp 
	$(CC) -c ..\globals.cpp

os.obj: osexe.cfg ..\os.cpp 
	$(CC) -c ..\os.cpp

#=============================================================
#		Compiler Configuration File
#=============================================================
osexe.cfg: osexe.mak
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
-H=E:\cpp\apps\onscreen\os2\osexe.CSM
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
| osexe.cfg


