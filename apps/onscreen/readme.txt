 			  Welcome to John Allen's OnScreen/2 Release 2.50.
			 --------------------------------------------------

 This README file contains important information about OnScreen/2.

 Changes
 ------------------------------------------------------------------------------
 General
 The os.ini file is now obselete, all settings are saved in the os.set file.


 Platform Specifics
 ------------------------------------------------------------------------------
 IBM OS/2 2.x & Warp 3.0/4.0
 OnScreen/2  is  supported  on OS/2 versions 2.0, 2.1, 2.11 & Warp 3.0.  If you
 have installed the OS/2 .DLL based version and did not install the .DLL's to a
 directory already  specified  on  your  LIBPATH  you  will  need  to edit your
 config.sys file and add the directory you choose for the .DLL's to it.

 Microsoft Windows NT & Windows95
 OnScreen/2 is supported on Windows  NT  3.5/4.0  and  Windows 95.  If you have
 installed the Win32 .DLL based version and did not install  the  .DLL's  to  a
 directory  already  specified  on your PATH you will need to add the directory
 you choose for the .DLL's to it.

 PC-DOS
 Release 2.50 is not supported on DOS.

 Sharing
 ------------------------------------------------------------------------------
 Please do give copies of OnScreen/2 to your friends, local BBS's etc.   Please
 ensure  that  you pass them the original OnScreen/2 zipfile intact.

 Distribution format
 ------------------------------------------------------------------------------
 As and from release 2.02, OnScreen/2 will no longer be distributed in one .zip
 file, instead each supported operating system will have  its  own  .zip  file.
 This  means  that  when you unzip a particular OnScreen/2 zipfile you will not
 get the entire directory structure as  listed  below, you will instead get the
 files directly under the ONSCREEN directory and one of the OS2, DOS  or  WIN32
 directories.

 The file naming convention used will be as follows:-

 Program Name   Version  Major_Minor Operating System    Status
 OS                      2    _00    O = OS/2            A = Alpha
 OS                      2    _00    W = Win32           G = Gamma
                                                         R = Release

 As  an  example of this the OS/2 2.02 version will be distributed in a zipfile
 called OS2_02OR.ZIP.

 Directory structure
 ------------------------------------------------------------------------------
 OnScreen/2  will  unpack  into  a  directory  named  ONSCREEN  in  the current
 directory, the install.exe will be in the current directory.  The easiest  way
 to  install  OnScreen/2  is  to  unzip it to a floppy and then run a:\install.
 Remember to use the -d switch with PKUNZIP to create the directory structure.

 The directory structure is as follows
 .\install.exe                           OS/2, DOS or Win32 install program
 .\ONSCREEN\readme.txt                   Readme please! (this file)
 .\ONSCREEN\os.doc                       Manual in Microsoft Word format
 .\ONSCREEN\os.man                       Manual in OnScreen/2 text format
 .\ONSCREEN\file_id.diz                  BBS description file
 .\ONSCREEN\license.doc                  License agreement
 .\ONSCREEN\os.hlp                       Online help
 .\ONSCREEN\os.set                       Settings file
 .\ONSCREEN\usa.reg                      US Registration form
 .\ONSCREEN\row.reg                      Rest of the World Registration form

 Additional directories for IBM OS/2 2.x, OS/2 Warp 3.0
 .\ONSCREEN\OS2                    <DIR> OS/2 2.x/Warp implementation
 .\ONSCREEN\OS2\os.inf                   Manual in OS/2 .inf format
 .\ONSCREEN\OS2\os.exe                   OnScreen/2 that does not require .DLL's
 .\ONSCREEN\OS2\osreg.exe                OS/2 registration executable
 .\ONSCREEN\OS2\dll                <DIR> OS/2 .DLL based implementation
 .\ONSCREEN\OS2\dll\c215mt.dll           Borland C++ OS/2 runtime .DLL
 .\ONSCREEN\OS2\dll\wf7lib10.dll         OS/2 2.x/Warp support .DLL
 .\ONSCREEN\OS2\dll\os.exe               OnScreen/2 that requires .DLL's

 Additional directories for Win32 (NT & Win95)
 .\ONSCREEN\WIN32                  <DIR> Win32 implementation
 .\ONSCREEN\WIN32\os.exe                 OnScreen/2 that does not require .DLL's
 .\ONSCREEN\WIN32\osreg.exe              Win32 registration executable
 .\ONSCREEN\WIN32\dll              <DIR> Win32 .DLL based implementation
 .\ONSCREEN\WIN32\dll\wf7lib10.dll       Win32 support .DLL
 .\ONSCREEN\WIN32\dll\cw3211mt.dll       Borland C++ Win32 runtime .DLL
 .\ONSCREEN\WIN32\dll\os.exe             OnScreen/2 that requires .DLL's

 ------------------------------------------------------------------------------

 If you have any installation difficulties send me a mail message detailing the
 specifics of the problem

 Thank you for supporting OnScreen/2.

 Yours sincerely John J. Allen.
 E-Mail: john.allen@codemountain.net

 See OS.MAN for additional information.

 -----------------------------------END----------------------------------------

