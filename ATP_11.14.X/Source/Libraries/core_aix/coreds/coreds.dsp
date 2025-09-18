# Microsoft Developer Studio Project File - Name="coreds" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=coreds - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "coreds.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "coreds.mak" CFG="coreds - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "coreds - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "coreds - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "coreds - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\Libraries\core_aix\include" /I "..\..\..\Libraries\Application_Win95\include" /I "..\..\..\Third_Party_Libraries\ORA804\include" /I "..\..\..\Third_Party_Libraries\AIX\Xipc\include" /I "..\..\..\Third_Party_Libraries\Win95\SSL" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "SSL_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib winspool.lib wsock32.lib advapi32.lib pte.lib pteipc.lib ptedb.lib xipc.lib /nologo /subsystem:console /pdb:none /machine:I386 /nodefaultlib:"libcmtd" /nodefaultlib:"libc" /out:"..\..\build_gui\bin\Release\coreds.exe" /libpath:"..\..\..\Build_GUI\lib\release" /libpath:"C:\XIPC\lib"

!ELSEIF  "$(CFG)" == "coreds - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "..\..\..\Libraries\core_aix\include" /I "..\..\..\Libraries\Application_Win95\include" /I "..\..\..\Third_Party_Libraries\ORA804\include" /I "..\..\..\Third_Party_Libraries\AIX\Xipc\include" /I "..\..\..\Third_Party_Libraries\Win95\SSL" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "SSL_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib wsock32.lib pte.lib pteipc.lib ptedb.lib xipc.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcd" /libpath:"..\..\..\Build_GUI\lib\debug" /libpath:"C:\XIPC\lib"

!ENDIF 

# Begin Target

# Name "coreds - Win32 Release"
# Name "coreds - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\..\Server_AIX\Common\CONSOLE.C

!IF  "$(CFG)" == "coreds - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coreds - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\coredb.c
# End Source File
# Begin Source File

SOURCE=.\coredb.pc

!IF  "$(CFG)" == "coreds - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\coredb.pc
InputName=coredb

"$(ProjDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=core/pinnacle@equitable

# End Custom Build

!ELSEIF  "$(CFG)" == "coreds - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\coredb.pc
InputName=coredb

"$(ProjDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=core/pinnacle@equitable

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\coredb.sqc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\coreds.c
# End Source File
# Begin Source File

SOURCE=.\dm_config.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Server_AIX\Common\SERVICE.C

!IF  "$(CFG)" == "coreds - Win32 Release"

!ELSEIF  "$(CFG)" == "coreds - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\coredb.h
# End Source File
# Begin Source File

SOURCE=.\dm_config.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
