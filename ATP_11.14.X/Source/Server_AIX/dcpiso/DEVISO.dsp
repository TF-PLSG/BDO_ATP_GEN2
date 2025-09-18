# Microsoft Developer Studio Project File - Name="deviso" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=deviso - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DEVISO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DEVISO.mak" CFG="deviso - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "deviso - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "deviso - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "deviso - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\deviso__"
# PROP BASE Intermediate_Dir ".\deviso__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /W3 /GX /Zd /O2 /I "..\include" /I "..\..\Libraries\core_aix\include" /I "..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\Libraries\Application_Win95\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib pte.lib pteipc.lib atpl.lib xipc.lib /nologo /subsystem:console /pdb:none /map /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcmtd" /out:"..\..\build_gui\bin\Release\dcpiso.exe" /libpath:"..\..\Build_GUI\lib\Release" /libpath:"C:\XIPC\lib"

!ELSEIF  "$(CFG)" == "deviso - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\deviso_0"
# PROP BASE Intermediate_Dir ".\deviso_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /ZI /Od /I "..\include" /I "..\..\Libraries\core_aix\include" /I "..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\Libraries\Application_Win95\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib pte.lib pteipc.lib atpl.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"libc" /out:"..\..\build_gui\bin\Debug\dcpiso.exe" /libpath:"n:\pinnacle\pte\lib\core\1_3_2_2" /libpath:"n:\pinnacle\pte\lib\equitable\4_4_1\debug" /libpath:"..\..\Build_GUI\lib\debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "deviso - Win32 Release"
# Name "deviso - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\batch_upload.c
# End Source File
# Begin Source File

SOURCE=..\Common\CONSOLE.C

!IF  "$(CFG)" == "deviso - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "deviso - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\database_request.c
# End Source File
# Begin Source File

SOURCE=.\database_response.c
# End Source File
# Begin Source File

SOURCE=.\general_utilities.c
# End Source File
# Begin Source File

SOURCE=.\iso_utilities.c
# End Source File
# Begin Source File

SOURCE=.\message_handler.c
# End Source File
# Begin Source File

SOURCE=.\process_incoming_msg.c
# End Source File
# Begin Source File

SOURCE=.\process_outgoing_msg.c
# End Source File
# Begin Source File

SOURCE=..\Common\SERVICE.C

!IF  "$(CFG)" == "deviso - Win32 Release"

!ELSEIF  "$(CFG)" == "deviso - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\service_functions.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\batch_upload.h
# End Source File
# Begin Source File

SOURCE=.\database_request.h
# End Source File
# Begin Source File

SOURCE=.\database_response.h
# End Source File
# Begin Source File

SOURCE=.\general_utilities.h
# End Source File
# Begin Source File

SOURCE=.\global_definitions.h
# End Source File
# Begin Source File

SOURCE=.\iso_utilities.h
# End Source File
# Begin Source File

SOURCE=.\isoutil.h
# End Source File
# Begin Source File

SOURCE=.\message_handler.h
# End Source File
# Begin Source File

SOURCE=.\process_incoming_msg.h
# End Source File
# Begin Source File

SOURCE=.\process_outgoing_msg.h
# End Source File
# Begin Source File

SOURCE=.\service_functions.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
