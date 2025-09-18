# Microsoft Developer Studio Project File - Name="VoiceAuthorization" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VoiceAuthorization - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VoiceAuthorization.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VoiceAuthorization.mak" CFG="VoiceAuthorization - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VoiceAuthorization - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VoiceAuthorization - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VoiceAuthorization - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "..\..\Libraries\include" /I "..\..\Server_AIX\include" /I "..\..\Libraries\Application_Win95\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 atptcp.lib ptetcp.lib pte.lib wsock32.lib mutex.lib shrmem.lib ptepci.lib version.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"libcd" /nodefaultlib:"libc" /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"nafxcw" /out:".\Release\VoiceAuthorization.exe" /libpath:"..\..\Build_GUI\lib\release"
# SUBTRACT LINK32 /pdb:none /incremental:yes /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Release\VoiceAuthorization.exe ..\..\build_gui\bin\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VoiceAuthorization - Win32 Debug"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\Libraries\include" /I "..\..\Server_AIX\include" /I "..\..\Libraries\Application_Win95\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 atptcp.lib ptetcp.lib pte.lib wsock32.lib mutex.lib shrmem.lib ptepci.lib version.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"libcd" /nodefaultlib:"libc" /nodefaultlib:"nafxcwd.lib" /out:".\Debug\VoiceAuthorization.exe" /pdbtype:sept /libpath:"..\..\Build_GUI\lib\Debug"
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Debug\VoiceAuthorization.exe ..\..\build_gui\bin\Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "VoiceAuthorization - Win32 Release"
# Name "VoiceAuthorization - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\IdleLoginFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MYUTILS.CPP
# End Source File
# Begin Source File

SOURCE=.\OverridePassword.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VoiceAuth.cpp
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthorization.cpp
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthorization.rc
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthorizationDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthorizationView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\IdleLoginFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MYUTILS.H
# End Source File
# Begin Source File

SOURCE=.\OverridePassword.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VoiceAuth.h
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthorization.h
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthorizationDoc.h
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthorizationIO.h
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthorizationView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VoiceAuthorization.ico
# End Source File
# Begin Source File

SOURCE=.\res\VoiceAuthorization.rc2
# End Source File
# Begin Source File

SOURCE=.\res\VoiceAuthorizationDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
