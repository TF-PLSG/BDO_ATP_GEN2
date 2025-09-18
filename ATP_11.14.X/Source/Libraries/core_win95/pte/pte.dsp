# Microsoft Developer Studio Project File - Name="PTE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=PTE - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pte.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pte.mak" CFG="PTE - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PTE - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "PTE - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PTE - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\..\Third_Party_Libraries\Win95\SSL" /I "..\..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\..\..\Source\Libraries\include" /I "..\..\Application_Win95\include" /I "..\..\..\Server_AIX\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "SSL_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\release\pte.lib  ..\..\..\build_gui\lib\release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "..\..\include" /I "..\..\..\Third_Party_Libraries\Win95\SSL" /I "..\..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\..\..\Source\Libraries\include" /I "..\..\Application_Win95\include" /I "..\..\..\Server_AIX\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SSL_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\debug\pte.lib  ..\..\..\build_gui\lib\debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "PTE - Win32 Release"
# Name "PTE - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\des.c
# End Source File
# Begin Source File

SOURCE=.\logger.c
# End Source File
# Begin Source File

SOURCE=.\ntutils.c
# End Source File
# Begin Source File

SOURCE=.\odbclogin.cpp
# End Source File
# Begin Source File

SOURCE=.\ptecopy.c
# End Source File
# Begin Source File

SOURCE=.\ptedes.c
# End Source File
# Begin Source File

SOURCE=.\ptehash.c
# End Source File
# Begin Source File

SOURCE=.\pteinit.c
# End Source File
# Begin Source File

SOURCE=.\ptellist.c
# End Source File
# Begin Source File

SOURCE=.\ptemsg.c
# End Source File
# Begin Source File

SOURCE=.\ptestats.c
# End Source File
# Begin Source File

SOURCE=.\ptesystm.c
# End Source File
# Begin Source File

SOURCE=.\ptetime.c
# End Source File
# Begin Source File

SOURCE=.\ptetimer.c
# End Source File
# Begin Source File

SOURCE=.\startup.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
