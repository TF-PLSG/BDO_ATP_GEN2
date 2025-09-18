# Microsoft Developer Studio Project File - Name="TxCntl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=TxCntl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "txcntl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "txcntl.mak" CFG="TxCntl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TxCntl - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "TxCntl - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TxCntl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /Zd /O2 /I "..\include" /I "..\..\Libraries\core_win95\include" /I "..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\Libraries\Application_Win95\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fr /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib pte.lib pteipc.lib ATPL.lib xipc.lib /nologo /subsystem:console /map /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /out:"..\..\build_gui\bin\Release\txcntl.exe" /libpath:"..\..\..\Source\Build_GUI\lib\Release" /libpath:"C:\XIPC\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TxCntl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /ZI /Od /I "..\include" /I "..\..\Libraries\core_win95\include" /I "..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\Libraries\Application_Win95\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib pte.lib pteipc.lib atpl.lib /nologo /subsystem:console /map /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcmt" /nodefaultlib:"libcd" /out:"..\..\build_gui\bin\Debug\txcntl.exe" /libpath:"..\..\Build_GUI\lib\debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "TxCntl - Win32 Release"
# Name "TxCntl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\Common\CONSOLE.C

!IF  "$(CFG)" == "TxCntl - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TxCntl - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\myutils.c
# End Source File
# Begin Source File

SOURCE=..\Common\SERVICE.C

!IF  "$(CFG)" == "TxCntl - Win32 Release"

!ELSEIF  "$(CFG)" == "TxCntl - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\txauthorize.c
# End Source File
# Begin Source File

SOURCE=.\txcntl.c
# End Source File
# Begin Source File

SOURCE=.\txdataserver.c
# End Source File
# Begin Source File

SOURCE=.\txfleet.c
# End Source File
# Begin Source File

SOURCE=.\txservice.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\myutils.h
# End Source File
# Begin Source File

SOURCE=.\txauthorize.h
# End Source File
# Begin Source File

SOURCE=.\TxCntl.h
# End Source File
# Begin Source File

SOURCE=.\TxCntl_Constants.h
# End Source File
# Begin Source File

SOURCE=.\txdataserver.h
# End Source File
# Begin Source File

SOURCE=.\txfleet.h
# End Source File
# Begin Source File

SOURCE=.\txservice.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
