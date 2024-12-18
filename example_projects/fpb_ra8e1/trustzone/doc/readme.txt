/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the DOC HAL module APIs. The project performs the different event 
        operations selected by the user in configurator properties. Events can be selected viz., Compare Match, Compare Mismatch, 
        Addition Overflow, Subtraction Underflow and compares with the reference data. Upon Success/Failure of operation, the 
        result will be displayed on RTT Viewer. LED will turn ON only if the selected event operation is success.

2. Software Requirements:
	Refer to the Tools section in the Release Notes for set up suitable Software version.
	https://github.com/renesas/fsp/releases

Note:﻿
1) User should view section 5 of application note R11AN0467 on how to build and run a trustzone project.
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

2) For the IAR project, user can launch the Workspace using the .eww workspace file in the non-secure project folder. 
   It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

3) For the KEIL project, user can launch the Workspace using the .uvmpw file in keil folder. It included multiple workspace 
   which has both secure (_s) and non-secure (_ns) project.

4) User must build secure project (_s) first, then build non-secure (_ns) project.

5) After the projects are built successfully, user should follow these three steps in sequence to launch the debug session:
   - Initialize device back to factory default. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Set Trustzone secure / non-secure boundaries. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Launch the Debug session from the non-secure project.

6) For IAR project, User can change setting in the Tools -> Option -> Stack -> Uncheck "Stack pointer(s) not
   valid until program reaches" option to ignore Stack Warning at launch. 

7) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x32062040
   b. Keil:	0x32062004
   c. IAR: 0x3206205c
 
8) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
