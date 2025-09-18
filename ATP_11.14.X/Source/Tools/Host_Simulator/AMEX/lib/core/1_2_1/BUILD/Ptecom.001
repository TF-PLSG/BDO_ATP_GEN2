# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=ptecom - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ptecom - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ptecom - Win32 Release" && "$(CFG)" != "ptecom - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "PTECOM.MAK" CFG="ptecom - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ptecom - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ptecom - Win32 Debug" (based on "Win32 (x86) Static Library")
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
# PROP Target_Last_Scanned "ptecom - Win32 Release"
CPP=cl.exe

!IF  "$(CFG)" == "ptecom - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.\Release

ALL : "$(OUTDIR)\PTECOM.lib"

CLEAN : 
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\convert.obj"
	-@erase "$(INTDIR)\notify.obj"
	-@erase "$(INTDIR)\serial.obj"
	-@erase "$(INTDIR)\tcp.obj"
	-@erase "$(OUTDIR)\PTECOM.lib"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /WX /YX
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTECOM.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTECOM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\convert.obj" \
	"$(INTDIR)\notify.obj" \
	"$(INTDIR)\serial.obj" \
	"$(INTDIR)\tcp.obj"

"$(OUTDIR)\PTECOM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ptecom - Win32 Debug"

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

ALL : "$(OUTDIR)\PTECOM.lib"

CLEAN : 
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\convert.obj"
	-@erase "$(INTDIR)\notify.obj"
	-@erase "$(INTDIR)\serial.obj"
	-@erase "$(INTDIR)\tcp.obj"
	-@erase "$(OUTDIR)\PTECOM.lib"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /WX /YX
CPP_PROJ=/nologo /MTd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTECOM.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTECOM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\convert.obj" \
	"$(INTDIR)\notify.obj" \
	"$(INTDIR)\serial.obj" \
	"$(INTDIR)\tcp.obj"

"$(OUTDIR)\PTECOM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

# Name "ptecom - Win32 Release"
# Name "ptecom - Win32 Debug"

!IF  "$(CFG)" == "ptecom - Win32 Release"

!ELSEIF  "$(CFG)" == "ptecom - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\tcp.c
DEP_CPP_TCP_C=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\common.h"\
	{$(INCLUDE)}"\NTUTILS.H"\
	{$(INCLUDE)}"\tcp.h"\
	

"$(INTDIR)\tcp.obj" : $(SOURCE) $(DEP_CPP_TCP_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\serial.c
DEP_CPP_SERIA=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\common.h"\
	{$(INCLUDE)}"\NTUTILS.H"\
	{$(INCLUDE)}"\serial.h"\
	

"$(INTDIR)\serial.obj" : $(SOURCE) $(DEP_CPP_SERIA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\common.c
DEP_CPP_COMMO=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\common.h"\
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
	{$(INCLUDE)}"\NTUTILS.H"\
	{$(INCLUDE)}"\PTE.H"\
	{$(INCLUDE)}"\PTEIPC.H"\
	{$(INCLUDE)}"\PTEMSG.H"\
	{$(INCLUDE)}"\QUEERR.H"\
	{$(INCLUDE)}"\QUEPUBD.H"\
	{$(INCLUDE)}"\QUEPUBP.H"\
	{$(INCLUDE)}"\SEMERR.H"\
	{$(INCLUDE)}"\SEMPUBD.H"\
	{$(INCLUDE)}"\SEMPUBP.H"\
	{$(INCLUDE)}"\serial.h"\
	{$(INCLUDE)}"\tcp.h"\
	{$(INCLUDE)}"\XIPC.H"\
	{$(INCLUDE)}"\XIPCASYN.H"\
	{$(INCLUDE)}"\XIPCERR.H"\
	{$(INCLUDE)}"\XIPCPUBD.H"\
	{$(INCLUDE)}"\XIPCPUBP.H"\
	{$(INCLUDE)}"\XIPCPUXD.H"\
	{$(INCLUDE)}"\XNETERR.H"\
	

"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\convert.c
DEP_CPP_CONVE=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\common.h"\
	{$(INCLUDE)}"\convert.h"\
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
	{$(INCLUDE)}"\notify.h"\
	{$(INCLUDE)}"\NTUTILS.H"\
	{$(INCLUDE)}"\PTE.H"\
	{$(INCLUDE)}"\ptecom.h"\
	{$(INCLUDE)}"\PTEIPC.H"\
	{$(INCLUDE)}"\PTEMSG.H"\
	{$(INCLUDE)}"\PTETIME.H"\
	{$(INCLUDE)}"\QUEERR.H"\
	{$(INCLUDE)}"\QUEPUBD.H"\
	{$(INCLUDE)}"\QUEPUBP.H"\
	{$(INCLUDE)}"\SEMERR.H"\
	{$(INCLUDE)}"\SEMPUBD.H"\
	{$(INCLUDE)}"\SEMPUBP.H"\
	{$(INCLUDE)}"\serial.h"\
	{$(INCLUDE)}"\tcp.h"\
	{$(INCLUDE)}"\XIPC.H"\
	{$(INCLUDE)}"\XIPCASYN.H"\
	{$(INCLUDE)}"\XIPCERR.H"\
	{$(INCLUDE)}"\XIPCPUBD.H"\
	{$(INCLUDE)}"\XIPCPUBP.H"\
	{$(INCLUDE)}"\XIPCPUXD.H"\
	{$(INCLUDE)}"\XNETERR.H"\
	

"$(INTDIR)\convert.obj" : $(SOURCE) $(DEP_CPP_CONVE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\notify.c
DEP_CPP_NOTIF=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\common.h"\
	{$(INCLUDE)}"\convert.h"\
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
	{$(INCLUDE)}"\NTUTILS.H"\
	{$(INCLUDE)}"\PTE.H"\
	{$(INCLUDE)}"\ptecom.h"\
	{$(INCLUDE)}"\PTEIPC.H"\
	{$(INCLUDE)}"\PTEMSG.H"\
	{$(INCLUDE)}"\PTETIME.H"\
	{$(INCLUDE)}"\QUEERR.H"\
	{$(INCLUDE)}"\QUEPUBD.H"\
	{$(INCLUDE)}"\QUEPUBP.H"\
	{$(INCLUDE)}"\SEMERR.H"\
	{$(INCLUDE)}"\SEMPUBD.H"\
	{$(INCLUDE)}"\SEMPUBP.H"\
	{$(INCLUDE)}"\serial.h"\
	{$(INCLUDE)}"\tcp.h"\
	{$(INCLUDE)}"\XIPC.H"\
	{$(INCLUDE)}"\XIPCASYN.H"\
	{$(INCLUDE)}"\XIPCERR.H"\
	{$(INCLUDE)}"\XIPCPUBD.H"\
	{$(INCLUDE)}"\XIPCPUBP.H"\
	{$(INCLUDE)}"\XIPCPUXD.H"\
	{$(INCLUDE)}"\XNETERR.H"\
	

"$(INTDIR)\notify.obj" : $(SOURCE) $(DEP_CPP_NOTIF) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
