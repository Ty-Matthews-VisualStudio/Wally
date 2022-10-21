@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by WALLY.HPJ. >"hlp\Wally.hm"
echo. >>"hlp\Wally.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\Wally.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\Wally.hm"
echo. >>"hlp\Wally.hm"
echo // Prompts (IDP_*) >>"hlp\Wally.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\Wally.hm"
echo. >>"hlp\Wally.hm"
echo // Resources (IDR_*) >>"hlp\Wally.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\Wally.hm"
echo. >>"hlp\Wally.hm"
echo // Dialogs (IDD_*) >>"hlp\Wally.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\Wally.hm"
echo. >>"hlp\Wally.hm"
echo // Frame Controls (IDW_*) >>"hlp\Wally.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\Wally.hm"
REM -- Make help for Project WALLY


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\Wally.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\Wally.hlp" goto :Error
if not exist "hlp\Wally.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\Wally.hlp" Debug
if exist Debug\nul copy "hlp\Wally.cnt" Debug
if exist Release\nul copy "hlp\Wally.hlp" Release
if exist Release\nul copy "hlp\Wally.cnt" Release
echo.
goto :done

:Error
echo hlp\Wally.hpj(1) : error: Problem encountered creating help file

:done
echo.
