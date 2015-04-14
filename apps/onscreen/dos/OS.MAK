#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCCD32     = Bcc32 +BccD32.cfg
TLINK32 = TLink32
TLIB    = TLib
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSD32 =  -LD:\BC4\LIB
LLATD32_osdexe =  -L\BCWIN\LIB; -Tpe -ax
RLATD32_osdexe = 
BLATD32_osdexe = 
CNIEAT_osdexe = -I\BCWIN\INCLUDE;\CPP\INCLUDE;..\;
LNIEAT_osdexe = -x
LEAT_osdexe = $(LLATD32_osdexe)
REAT_osdexe = $(RLATD32_osdexe)
BEAT_osdexe = $(BLATD32_osdexe)

#
# Dependency List
#
Dep_os = \
   os.exe

os : BccD32.cfg $(Dep_os)
  echo MakeNode 

Dep_osdexe = \
   os.obj\
   fileinfo.obj\
   fileman.obj\
   globals.obj\
   osedit.obj\
   osgetch.obj\
   osmisc.obj\
   osser.obj\
   osstyle.obj\
   ostime.obj\
   osvideo.obj\
   osview.obj\
   pickbuf.obj\
   picklist.obj\
   strchri.obj\
   strstri.obj\
   getopt.obj\
   rcsver.obj\
   strsrch.obj\
   conditio.obj\
   cstring.obj\
   date.obj\
   dir.obj\
   regexp.obj\
   time.obj\
   tracer.obj

os.exe : $(Dep_osdexe)
  $(TLINK32) @&&|
  $(IDE_LFLAGSD32) $(LEAT_osdexe) $(LNIEAT_osdexe) +
\BCWIN\LIB\c0x32.obj+
os.obj+
fileinfo.obj+
fileman.obj+
globals.obj+
osedit.obj+
osgetch.obj+
osmisc.obj+
osser.obj+
osstyle.obj+
ostime.obj+
osvideo.obj+
osview.obj+
pickbuf.obj+
picklist.obj+
strchri.obj+
strstri.obj+
getopt.obj+
rcsver.obj+
strsrch.obj+
conditio.obj+
cstring.obj+
date.obj+
dir.obj+
regexp.obj+
time.obj+
tracer.obj
$<,$*
dpmi32.lib+
\BCWIN\LIB\cw32.lib

|

os.obj :  ..\os.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\os.cpp
|

fileinfo.obj :  ..\fileinfo.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\fileinfo.cpp
|

fileman.obj :  ..\fileman.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\fileman.cpp
|

globals.obj :  ..\globals.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\globals.cpp
|

osedit.obj :  ..\osedit.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osedit.cpp
|

osgetch.obj :  ..\osgetch.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osgetch.cpp
|

osmisc.obj :  ..\osmisc.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osmisc.cpp
|

osser.obj :  ..\osser.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osser.cpp
|

osstyle.obj :  ..\osstyle.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osstyle.cpp
|

ostime.obj :  ..\ostime.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\ostime.cpp
|

osvideo.obj :  ..\osvideo.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osvideo.cpp
|

osview.obj :  ..\osview.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osview.cpp
|

pickbuf.obj :  ..\pickbuf.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\pickbuf.cpp
|

picklist.obj :  ..\picklist.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\picklist.cpp
|

strchri.obj :  ..\..\..\func\strchri.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\func\strchri.cpp
|

strstri.obj :  ..\..\..\func\strstri.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\func\strstri.cpp
|

getopt.obj :  ..\..\..\func\getopt.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\func\getopt.cpp
|

rcsver.obj :  ..\..\..\func\rcsver.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\func\rcsver.cpp
|

strsrch.obj :  ..\..\..\func\strsrch.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\func\strsrch.cpp
|

conditio.obj :  ..\..\..\class\conditio.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\class\conditio.cpp
|

cstring.obj :  ..\..\..\class\cstring.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\class\cstring.cpp
|

date.obj :  ..\..\..\class\date.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\class\date.cpp
|

dir.obj :  ..\..\..\class\dir.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\class\dir.cpp
|

regexp.obj :  ..\..\..\class\regexp.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\class\regexp.cpp
|

time.obj :  ..\..\..\class\time.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\class\time.cpp
|

tracer.obj :  ..\..\..\class\tracer.cpp
  $(BCCD32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\..\..\class\tracer.cpp
|

# Compiler configuration file
BccD32.cfg : 
   Copy &&|
-R
-vi
-H
-H=os.csm
-d
-x-
-xd-
-RT-
-Ot
-wcln
-wsig
-wucp
-wbbf
-wpin
-w-nak
-wdef
-wnod
-wpro
-wamb
-wpar
-wuse
-wstv
-wasm
-wamp
-wobs
-wpch
-WX
| $@


