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
LLATC32_osdexe =  -L\BCWIN\LIB; -v -Tpe -ap -c
RLATC32_osdexe =  -w32
BLATC32_osdexe = 
CNIEAT_osdexe = -I\BCWIN\INCLUDE;..\..\..\INCLUDE;..\; -D_RTLDLL;
LNIEAT_osdexe = -x
LEAT_osdexe = $(LLATC32_osdexe)
REAT_osdexe = $(RLATC32_osdexe)
BEAT_osdexe = $(BLATC32_osdexe)
CLATW16_ddbddbddbwin32blibbjaclib10dlib = 
LLATW16_ddbddbddbwin32blibbjaclib10dlib = 
RLATW16_ddbddbddbwin32blibbjaclib10dlib = 
BLATW16_ddbddbddbwin32blibbjaclib10dlib = 
CEAT_ddbddbddbwin32blibbjaclib10dlib = $(CEAT_osdexe) $(CLATW16_ddbddbddbwin32blibbjaclib10dlib)
CNIEAT_ddbddbddbwin32blibbjaclib10dlib = -I\BCWIN\INCLUDE;..\..\..\INCLUDE;..\; -D_RTLDLL;
LNIEAT_ddbddbddbwin32blibbjaclib10dlib = -x
LEAT_ddbddbddbwin32blibbjaclib10dlib = $(LEAT_osdexe) $(LLATW16_ddbddbddbwin32blibbjaclib10dlib)
REAT_ddbddbddbwin32blibbjaclib10dlib = $(REAT_osdexe) $(RLATW16_ddbddbddbwin32blibbjaclib10dlib)
BEAT_ddbddbddbwin32blibbjaclib10dlib = $(BEAT_osdexe) $(BLATW16_ddbddbddbwin32blibbjaclib10dlib)

#
# Dependency List
#
Dep_os = \
   os.exe

os : BccW32.cfg $(Dep_os)
  echo MakeNode 

Dep_osdexe = \
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
   fileinfo.obj\
   ..\..\..\win32\lib\jaclib10.lib

os.exe : $(Dep_osdexe)
  $(TLINK32) @&&|
  $(IDE_LFLAGS32) $(LEAT_osdexe) $(LNIEAT_osdexe) +
\BCWIN\LIB\c0x32.obj+
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
..\..\..\win32\lib\jaclib10.lib+
\BCWIN\LIB\import32.lib+
\BCWIN\LIB\cw32mti.lib

|

globals.obj :  ..\globals.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\globals.cpp
|

osvideo.obj :  ..\osvideo.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osvideo.cpp
|

osgetch.obj :  ..\osgetch.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osgetch.cpp
|

osser.obj :  ..\osser.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osser.cpp
|

fileman.obj :  ..\fileman.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\fileman.cpp
|

osstyle.obj :  ..\osstyle.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osstyle.cpp
|

osedit.obj :  ..\osedit.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osedit.cpp
|

os.obj :  ..\os.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\os.cpp
|

osmisc.obj :  ..\osmisc.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osmisc.cpp
|

ostime.obj :  ..\ostime.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\ostime.cpp
|

osview.obj :  ..\osview.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\osview.cpp
|

pickbuf.obj :  ..\pickbuf.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\pickbuf.cpp
|

picklist.obj :  ..\picklist.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\picklist.cpp
|

fileinfo.obj :  ..\fileinfo.cpp
  $(BCC32) -c @&&|
 $(CEAT_osdexe) $(CNIEAT_osdexe) -o$@ ..\fileinfo.cpp
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-D__PENMAN_INTERNAL__
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


