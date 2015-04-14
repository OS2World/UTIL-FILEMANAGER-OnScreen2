#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC     = Bcc +BccW16.cfg 
TLINK   = TLink
TLIB    = TLib
BRC     = Brc
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS =  -L
IDE_RFLAGS =  -w32
LLATW16_osregdexe =  -L\BCWIN\LIB;
RLATW16_osregdexe = 
BLATW16_osregdexe = 
CNIEAT_osregdexe = -I\BCWIN\INCLUDE; -DDEBUG
LNIEAT_osregdexe = -x
LEAT_osregdexe = $(LLATW16_osregdexe)
REAT_osregdexe = $(RLATW16_osregdexe)
BEAT_osregdexe = $(BLATW16_osregdexe)

#
# Dependency List
#
Dep_osreg = \
   osreg.exe

osreg : BccW16.cfg $(Dep_osreg)
  echo MakeNode 

Dep_osregdexe = \
   osreg.obj

osreg.exe : $(Dep_osregdexe)
  $(TLINK)   @&&|
  $(IDE_LFLAGS) $(LEAT_osregdexe) $(LNIEAT_osregdexe) +
\BCWIN\LIB\c0x32.obj+
osreg.obj
$<,$*
dpmi32.lib+
\BCWIN\LIB\cw32.lib

|

osreg.obj :  ..\osreg.cpp
  $(BCC)   -c @&&|
 $(CEAT_osregdexe) $(CNIEAT_osregdexe) -o$@ ..\osreg.cpp
|

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-vi
-H
-H=osreg.csm
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
-wdef
-wnod
-wamb
-wuse
-wstv
-wasm
-wamp
-wobs
-WS
| $@


