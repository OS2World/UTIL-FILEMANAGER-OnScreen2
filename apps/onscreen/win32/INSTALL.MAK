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
LLATC32_installdexe =  -L\BCWIN\LIB; -v -Tpe -ap -c
RLATC32_installdexe =  -w32
BLATC32_installdexe = 
CNIEAT_installdexe = -I\BCWIN\INCLUDE;..\..\..\INCLUDE;..\; -D
LNIEAT_installdexe = -x
LEAT_installdexe = $(LLATC32_installdexe)
REAT_installdexe = $(RLATC32_installdexe)
BEAT_installdexe = $(BLATC32_installdexe)

#
# Dependency List
#
Dep_install = \
   install.exe

install : BccW32.cfg $(Dep_install)
  echo MakeNode 

Dep_installdexe = \
   install.obj\
   osvideo.obj

install.exe : $(Dep_installdexe)
  $(TLINK32) @&&|
  $(IDE_LFLAGS32) $(LEAT_installdexe) $(LNIEAT_installdexe) +
\BCWIN\LIB\c0x32.obj+
install.obj+
osvideo.obj
$<,$*
\BCWIN\LIB\import32.lib+
\BCWIN\LIB\cw32.lib

|

install.obj :  ..\install.cpp
  $(BCC32) -c @&&|
 $(CEAT_installdexe) $(CNIEAT_installdexe) -o$@ ..\install.cpp
|

osvideo.obj :  ..\osvideo.cpp
  $(BCC32) -c @&&|
 $(CEAT_installdexe) $(CNIEAT_installdexe) -o$@ ..\osvideo.cpp
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
-WC
| $@


