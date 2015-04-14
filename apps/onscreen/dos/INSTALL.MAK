#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCCDOS  = Bcc +BccDos.cfg 
TLINK   = TLink
TLIB    = TLib
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSDOS =  -L\BC4\LIB
IDE_BFLAGS = 
LLATDOS_installdexe =  -L\BCWIN\LIB -c -Tde
RLATDOS_installdexe = 
BLATDOS_installdexe = 
CNIEAT_installdexe = -I\BCWIN\INCLUDE;\CPP\INCLUDE;..\; -DDEBUG
LNIEAT_installdexe = -x
LEAT_installdexe = $(LLATDOS_installdexe)
REAT_installdexe = $(RLATDOS_installdexe)
BEAT_installdexe = $(BLATDOS_installdexe)

#
# Dependency List
#
Dep_install = \
   install.exe

install : BccDos.cfg $(Dep_install)
  echo MakeNode 

Dep_installdexe = \
   install.obj\
   osvideo.obj

install.exe : $(Dep_installdexe)
  $(TLINK)   @&&|
  $(IDE_LFLAGSDOS) $(LEAT_installdexe) $(LNIEAT_installdexe) +
\BCWIN\LIB\c0l.obj+
install.obj+
osvideo.obj
$<,$*
\BCWIN\LIB\cl.lib

|

install.obj :  ..\install.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_installdexe) $(CNIEAT_installdexe) -o$@ ..\install.cpp
|

osvideo.obj :  ..\osvideo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_installdexe) $(CNIEAT_installdexe) -o$@ ..\osvideo.cpp
|

# Compiler configuration file
BccDos.cfg :
	Copy &&|
-W-
-R
-vi
-H
-H=install.csm
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
-ml
-f-
| $@


