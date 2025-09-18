@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by ATP.HPJ. >"hlp\Atp.hm"
echo. >>"hlp\Atp.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\Atp.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\Atp.hm"
echo. >>"hlp\Atp.hm"
echo // Prompts (IDP_*) >>"hlp\Atp.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\Atp.hm"
echo. >>"hlp\Atp.hm"
echo // Resources (IDR_*) >>"hlp\Atp.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\Atp.hm"
echo. >>"hlp\Atp.hm"
echo // Dialogs (IDD_*) >>"hlp\Atp.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\Atp.hm"
echo. >>"hlp\Atp.hm"
echo // Frame Controls (IDW_*) >>"hlp\Atp.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\Atp.hm"
REM -- Make help for Project ATP


echo Building Win32 Help files
start /wait hcrtf -x "hlp\Atp.hpj"
echo.
if exist Debug\nul copy "hlp\Atp.hlp" Debug
if exist Debug\nul copy "hlp\Atp.cnt" Debug
if exist Release\nul copy "hlp\Atp.hlp" Release
if exist Release\nul copy "hlp\Atp.cnt" Release
echo.


