/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the RTC HAL module APIs. 
	The project initializes the RTC module and allows the user to set RTC Calendar time and Calendar alarm.
	Users can enable periodic interrupt and can also view the current RTC calendar time.
        On enabling periodic interrupt, on-board LED toggles every 1 second and on occurrence of Calendar alarm. LED is turned ON.

2. Software Requirements:
	Refer to the Tools section in the Release Notes for set up suitable Software version.
	https://github.com/renesas/fsp/releases

Note:
1) The user is expected to enter Time values to set Calendar time and Calendar alarm in 24-hours format.
   Sample Input: 23:10:2019 16:14:55 (DD:MM:YYYY HH:MM:SS)

2) The user is expected to enter data of size not exceeding above 15 bytes.

3) Operation is not guaranteed for any user input value other than integer, char (i.e. float, special char) through RTT.

4) The user should view section 5 of application note R11AN0467 on how to build and run a trustzone project.
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

5) For the IAR project, the user can launch the Workspace using the .eww workspace file in the non-secure project folder. 
   It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

6) For the KEIL project, the user can launch the Workspace using the .uvmpw file in keil folder. It included multiple workspace 
   which has both secure (_s) and non-secure (_ns) project.

7) The user must build secure project (_s) first, then build non-secure (_ns) project. 

8) After the projects are built successfully, the user should follow these steps in sequence to launch the debug session:
   - Open Renesas Device Partition Manager, then choose "Connection Type" as SWD.
   - Initialize device back to factory default. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Set Trustzone secure / non-secure boundaries. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Launch the Debug session from the non-secure project.

9) For IAR project, the user can change setting in the Tools -> Option -> Stack -> Uncheck "Stack pointer(s) not
   valid until program reaches" option to ignore Stack Warning at launch. 
﻿
10) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
    RTT Block address for hex file committed in repository are as follows:
    a. e2studio: 0x200024ac
    b. Keil: 	 0x20002008
    c. IAR : 	 0x200020e8
 
11) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
    in .map file generated in the build configuration folder (Debug/Release).
