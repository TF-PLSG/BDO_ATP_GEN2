# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=ptetcp - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ptetcp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ptetcp - Win32 Release" && "$(CFG)" != "ptetcp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "PTETCP.MAK" CFG="ptetcp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ptetcp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ptetcp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "ptetcp - Win32 Release"
CPP=cl.exe

!IF  "$(CFG)" == "ptetcp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ptetcp__"
# PROP BASE Intermediate_Dir "ptetcp__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.\Release

ALL : "$(OUTDIR)\PTETCP.lib"

CLEAN : 
	-@erase "$(INTDIR)\Cdsapi.obj"
	-@erase "$(INTDIR)\ptetcp.obj"
	-@erase "$(OUTDIR)\PTETCP.lib"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTETCP.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTETCP.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Cdsapi.obj" \
	"$(INTDIR)\ptetcp.obj" \
	"C:\MSDEV\lib\WSOCK32.LIB"

"$(OUTDIR)\PTETCP.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ptetcp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.\Debug

ALL : "$(OUTDIR)\PTETCP.lib"

CLEAN : 
	-@erase "$(INTDIR)\Cdsapi.obj"
	-@erase "$(INTDIR)\ptetcp.obj"
	-@erase "$(OUTDIR)\PTETCP.lib"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MTd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTETCP.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTETCP.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Cdsapi.obj" \
	"$(INTDIR)\ptetcp.obj" \
	"C:\MSDEV\lib\WSOCK32.LIB"

"$(OUTDIR)\PTETCP.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "ptetcp - Win32 Release"
# Name "ptetcp - Win32 Debug"

!IF  "$(CFG)" == "ptetcp - Win32 Release"

!ELSEIF  "$(CFG)" == "ptetcp - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ptetcp.c
DEP_CPP_PTETC=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\ntutils.h"\
	{$(INCLUDE)}"\pte.h"\
	{$(INCLUDE)}"\PTEMSG.H"\
	

"$(INTDIR)\ptetcp.obj" : $(SOURCE) $(DEP_CPP_PTETC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=C:\MSDEV\lib\WSOCK32.LIB

!IF  "$(CFG)" == "ptetcp - Win32 Release"

!ELSEIF  "$(CFG)" == "ptetcp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cdsapi.c
DEP_CPP_CDSAP=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\cdsapi.h"\
	{$(INCLUDE)}"\coreinfo.h"\
	{$(INCLUDE)}"\memdata.h"\
	{$(INCLUDE)}"\memerr.h"\
	{$(INCLUDE)}"\mempubd.h"\
	{$(INCLUDE)}"\mempubp.h"\
	{$(INCLUDE)}"\mmcos.h"\
	{$(INCLUDE)}"\mmopsys.h"\
	{$(INCLUDE)}"\mmxos.h"\
	{$(INCLUDE)}"\mmxosoff.h"\
	{$(INCLUDE)}"\momerr.h"\
	{$(INCLUDE)}"\mompubd.h"\
	{$(INCLUDE)}"\pte.h"\
	{$(INCLUDE)}"\pteipc.h"\
	{$(INCLUDE)}"\PTEMSG.H"\
	{$(INCLUDE)}"\ptetcp.h"\
	{$(INCLUDE)}"\queerr.h"\
	{$(INCLUDE)}"\quepubd.h"\
	{$(INCLUDE)}"\quepubp.h"\
	{$(INCLUDE)}"\semerr.h"\
	{$(INCLUDE)}"\sempubd.h"\
	{$(INCLUDE)}"\sempubp.h"\
	{$(INCLUDE)}"\xipc.h"\
	{$(INCLUDE)}"\xipcasyn.h"\
	{$(INCLUDE)}"\xipcerr.h"\
	{$(INCLUDE)}"\xipcpubd.h"\
	{$(INCLUDE)}"\xipcpubp.h"\
	{$(INCLUDE)}"\xipcpuxd.h"\
	{$(INCLUDE)}"\xneterr.h"\
	

"$(INTDIR)\Cdsapi.obj" : $(SOURCE) $(DEP_CPP_CDSAP) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
