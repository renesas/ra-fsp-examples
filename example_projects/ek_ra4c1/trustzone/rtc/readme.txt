/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the RTC HAL module APIs. 
	The project initializes the RTC module and allows the user to set RTC Calendar time and Calendar alarm.
	Users can enable periodic interrupt and can also view the current RTC calendar time.
	On enabling periodic interrupt, on-board LED toggles every 1 second and on occurrence of Calendar alarm.
	LED is turned ON.

2. Software Requirements:
	Refer to the "Tools" section in the FSP release notes to set up all the required software via the link below:
	https://github.com/renesas/fsp/releases

3. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

4. Hardware Connections:
	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

5. Verifying Operation:
	a) Import and build projects: the user must build secure project (_s) first, then build non-secure (_ns) project.
	b) After the projects are built successfully, the user should follow these steps in sequence to launch the debug
	   session:
		- Open Renesas Device Partition Manager (RDPM): In e2studio or IAR and Keil using RASC -> Run ->
		  Renesas Debug Tools -> Renesas Device Partition Manager, then choose "Connection Type" as SWD.
		- Initialize the MCU: On RDPM, select only "Initialize device" option, then click "Run". If successful,
		  go to the next steps.
   		- Set Trustzone secure/ non-secure boundaries: On RDPM, uncheck "Initialize device", select only
		  "Set Trustzone secure/ non-secure boundaries" -> Select "Use Renesas Partition Data file" -> 
		  Use "Browse" to select the .rpd file generated from the secure project (For "e2studio":
		  e2studio\secure_project\Debug, for "Keil": keil\secure_project\Objects, and for "IAR":
		  iar\secure_project\Debug\Exe), then click "Run". If successful, go to the next steps.
		- Only setting used for IAR project: The user can change setting in the Tools -> Option -> Stack ->
		  Uncheck "Stack pointer(s) not valid until program reaches" option to ignore Stack Warning at launch.
   		- Launch the debug session from the non-secure project.
	c) Open J-Link RTT Viewer to view the example project operation.

Note:
1) The user is expected to Set RTC Calendar Date and Time (option 1) before other runnings.

2) The user is expected to enter Time values to set Calendar time and Calendar alarm in 24-hours format.
   Sample Input: 23:10:2019 16:14:55 (DD:MM:YYYY HH:MM:SS)

3) The user is expected to enter data not exceeding 32 bytes in size.

4) Operation is not guaranteed for any user input value other than integer, char (e.g., float, special char) through RTT.

5) The user should view "Example Project for IP Protection" section of application note R11AN0467 on how to build and run
   a trustzone project via link: https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33

6) The user should refer to the "RASC User Guide for MDK and IAR" section for instructions on using RASC for IAR and Keil
   via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-IAR-user-guide

7) For the IAR project, the user can launch the workspace using the .eww file in the non-secure project folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

8) For the KEIL project, the user can launch the workspace using the .uvmpw file in keil folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

9) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200028b4
   b. Keil:     0x2000283c
   c. IAR:      0x20002900
 
10) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
    in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
