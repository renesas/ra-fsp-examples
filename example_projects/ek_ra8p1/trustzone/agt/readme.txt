/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the functionality of AGT in periodic mode and one-shot mode. On providing
	any input on the RTT Viewer, AGT channel 0 starts in one-shot mode. AGT channel 1 starts in periodic mode when
	AGT channel 0 expires. Timer in periodic mode expires periodically at a time period specified by the user and
	toggles the on-board LED.

2. Software Requirements:
	Refer to the "Tools" section in the Release Notes to set up all the required software via link:
	https://github.com/renesas/fsp/releases

3. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

4. Verifying Operation:
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
1) The user is expected to enter data not exceeding 15 bytes in size.
   Operation is not guaranteed for any user input value other than integer, char (e.g., float, special char) through RTT.

2) The user should select "Input -> Sending -> Send on Enter" for RTT Viewer configuration.

3) The user should view "Example Project for IP Protection" section of application note R11AN0467 on how to build and run
   a trustzone project via link: https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33

4) The user should refer to the "RASC User Guide for MDK and IAR" section for instructions on using RASC for IAR and Keil
   via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-IAR-user-guide

5) For the IAR project, the user can launch the workspace using the .eww file in the non-secure project folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

6) For the KEIL project, the user can launch the workspace using the .uvmpw file in keil folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

7) For EK-RA8P1: When using SEGGER J-Flash Lite to flash the hex file of the non-secure project into the RA board,
   make sure to select the correct flash bank in the "Flash banks" section of the SEGGER J-Flash Lite by ticking
   the checkbox: 0x12000000 MRAM (Non-Secure) (Loader: Default).

8) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x32002450
   b. Keil:	0x32002850
   c. IAR: 0x320028dc

9) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
