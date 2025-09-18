# Microsoft Developer Studio Project File - Name="ptedb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ptedb - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ptedb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ptedb.mak" CFG="ptedb - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ptedb - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ptedb - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ptedb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\ptedb___"
# PROP BASE Intermediate_Dir ".\ptedb___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\build_gui\lib\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\Libraries\core_aix\include" /I "..\..\..\Third_Party_Libraries\AIX\Xipc\include" /I "..\..\..\Third_Party_Libraries\ORA804\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ptedb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\ptedb__0"
# PROP BASE Intermediate_Dir ".\ptedb__0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\build_gui\lib\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "..\..\..\Libraries\core_aix\include" /I "..\..\..\Third_Party_Libraries\AIX\Xipc\include" /I "..\..\..\Third_Party_Libraries\ORA804\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ptedb - Win32 Release"
# Name "ptedb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\dbcommon.c
# End Source File
# Begin Source File

SOURCE=.\dbcommon.pc

!IF  "$(CFG)" == "ptedb - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\dbcommon.pc
InputName=dbcommon

"$(ProjDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics  userid=core/pinnacle@equitable

# End Custom Build

!ELSEIF  "$(CFG)" == "ptedb - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\dbcommon.pc
InputName=dbcommon

"$(ProjDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics  userid=core/pinnacle@equitable

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dstimer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Third_Party_Libraries\Ora9i\lib\oraSQL9.LIB
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
