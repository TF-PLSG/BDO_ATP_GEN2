# Microsoft Developer Studio Project File - Name="atptcp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=atptcp - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Atptcp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Atptcp.mak" CFG="atptcp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "atptcp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "atptcp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "atptcp - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\include" /I "..\..\Application_Win95\include" /I "..\..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\..\..\Source\Server_AIX\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
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
PostBuild_Cmds=copy .\release\atptcp.lib  ..\..\..\build_gui\lib\release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "atptcp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\..\include" /I "..\..\Application_Win95\include" /I "..\..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\..\..\Source\Server_AIX\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
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
PostBuild_Cmds=copy .\debug\atptcp.lib  ..\..\..\build_gui\lib\debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "atptcp - Win32 Release"
# Name "atptcp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\diskio.c
# End Source File
# Begin Source File

SOURCE=.\GENUTIL.C
# End Source File
# Begin Source File

SOURCE=.\MEMMNGER.C
# End Source File
# Begin Source File

SOURCE=.\timings.c
# End Source File
# Begin Source File

SOURCE=.\TXDSAPI.C
# End Source File
# Begin Source File

SOURCE=.\txtrace.c
# End Source File
# Begin Source File

SOURCE=.\TXUTILS.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\dc_database.h
# End Source File
# Begin Source File

SOURCE=.\dc_dbstruct.h
# End Source File
# Begin Source File

SOURCE=.\GENUTIL.H
# End Source File
# Begin Source File

SOURCE=.\MEMMNGER.H
# End Source File
# Begin Source File

SOURCE=.\nc_database.h
# End Source File
# Begin Source File

SOURCE=.\nc_dbstruct.h
# End Source File
# Begin Source File

SOURCE=.\timings.h
# End Source File
# Begin Source File

SOURCE=.\tx_database.h
# End Source File
# Begin Source File

SOURCE=.\txdsapi.h
# End Source File
# Begin Source File

SOURCE=.\TXUTILS.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
