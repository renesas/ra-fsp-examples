/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of USB PHID driver on Renesas RA MCUs based on Renesas FSP.
	The project configures RA board as a Keyboard. On pressing any key from RTTViewer, MCU sends the host PC 
	characters from a - z & numbers 0 - 9.

2. Software Requirements:
	Refer to the Tools section in the Release Notes for set up suitable Software version.
	https://github.com/renesas/fsp/releases

3. Hardware Requirement:
	micro USB Cable : 1 nos
	
4. Hardware connection :
	EK-RA6M4,EK-RA4M3,EK-RA4M2,EK-RA6M5,EK-RA8M1,EK-RA8D1:(support only full speed)
	  Jumper J12 placement is pins 2-3
	  Connect jumper J15 pins 
	  Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	  port (J11) of the board.  Connect the other end of this cable to USB port of the 
	  host PC. 

	EK-RA6E2: (support only full speed)
 	  Connect the micro USB end of the micro USB cable to micro-AB USB Full Speed
	  port (J11) of the board.  Connect the other end of this cable to USB port of the
	  host PC. 

Note:
1) Open Device Manager, and check 'Human Interface Devices', to see ' USB Input Device' has been detected.

2) Lower case a - z and numbers 0 - 9 is send from MCU to the host PC.﻿

3) User should view section 5 of application note R11AN0467 on how to build and run a trustzone project.
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

4) For the IAR project, user can launch the Workspace using the .eww workspace file in the non-secure project folder. 
   It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

5) For the KEIL project, user can launch the Workspace using the .uvmpw file in keil folder. It included multiple workspace 
   which has both secure (_s) and non-secure (_ns) project.

6) User must build secure project (_s) first, then build non-secure (_ns) project.

7) After the projects are built successfully, user should follow these three steps in sequence to launch the debug session:
   - Initialize device back to factory default. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Set Trustzone secure / non-secure boundaries. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Launch the Debug session from the non-secure project.

8) For IAR project, User can change setting in the Tools -> Option -> Stack -> Uncheck "Stack pointer(s) not
   valid until program reaches" option to ignore Stack Warning at launch. 

9) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x32002a00
   b. Keil:	0x320035b0
   c. IAR: 0x32006f38
 
10) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
    in .map file generated in the build configuration folder (Debug/Release).
