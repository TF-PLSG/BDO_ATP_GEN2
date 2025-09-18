# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=PTE - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to PTE - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "PTE - Win32 Release" && "$(CFG)" != "PTE - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "PTE.MAK" CFG="PTE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PTE - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "PTE - Win32 Debug" (based on "Win32 (x86) Static Library")
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
# PROP Target_Last_Scanned "PTE - Win32 Release"
CPP=cl.exe

!IF  "$(CFG)" == "PTE - Win32 Release"

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

ALL : "$(OUTDIR)\PTE.lib"

CLEAN : 
	-@erase "$(INTDIR)\des.obj"
	-@erase "$(INTDIR)\LOGGER.OBJ"
	-@erase "$(INTDIR)\NTUTILS.OBJ"
	-@erase "$(INTDIR)\odbclogin.obj"
	-@erase "$(INTDIR)\PTECOPY.OBJ"
	-@erase "$(INTDIR)\ptedes.obj"
	-@erase "$(INTDIR)\pteinit.obj"
	-@erase "$(INTDIR)\ptellist.obj"
	-@erase "$(INTDIR)\PTEMSG.OBJ"
	-@erase "$(INTDIR)\PTESTATS.OBJ"
	-@erase "$(INTDIR)\ptesystm.obj"
	-@erase "$(INTDIR)\ptetime.obj"
	-@erase "$(INTDIR)\PTETIMER.OBJ"
	-@erase "$(INTDIR)\STARTUP.OBJ"
	-@erase "$(OUTDIR)\PTE.lib"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTE.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTE.lib" 
LIB32_OBJS= \
	"$(INTDIR)\des.obj" \
	"$(INTDIR)\LOGGER.OBJ" \
	"$(INTDIR)\NTUTILS.OBJ" \
	"$(INTDIR)\odbclogin.obj" \
	"$(INTDIR)\PTECOPY.OBJ" \
	"$(INTDIR)\ptedes.obj" \
	"$(INTDIR)\pteinit.obj" \
	"$(INTDIR)\ptellist.obj" \
	"$(INTDIR)\PTEMSG.OBJ" \
	"$(INTDIR)\PTESTATS.OBJ" \
	"$(INTDIR)\ptesystm.obj" \
	"$(INTDIR)\ptetime.obj" \
	"$(INTDIR)\PTETIMER.OBJ" \
	"$(INTDIR)\STARTUP.OBJ"

"$(OUTDIR)\PTE.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

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

ALL : "$(OUTDIR)\PTE.lib"

CLEAN : 
	-@erase "$(INTDIR)\des.obj"
	-@erase "$(INTDIR)\LOGGER.OBJ"
	-@erase "$(INTDIR)\NTUTILS.OBJ"
	-@erase "$(INTDIR)\odbclogin.obj"
	-@erase "$(INTDIR)\PTECOPY.OBJ"
	-@erase "$(INTDIR)\ptedes.obj"
	-@erase "$(INTDIR)\pteinit.obj"
	-@erase "$(INTDIR)\ptellist.obj"
	-@erase "$(INTDIR)\PTEMSG.OBJ"
	-@erase "$(INTDIR)\PTESTATS.OBJ"
	-@erase "$(INTDIR)\ptesystm.obj"
	-@erase "$(INTDIR)\ptetime.obj"
	-@erase "$(INTDIR)\PTETIMER.OBJ"
	-@erase "$(INTDIR)\STARTUP.OBJ"
	-@erase "$(OUTDIR)\PTE.lib"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTE.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTE.lib" 
LIB32_OBJS= \
	"$(INTDIR)\des.obj" \
	"$(INTDIR)\LOGGER.OBJ" \
	"$(INTDIR)\NTUTILS.OBJ" \
	"$(INTDIR)\odbclogin.obj" \
	"$(INTDIR)\PTECOPY.OBJ" \
	"$(INTDIR)\ptedes.obj" \
	"$(INTDIR)\pteinit.obj" \
	"$(INTDIR)\ptellist.obj" \
	"$(INTDIR)\PTEMSG.OBJ" \
	"$(INTDIR)\PTESTATS.OBJ" \
	"$(INTDIR)\ptesystm.obj" \
	"$(INTDIR)\ptetime.obj" \
	"$(INTDIR)\PTETIMER.OBJ" \
	"$(INTDIR)\STARTUP.OBJ"

