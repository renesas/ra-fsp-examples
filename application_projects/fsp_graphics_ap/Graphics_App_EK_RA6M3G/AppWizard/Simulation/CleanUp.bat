@ECHO OFF

SET FOLDERS=Output, oD, oR, ipch, Exe
SET FILES=*.OPT, *.PLG, *.APS, *.NCB, *.DEP, *.MAK, *.SDF, *.SUO, *.USER, *.depend, *.layout, SimulationTrial_build_log.html

ECHO CLEAN.BAT:    Deleting temporary files

FOR %%i IN (%FILES%) DO IF EXIST %%i (
  ATTRIB %%i -h
  DEL %%i
  ECHO %%i
)

IF "%OS%" == "Windows_NT" GOTO WinNT

FOR %%i IN (%FOLDERS%) DO IF EXIST %%i DELTREE /Y %%i

GOTO READY

:WinNT

FOR %%i IN (%FOLDERS%) DO IF EXIST %%i RD %%i /S/Q

:READY
