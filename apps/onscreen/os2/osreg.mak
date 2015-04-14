#=============================================================
#
#	OSREG.MAK - Makefile for project E:\cpp\apps\onscreen\os2\osreg.prj
#		Created on 12/27/94 at 13:56
#
#=============================================================

.AUTODEPEND

#=============================================================
#		Translator Definitions
#=============================================================
CC = bcc +OSREG.CFG
TASM = tasm.exe
TLIB = tlib.exe
TLINK = tlink
RC = brcc.exe
RB = rc.exe
LIBPATH = \BCOS2\LIB
INCLUDEPATH = \BCOS2\INCLUDE;\cpp\include;\cpp\apps\onscreen;


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
 osreg.obj

#=============================================================
#		Explicit Rules
#=============================================================
osreg.exe: osreg.cfg $(EXE_DEPENDENCIES)
  $(TLINK) /wdef /wimt /wmsk /wdpl /Toe /ap /L$(LIBPATH) @&&|
\BCOS2\LIB\C02.OBJ+
osreg.obj
osreg,osreg
\BCOS2\LIB\C2MT.LIB+
\BCOS2\LIB\OS2.LIB

|


#=============================================================
#		Individual File Dependencies
#=============================================================
osreg.obj: osreg.cfg ..\osreg.cpp 
	$(CC) -c ..\osreg.cpp

#=============================================================
#		Compiler Configuration File
#=============================================================
osreg.cfg: osreg.mak
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
-H=E:\cpp\apps\onscreen\os2\osreg.CSM
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
| osreg.cfg


