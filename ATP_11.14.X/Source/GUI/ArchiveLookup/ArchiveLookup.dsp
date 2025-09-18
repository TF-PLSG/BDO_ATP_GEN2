# Microsoft Developer Studio Project File - Name="ArchiveLookup" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ArchiveLookup - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ArchiveLookup.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ArchiveLookup.mak" CFG="ArchiveLookup - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ArchiveLookup - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ArchiveLookup - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ArchiveLookup - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /I "..\..\Libraries\include" /I "..\..\Libraries\Application_Win95\include" /I "..\..\Third_Party_Libraries\Win95\SSL" /I "..\..\Server_AIX\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x4009 /d "NDEBUG"
# ADD RSC /l 0x4009 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 atptcp.lib pte.lib ptetcp.lib wsock32.lib shrmem.lib mutex.lib ptepci.lib version.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"nafxcw" /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"libcd" /nodefaultlib:"libc" /nodefaultlib:"nafxcwd" /libpath:"..\..\Build_GUI\lib\Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Release\ArchiveLookup.exe  ..\..\Build_gui\bin\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ArchiveLookup - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\Libraries\include" /I "..\..\Libraries\Application_Win95\include" /I "..\..\Third_Party_Libraries\Win95\SSL" /I "..\..\Server_AIX\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x4009 /d "_DEBUG"
# ADD RSC /l 0x4009 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 atptcp.lib pte.lib ptetcp.lib wsock32.lib shrmem.lib mutex.lib ptepci.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"nafxcw" /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"libcd" /nodefaultlib:"libc" /nodefaultlib:"nafxcwd" /pdbtype:sept /libpath:"..\..\Build_GUI\lib\Debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Debug\ArchiveLookup.exe  ..\..\Build_gui\bin\Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ArchiveLookup - Win32 Release"
# Name "ArchiveLookup - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ArchiveLookup.cpp
# End Source File
# Begin Source File

SOURCE=.\ArchiveLookup.rc
# End Source File
# Begin Source File

SOURCE=.\ArchiveLookupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BatchLookup.cpp
# End Source File
# Begin Source File

SOURCE=.\BCH01Details.cpp
# End Source File
# Begin Source File

SOURCE=.\BCH11Details.cpp
# End Source File
# Begin Source File

SOURCE=.\BCH11Lookup.cpp
# End Source File
# Begin Source File

SOURCE=.\BCH20Details.cpp
# End Source File
# Begin Source File

SOURCE=.\BCH20Lookup.cpp
# End Source File
# Begin Source File

SOURCE=.\GetTLFData.cpp
# End Source File
# Begin Source File

SOURCE=.\ListViewCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Loginfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainMenuDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MYUTILS.CPP
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TransactionDetails.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ArchiveLookup.h
# End Source File
# Begin Source File

SOURCE=.\ArchiveLookupDlg.h
# End Source File
# Begin Source File

SOURCE=.\BatchLookup.h
# End Source File
# Begin Source File

SOURCE=.\BCH01Details.h
# End Source File
# Begin Source File

SOURCE=.\BCH11Details.h
# End Source File
# Begin Source File

SOURCE=.\BCH11Lookup.h
# End Source File
# Begin Source File

SOURCE=.\BCH20Details.h
# End Source File
# Begin Source File

SOURCE=.\BCH20Lookup.h
# End Source File
# Begin Source File

SOURCE=.\CVector.h
# End Source File
# Begin Source File

SOURCE=.\GetTLFData.h
# End Source File
# Begin Source File

SOURCE=.\ListViewCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\Loginfrm.h
# End Source File
# Begin Source File

SOURCE=.\MainMenuDlg.h
# End Source File
# Begin Source File

SOURCE=.\MYUTILS.H
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TransactionDetails.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ArchiveLookup.ico
# End Source File
# Begin Source File

SOURCE=.\res\ArchiveLookup.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
