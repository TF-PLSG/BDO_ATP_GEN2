# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=ptedb - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ptedb - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ptedb - Win32 Release" && "$(CFG)" != "ptedb - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "PTEDB.MAK" CFG="ptedb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ptedb - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ptedb - Win32 Debug" (based on "Win32 (x86) Static Library")
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
# PROP Target_Last_Scanned "ptedb - Win32 Release"
CPP=cl.exe

!IF  "$(CFG)" == "ptedb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ptedb___"
# PROP BASE Intermediate_Dir "ptedb___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.\Release

ALL : "$(OUTDIR)\PTEDB.lib"

CLEAN : 
	-@erase "$(INTDIR)\dbcommon.obj"
	-@erase "$(INTDIR)\dstimer.obj"
	-@erase "$(OUTDIR)\PTEDB.lib"
	-@erase ".\dbcommon.c"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTEDB.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTEDB.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dbcommon.obj" \
	"$(INTDIR)\dstimer.obj" \
	"..\..\ORA804\SQLLIB80.LIB"

"$(OUTDIR)\PTEDB.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ptedb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ptedb__0"
# PROP BASE Intermediate_Dir "ptedb__0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.\Debug

ALL : "$(OUTDIR)\PTEDB.lib"

CLEAN : 
	-@erase "$(INTDIR)\dbcommon.obj"
	-@erase "$(INTDIR)\dstimer.obj"
	-@erase "$(OUTDIR)\PTEDB.lib"
	-@erase ".\dbcommon.c"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTEDB.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTEDB.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dbcommon.obj" \
	"$(INTDIR)\dstimer.obj" \
	"..\..\ORA804\SQLLIB80.LIB"

"$(OUTDIR)\PTEDB.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

# Name "ptedb - Win32 Release"
# Name "ptedb - Win32 Debug"

!IF  "$(CFG)" == "ptedb - Win32 Release"

!ELSEIF  "$(CFG)" == "ptedb - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dbcommon.pc

!IF  "$(CFG)" == "ptedb - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\dbcommon.pc
InputName=dbcommon

"$(ProjDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics\
     userid=core/pinnacle@pinnacle

# End Custom Build

!ELSEIF  "$(CFG)" == "ptedb - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\dbcommon.pc
InputName=dbcommon

"$(ProjDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics\
     userid=core/pinnacle@pinnacle

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dstimer.c
DEP_CPP_DSTIM=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\dbcommon.h"\
	{$(INCLUDE)}"\dstimer.h"\
	{$(INCLUDE)}"\MEMDATA.H"\
	{$(INCLUDE)}"\MEMERR.H"\
	{$(INCLUDE)}"\MEMPUBD.H"\
	{$(INCLUDE)}"\MEMPUBP.H"\
	{$(INCLUDE)}"\MMCOS.H"\
	{$(INCLUDE)}"\MMOPSYS.H"\
	{$(INCLUDE)}"\MMXOS.H"\
	{$(INCLUDE)}"\MMXOSOFF.H"\
	{$(INCLUDE)}"\MOMERR.H"\
	{$(INCLUDE)}"\MOMPUBD.H"\
	{$(INCLUDE)}"\ntutils.h"\
	{$(INCLUDE)}"\pte.h"\
	{$(INCLUDE)}"\pteipc.h"\
	{$(INCLUDE)}"\ptellist.h"\
	{$(INCLUDE)}"\ptemsg.h"\
	{$(INCLUDE)}"\ptetimer.h"\
	{$(INCLUDE)}"\QUEERR.H"\
	{$(INCLUDE)}"\QUEPUBD.H"\
	{$(INCLUDE)}"\QUEPUBP.H"\
	{$(INCLUDE)}"\SEMERR.H"\
	{$(INCLUDE)}"\SEMPUBD.H"\
	{$(INCLUDE)}"\SEMPUBP.H"\
	{$(INCLUDE)}"\XIPC.H"\
	{$(INCLUDE)}"\XIPCASYN.H"\
	{$(INCLUDE)}"\XIPCERR.H"\
	{$(INCLUDE)}"\XIPCPUBD.H"\
	{$(INCLUDE)}"\XIPCPUBP.H"\
	{$(INCLUDE)}"\XIPCPUXD.H"\
	{$(INCLUDE)}"\XNETERR.H"\
	

"$(INTDIR)\dstimer.obj" : $(SOURCE) $(DEP_CPP_DSTIM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dbcommon.c
DEP_CPP_DBCOM=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\dbcommon.h"\
	{$(INCLUDE)}"\ntutils.h"\
	{$(INCLUDE)}"\ORACA.H"\
	{$(INCLUDE)}"\pte.h"\
	{$(INCLUDE)}"\ptemsg.h"\
	{$(INCLUDE)}"\ptetimer.h"\
	{$(INCLUDE)}"\SQLCA.H"\
	{$(INCLUDE)}"\SQLCPR.H"\
	{$(INCLUDE)}"\SQLPROTO.H"\
	

"$(INTDIR)\dbcommon.obj" : $(SOURCE) $(DEP_CPP_DBCOM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\Pinnacle\PTE\LIB\CORE\ORA804\SQLLIB80.LIB

!IF  "$(CFG)" == "ptedb - Win32 Release"

!ELSEIF  "$(CFG)" == "ptedb - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
