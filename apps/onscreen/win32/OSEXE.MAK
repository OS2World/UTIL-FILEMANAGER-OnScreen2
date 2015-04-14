#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
TLINK32 = TLink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS32 =  -LC:\BC4\LIB
LLATC32_osexedexe =  -L\BCWIN\LIB; -v -Tpe -ap -c
RLATC32_osexedexe =  -w32
BLATC32_osexedexe = 
CNIEAT_osexedexe = -I\BCWIN\INCLUDE;..\..\..\INCLUDE;..\; -D_RTLDLL;
LNIEAT_osexedexe = -x
LEAT_osexedexe = $(LLATC32_osexedexe)
REAT_osexedexe = $(RLATC32_osexedexe)
BEAT_osexedexe = $(BLATC32_osexedexe)

#
# Dependency List
#
Dep_osexe = \
   osexe.exe

osexe : BccW32.cfg $(Dep_osexe)
  echo MakeNode 

Dep_osexedexe = \
   conditio.obj\
   tracer.obj\
   getopt.obj\
   rcsver.obj\
   strchri.obj\
   strrchri.obj\
   strsrch.obj\
   strstri.obj\
   cstring.obj\
   date.obj\
   dir.obj\
   regexp.obj\
   time.obj\
   globals.obj\
   osvideo.obj\
   osgetch.obj\
   osser.obj\
   fileman.obj\
   osstyle.obj\
   osedit.obj\
   os.obj\
   osmisc.obj\
   ostime.obj\
   osview.obj\
   pickbuf.obj\
   picklist.obj\
   fileinfo.obj

osexe.exe : $(Dep_osexedexe)
  $(TLINK32) @&&|
  $(IDE_LFLAGS32) $(LEAT_osexedexe) $(LNIEAT_osexedexe) +
\BCWIN\LIB\c0x32.obj+
conditio.obj+
tracer.obj+
getopt.obj+
rcsver.obj+
strchri.obj+
strrchri.obj+
strsrch.obj+
strstri.obj+
cstring.obj+
date.obj+
dir.obj+
regexp.obj+
time.obj+
globals.obj+
osvideo.obj+
osgetch.obj+
osser.obj+
fileman.obj+
osstyle.obj+
osedit.obj+
os.obj+
osmisc.obj+
ostime.obj+
osview.obj+
pickbuf.obj+
picklist.obj+
fileinfo.obj
$<,$*
\BCWIN\LIB\import32.lib+
\BCWIN\LIB\cw32mti.lib

|

conditio.obj :  ..\..\..\class\conditio.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\class\conditio.cpp
|

tracer.obj :  ..\..\..\class\tracer.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\class\tracer.cpp
|

getopt.obj :  ..\..\..\func\getopt.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\func\getopt.cpp
|

rcsver.obj :  ..\..\..\func\rcsver.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\func\rcsver.cpp
|

strchri.obj :  ..\..\..\func\strchri.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\func\strchri.cpp
|

strrchri.obj :  ..\..\..\func\strrchri.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\func\strrchri.cpp
|

strsrch.obj :  ..\..\..\func\strsrch.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\func\strsrch.cpp
|

strstri.obj :  ..\..\..\func\strstri.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\func\strstri.cpp
|

cstring.obj :  ..\..\..\class\cstring.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\class\cstring.cpp
|

date.obj :  ..\..\..\class\date.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\class\date.cpp
|

dir.obj :  ..\..\..\class\dir.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\class\dir.cpp
|

regexp.obj :  ..\..\..\class\regexp.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\class\regexp.cpp
|

time.obj :  ..\..\..\class\time.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\..\..\class\time.cpp
|

globals.obj :  ..\globals.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\globals.cpp
|

osvideo.obj :  ..\osvideo.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\osvideo.cpp
|

osgetch.obj :  ..\osgetch.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\osgetch.cpp
|

osser.obj :  ..\osser.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\osser.cpp
|

fileman.obj :  ..\fileman.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\fileman.cpp
|

osstyle.obj :  ..\osstyle.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\osstyle.cpp
|

osedit.obj :  ..\osedit.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\osedit.cpp
|

os.obj :  ..\os.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\os.cpp
|

osmisc.obj :  ..\osmisc.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\osmisc.cpp
|

ostime.obj :  ..\ostime.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\ostime.cpp
|

osview.obj :  ..\osview.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\osview.cpp
|

pickbuf.obj :  ..\pickbuf.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\pickbuf.cpp
|

picklist.obj :  ..\picklist.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\picklist.cpp
|

fileinfo.obj :  ..\fileinfo.cpp
  $(BCC32) -c @&&|
 $(CEAT_osexedexe) $(CNIEAT_osexedexe) -o$@ ..\fileinfo.cpp
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-R
-vi
-H
-H=list2.csm
-d
-x-
-xd-
-RT-
-H=OS.CSM
-WM
-WC
| $@


