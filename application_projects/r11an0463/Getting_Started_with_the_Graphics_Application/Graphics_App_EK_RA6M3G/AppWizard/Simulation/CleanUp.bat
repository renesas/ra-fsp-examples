@echo off

cd %~dp0

set FOLDERS=Output, oD, oR, ipch, Exe, .vs
set FILES=OPT, PLG, APS, NCB, DEP, MAK, SUO, SDF, USER, depend, layout, html

echo CLEAN.BAT:    Deleting temporary files

attrib *.SUO -h

for %%i in (%FILES%) do if exist *.%%i (
  del *.%%i
)

if "%OS%" == "Windows_NT" goto WinNT

for %%i in (%FOLDERS%) do if exist %%i deltree /Y %%i

goto :eof

:WinNT

for %%i in (%FOLDERS%) do if exist %%i rd %%i /S/Q