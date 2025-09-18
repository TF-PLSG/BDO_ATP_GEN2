# Microsoft Developer Studio Project File - Name="PinDlg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PinDlg - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PinDlg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PinDlg.mak" CFG="PinDlg - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PinDlg - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PinDlg - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PinDlg - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /I "..\..\Libraries\include" /I "..\..\Server_AIX\include" /I "..\..\Libraries\Application_Win95\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 odbc32.lib odbccp32.lib wsock32.lib pte.lib atptcp.lib ptetcp.lib mutex.lib shrmem.lib ptepci.lib version.lib /nologo /subsystem:windows /pdb:none /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"nafxcw" /nodefaultlib:"libc" /nodefaultlib:"nafxcwd" /libpath:"..\..\build_gui\lib\Release"
# SUBTRACT LINK32 /debug /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Release\PinDlg.exe ..\..\Build_GUI\Bin\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "PinDlg - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\..\Libraries\include" /I "..\..\Libraries\Application_Win95\include" /I "..\..\Third_Party_Libraries\Win95\SSL" /I "..\..\Server_AIX\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 odbc32.lib odbccp32.lib wsock32.lib pte.lib atptcp.lib ptetcp.lib mutex.lib shrmem.lib version.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"libcd" /nodefaultlib:"nafxcw" /nodefaultlib:"nafxcwd" /libpath:"..\..\build_gui\lib\Debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\debug\pindlg.exe  ..\..\build_gui\bin\Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "PinDlg - Win32 Release"
# Name "PinDlg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\ComSetupPg.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogPg.cpp
# End Source File
# Begin Source File

SOURCE=.\Loginfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\PinDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PinDlg.rc
# End Source File
# Begin Source File

SOURCE=.\PropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\RouterRec.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TPDUPg.cpp
# End Source File
# Begin Source File

SOURCE=.\TPDURec.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\ComSetupPg.h
# End Source File
# Begin Source File

SOURCE=.\DialogPg.h
# End Source File
# Begin Source File

SOURCE=.\Loginfrm.h
# End Source File
# Begin Source File

SOURCE=.\PinDlg.h
# End Source File
# Begin Source File

SOURCE=.\PropSheet.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RouterRec.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TPDUPg.h
# End Source File
# Begin Source File

SOURCE=.\TPDURec.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\offline.bmp
# End Source File
# Begin Source File

SOURCE=.\res\online.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PinDlg.ico
# End Source File
# Begin Source File

SOURCE=.\res\PinDlg.rc2
# End Source File
# Begin Source File

SOURCE=.\pinn256.bmp
# End Source File
# End Group
# End Target
# End Project
