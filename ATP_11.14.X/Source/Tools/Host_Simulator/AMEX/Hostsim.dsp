# Microsoft Developer Studio Project File - Name="hostsim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=hostsim - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Hostsim.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Hostsim.mak" CFG="hostsim - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hostsim - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "hostsim - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hostsim - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "D:\HostAmex\include\core_1_2_1" /I "D:\HostAmex\include\bea_1_0_0" /I "C:\veena\bdo\branches\Amex\Host_sim_libs\include\bea_1_0_0" /I "C:\veena\bdo\branches\Amex\Host_sim_libs\include\Core_1_2_1" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib pte.lib ptecom.lib pteipc.lib /nologo /subsystem:windows /pdb:none /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"nafxcw" /out:".\Release/Hostsim_Amex.exe" /libpath:"D:\HostAmex\lib\core\1_2_1" /libpath:"D:\HostAmex\lib\bea\1_0_0" /libpath:"C:\veena\bdo\branches\Amex\Host_sim_libs\lib\bea\1_0_0" /libpath:"C:\veena\bdo\branches\Amex\Host_sim_libs\lib\core\1_2_1"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "hostsim - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "D:\HostAmex\include\core_1_2_1" /I "D:\HostAmex\include\bea_1_0_0" /I "C:\veena\bdo\branches\Amex\Host_sim_libs\include\bea_1_0_0" /I "C:\veena\bdo\branches\Amex\Host_sim_libs\include\Core_1_2_1" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /X /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib pte.lib ptecom.lib pteipc.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"nafxcw" /libpath:"D:\HostAmex\lib\core\1_2_1" /libpath:"D:\HostAmex\lib\bea\1_0_0" /libpath:"C:\veena\bdo\branches\Amex\Host_sim_libs\lib\bea\1_0_0" /libpath:"C:\veena\bdo\branches\Amex\Host_sim_libs\lib\core\1_2_1"

!ENDIF 

# Begin Target

# Name "hostsim - Win32 Release"
# Name "hostsim - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\convert.c
# End Source File
# Begin Source File

SOURCE=.\hostsim.cpp
# End Source File
# Begin Source File

SOURCE=.\hostsim.rc
# End Source File
# Begin Source File

SOURCE=.\hostsimDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\hostsimn.c
# End Source File
# Begin Source File

SOURCE=.\response.c
# End Source File
# Begin Source File

SOURCE=.\serialc.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\tcpip.cpp
# End Source File
# Begin Source File

SOURCE=.\trace.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\hostsimc.h
# End Source File
# Begin Source File

SOURCE=.\hostsimDlg.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RESPONSE.H
# End Source File
# Begin Source File

SOURCE=.\SERIALC.H
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TCPIP.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\hostsim.ico
# End Source File
# Begin Source File

SOURCE=.\res\hostsim.rc2
# End Source File
# End Group
# End Target
# End Project
