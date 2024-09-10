REM script to download application and setup FAW
echo off

set "ERRORLEVEL="


programinstaller.exe %1 secure_data_at_rest_ek_ra6m3.srec
if %ERRORLEVEL% neq 0 goto failure

:success
set ERRORLEVEL=0
exit /b [0]

:failure
set ERRORLEVEL=1
echo FAILED
exit /b [1]
