@echo off
REM RASC launcher 2024-08-05

setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

REM First parameter is (possibly non-existent) file containing RASC version to invoke
set "RascVersionFile=%~1"

REM RASC version handler script is located in the same directory as this launcher script
set "RascVersionHandler=%~dp0rasc_version.bat"

REM Shift to leave remaining parameters as input parameters to RASC
shift

REM Invoke rasc_version.bat to check rasc_version.txt and update it if required
REM If user selection of RASC version is required then the first non-interactive call will exit with error status
REM In that case we re-invoke in a new command shell to allow user interaction
call "%RascVersionHandler%" "%RascVersionFile%" NonInteractive || start /wait "Renesas" cmd /c ""%RascVersionHandler%" "%RascVersionFile%""
if errorlevel 1 exit /b 1

REM Extract specific RASC version from file
REM echo "%RascVersionFile%"
if exist "%RascVersionFile%" (

    REM echo DEBUG: Have version file: "%RascVersionFile%"

    set /a idx=0
    for /f "usebackq tokens=*" %%a in ("%RascVersionFile%") do (
        if !idx! EQU 2 (
            set "RascExe=%%a"
        )
        set /a idx+=1
    )
)

REM Synchronous behaviour for build pre/post steps
set "WaitRasc="
IF "%~3"=="--generate" SET CLI=true
IF "%~3"=="--gensmartbundle" SET CLI=true
IF "%CLI%"=="true" (
    SET "WaitRasc=/b /wait"
    SET RascExe=%RascExe:rasc.exe=rascc.exe%
)

set Parameters=
for %%a in (%*) do (
    if defined FirstParamSkipped set Parameters=!Parameters! %%a
    set FirstParamSkipped=true
)
REM echo DEBUG: Launching "%RascExe%" %Parameters%
start "" %WaitRasc% "%RascExe%" %Parameters%
if not errorlevel 1 goto :EOF
exit /b 1
