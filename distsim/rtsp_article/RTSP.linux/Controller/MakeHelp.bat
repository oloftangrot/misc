@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by CONTROLLER.HPJ. >"hlp\Controller.hm"
echo. >>"hlp\Controller.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\Controller.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\Controller.hm"
echo. >>"hlp\Controller.hm"
echo // Prompts (IDP_*) >>"hlp\Controller.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\Controller.hm"
echo. >>"hlp\Controller.hm"
echo // Resources (IDR_*) >>"hlp\Controller.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\Controller.hm"
echo. >>"hlp\Controller.hm"
echo // Dialogs (IDD_*) >>"hlp\Controller.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\Controller.hm"
echo. >>"hlp\Controller.hm"
echo // Frame Controls (IDW_*) >>"hlp\Controller.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\Controller.hm"
REM -- Make help for Project CONTROLLER


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\Controller.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\Controller.hlp" goto :Error
if not exist "hlp\Controller.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\Controller.hlp" Debug
if exist Debug\nul copy "hlp\Controller.cnt" Debug
if exist Release\nul copy "hlp\Controller.hlp" Release
if exist Release\nul copy "hlp\Controller.cnt" Release
echo.
goto :done

:Error
echo hlp\Controller.hpj(1) : error: Problem encountered creating help file

:done
echo.
