# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=pteipc - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to pteipc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "pteipc - Win32 Release" && "$(CFG)" != "pteipc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "PTEIPC.MAK" CFG="pteipc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pteipc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pteipc - Win32 Debug" (based on "Win32 (x86) Static Library")
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
# PROP Target_Last_Scanned "pteipc - Win32 Release"
CPP=cl.exe

!IF  "$(CFG)" == "pteipc - Win32 Release"

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

ALL : "$(OUTDIR)\PTEIPC.lib"

CLEAN : 
	-@erase "$(INTDIR)\CDSAPI.OBJ"
	-@erase "$(INTDIR)\PTEIPC.OBJ"
	-@erase "$(OUTDIR)\PTEIPC.lib"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "XIPC" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "XIPC"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTEIPC.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTEIPC.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CDSAPI.OBJ" \
	"$(INTDIR)\PTEIPC.OBJ" \
	"..\..\XIPC302A\NXIPCTCP.LIB" \
	"..\..\XIPC302A\XIPC.LIB"

"$(OUTDIR)\PTEIPC.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pteipc - Win32 Debug"

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

ALL : "$(OUTDIR)\PTEIPC.lib"

CLEAN : 
	-@erase "$(INTDIR)\CDSAPI.OBJ"
	-@erase "$(INTDIR)\PTEIPC.OBJ"
	-@erase "$(OUTDIR)\PTEIPC.lib"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "XIPC" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MTd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "XIPC" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/PTEIPC.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/PTEIPC.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CDSAPI.OBJ" \
	"$(INTDIR)\PTEIPC.OBJ" \
	"..\..\XIPC302A\NXIPCTCP.LIB" \
	"..\..\XIPC302A\XIPC.LIB"

"$(OUTDIR)\PTEIPC.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

# Name "pteipc - Win32 Release"
# Name "pteipc - Win32 Debug"

!IF  "$(CFG)" == "pteipc - Win32 Release"

!ELSEIF  "$(CFG)" == "pteipc - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\PTEIPC.C
DEP_CPP_PTEIP=\
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
	{$(INCLUDE)}"\pteipc.h"\
	{$(INCLUDE)}"\ptemsg.h"\
	{$(INCLUDE)}"\ptetime.h"\
	{$(INCLUDE)}"\queerr.h"\
	{$(INCLUDE)}"\quepubd.h"\
	{$(INCLUDE)}"\quepubp.h"\
	{$(INCLUDE)}"\semerr.h"\
	{$(INCLUDE)}"\sempubd.h"\
	{$(INCLUDE)}"\sempubp.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\xipc.h"\
	{$(INCLUDE)}"\xipcasyn.h"\
	{$(INCLUDE)}"\xipcerr.h"\
	{$(INCLUDE)}"\xipcpubd.h"\
	{$(INCLUDE)}"\xipcpubp.h"\
	{$(INCLUDE)}"\xipcpuxd.h"\
	{$(INCLUDE)}"\xneterr.h"\
	

"$(INTDIR)\PTEIPC.OBJ" : $(SOURCE) $(DEP_CPP_PTEIP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\Pinnacle\PTE\LIB\CORE\XIPC302A\XIPC.LIB

!IF  "$(CFG)" == "pteipc - Win32 Release"

!ELSEIF  "$(CFG)" == "pteipc - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Pinnacle\PTE\LIB\CORE\XIPC302A\NXIPCTCP.LIB

!IF  "$(CFG)" == "pteipc - Win32 Release"

!ELSEIF  "$(CFG)" == "pteipc - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CDSAPI.C
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
	{$(INCLUDE)}"\ptemsg.h"\
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
	

!IF  "$(CFG)" == "pteipc - Win32 Release"


"$(INTDIR)\CDSAPI.OBJ" : $(SOURCE) $(DEP_CPP_CDSAP) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "XIPC" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "pteipc - Win32 Debug"


"$(INTDIR)\CDSAPI.OBJ" : $(SOURCE) $(DEP_CPP_CDSAP) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "XIPC" /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
