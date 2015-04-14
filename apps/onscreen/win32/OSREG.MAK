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
LLATC32_osregdexe =  -L\BCWIN\LIB; -Tpe -ap -c
RLATC32_osregdexe =  -w32
BLATC32_osregdexe = 
CNIEAT_osregdexe = -I\BCWIN\INCLUDE;..\INCLUDE; -D
LNIEAT_osregdexe = -x
LEAT_osregdexe = $(LLATC32_osregdexe)
REAT_osregdexe = $(RLATC32_osregdexe)
BEAT_osregdexe = $(BLATC32_osregdexe)

#
# Dependency List
#
Dep_osreg = \
   osreg.exe

osreg : BccW32.cfg $(Dep_osreg)
  echo MakeNode 

Dep_osregdexe = \
   osreg.obj

osreg.exe : $(Dep_osregdexe)
  $(TLINK32) @&&|
  $(IDE_LFLAGS32) $(LEAT_osregdexe) $(LNIEAT_osregdexe) +
\BCWIN\LIB\c0x32.obj+
osreg.obj
$<,$*
\BCWIN\LIB\import32.lib+
\BCWIN\LIB\cw32mt.lib

|

osreg.obj :  ..\osreg.cpp
  $(BCC32) -c @&&|
 $(CEAT_osregdexe) $(CNIEAT_osregdexe) -o$@ ..\osreg.cpp
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
-WM
-WC
| $@