"$(OUTDIR)\PTE.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

# Name "PTE - Win32 Release"
# Name "PTE - Win32 Debug"

!IF  "$(CFG)" == "PTE - Win32 Release"

!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ptetime.c

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_PTETI=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\sys\TIMEB.H"\
	

"$(INTDIR)\ptetime.obj" : $(SOURCE) $(DEP_CPP_PTETI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_PTETI=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\sys\TIMEB.H"\
	

"$(INTDIR)\ptetime.obj" : $(SOURCE) $(DEP_CPP_PTETI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PTECOPY.C

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_PTECO=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\ptecopy.h"\
	

"$(INTDIR)\PTECOPY.OBJ" : $(SOURCE) $(DEP_CPP_PTECO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_PTECO=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\PTECOPY.H"\
	

"$(INTDIR)\PTECOPY.OBJ" : $(SOURCE) $(DEP_CPP_PTECO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ptedes.c

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_PTEDE=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\des.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\ptedes.obj" : $(SOURCE) $(DEP_CPP_PTEDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_PTEDE=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\DES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\ptedes.obj" : $(SOURCE) $(DEP_CPP_PTEDE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pteinit.c

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_PTEIN=\
	{$(INCLUDE)}"\basictyp.h"\
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
	{$(INCLUDE)}"\ntutils.h"\
	{$(INCLUDE)}"\pte.h"\
	{$(INCLUDE)}"\pteinit.h"\
	{$(INCLUDE)}"\pteipc.h"\
	{$(INCLUDE)}"\ptemsg.h"\
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
	

"$(INTDIR)\pteinit.obj" : $(SOURCE) $(DEP_CPP_PTEIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_PTEIN=\
	{$(INCLUDE)}"\BASICTYP.H"\
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
	{$(INCLUDE)}"\PTEINIT.H"\
	{$(INCLUDE)}"\PTEIPC.H"\
	{$(INCLUDE)}"\PTEMSG.H"\
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
	

"$(INTDIR)\pteinit.obj" : $(SOURCE) $(DEP_CPP_PTEIN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ptellist.c

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_PTELL=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\ptellist.h"\
	

"$(INTDIR)\ptellist.obj" : $(SOURCE) $(DEP_CPP_PTELL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_PTELL=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\PTELLIST.H"\
	

"$(INTDIR)\ptellist.obj" : $(SOURCE) $(DEP_CPP_PTELL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PTEMSG.C

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_PTEMS=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\ntutils.h"\
	{$(INCLUDE)}"\pte.h"\
	{$(INCLUDE)}"\ptecopy.h"\
	{$(INCLUDE)}"\ptemsg.h"\
	

"$(INTDIR)\PTEMSG.OBJ" : $(SOURCE) $(DEP_CPP_PTEMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_PTEMS=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\NTUTILS.H"\
	{$(INCLUDE)}"\PTE.H"\
	{$(INCLUDE)}"\PTECOPY.H"\
	{$(INCLUDE)}"\PTEMSG.H"\
	

"$(INTDIR)\PTEMSG.OBJ" : $(SOURCE) $(DEP_CPP_PTEMS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ptesystm.c

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_PTESY=\
	{$(INCLUDE)}"\basictyp.h"\
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
	{$(INCLUDE)}"\ptemsg.h"\
	{$(INCLUDE)}"\ptesystm.h"\
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
	

"$(INTDIR)\ptesystm.obj" : $(SOURCE) $(DEP_CPP_PTESY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_PTESY=\
	{$(INCLUDE)}"\BASICTYP.H"\
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
	{$(INCLUDE)}"\PTE.H"\
	{$(INCLUDE)}"\PTEIPC.H"\
	{$(INCLUDE)}"\PTEMSG.H"\
	{$(INCLUDE)}"\PTESYSTM.H"\
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
	

"$(INTDIR)\ptesystm.obj" : $(SOURCE) $(DEP_CPP_PTESY) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\des.c

"$(INTDIR)\des.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\NTUTILS.C

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_NTUTI=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\ntutils.h"\
	

"$(INTDIR)\NTUTILS.OBJ" : $(SOURCE) $(DEP_CPP_NTUTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_NTUTI=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\NTUTILS.H"\
	

"$(INTDIR)\NTUTILS.OBJ" : $(SOURCE) $(DEP_CPP_NTUTI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PTESTATS.C

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_PTEST=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\pte.h"\
	{$(INCLUDE)}"\ptemsg.h"\
	{$(INCLUDE)}"\ptestats.h"\
	

"$(INTDIR)\PTESTATS.OBJ" : $(SOURCE) $(DEP_CPP_PTEST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_PTEST=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\PTE.H"\
	{$(INCLUDE)}"\PTEMSG.H"\
	{$(INCLUDE)}"\PTESTATS.H"\
	

"$(INTDIR)\PTESTATS.OBJ" : $(SOURCE) $(DEP_CPP_PTEST) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PTETIMER.C

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_PTETIM=\
	{$(INCLUDE)}"\basictyp.h"\
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
	{$(INCLUDE)}"\ptemsg.h"\
	{$(INCLUDE)}"\ptetimer.h"\
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
	

"$(INTDIR)\PTETIMER.OBJ" : $(SOURCE) $(DEP_CPP_PTETIM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_PTETIM=\
	{$(INCLUDE)}"\BASICTYP.H"\
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
	{$(INCLUDE)}"\PTE.H"\
	{$(INCLUDE)}"\PTEIPC.H"\
	{$(INCLUDE)}"\PTEMSG.H"\
	{$(INCLUDE)}"\PTETIMER.H"\
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
	

"$(INTDIR)\PTETIMER.OBJ" : $(SOURCE) $(DEP_CPP_PTETIM) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LOGGER.C

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_LOGGE=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\logger.h"\
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
	{$(INCLUDE)}"\ntutils.h"\
	{$(INCLUDE)}"\pte.h"\
	{$(INCLUDE)}"\pteipc.h"\
	{$(INCLUDE)}"\ptemsg.h"\
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
	

"$(INTDIR)\LOGGER.OBJ" : $(SOURCE) $(DEP_CPP_LOGGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_LOGGE=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\LOGGER.H"\
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
	{$(INCLUDE)}"\XIPC.H"\
	{$(INCLUDE)}"\XIPCASYN.H"\
	{$(INCLUDE)}"\XIPCERR.H"\
	{$(INCLUDE)}"\XIPCPUBD.H"\
	{$(INCLUDE)}"\XIPCPUBP.H"\
	{$(INCLUDE)}"\XIPCPUXD.H"\
	{$(INCLUDE)}"\XNETERR.H"\
	

"$(INTDIR)\LOGGER.OBJ" : $(SOURCE) $(DEP_CPP_LOGGE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\odbclogin.cpp

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_ODBCL=\
	{$(INCLUDE)}"\odbclogin.h"\
	

"$(INTDIR)\odbclogin.obj" : $(SOURCE) $(DEP_CPP_ODBCL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_ODBCL=\
	{$(INCLUDE)}"\odbclogin.h"\
	

"$(INTDIR)\odbclogin.obj" : $(SOURCE) $(DEP_CPP_ODBCL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STARTUP.C

!IF  "$(CFG)" == "PTE - Win32 Release"

DEP_CPP_START=\
	{$(INCLUDE)}"\basictyp.h"\
	{$(INCLUDE)}"\ntutils.h"\
	

"$(INTDIR)\STARTUP.OBJ" : $(SOURCE) $(DEP_CPP_START) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PTE - Win32 Debug"

DEP_CPP_START=\
	{$(INCLUDE)}"\BASICTYP.H"\
	{$(INCLUDE)}"\NTUTILS.H"\
	

"$(INTDIR)\STARTUP.OBJ" : $(SOURCE) $(DEP_CPP_START) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
