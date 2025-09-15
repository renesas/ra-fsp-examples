/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of USB PHID driver on Renesas RA MCUs based on Renesas FSP.
	The project configures RA board as a Keyboard. On pressing any key from J-Link RTT Viewer, MCU sends characters
	from a - z & numbers 0 - 9 to the host PC.

2. Software Requirements:
	Refer to the "Tools" section in the Release Notes to set up all the required software via link:
	https://github.com/renesas/fsp/releases

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA4E2, EK-RA6M4, EK-RA6M5, EK-RA6E2, EK-RA8M1, EK-RA8D1, MCK-RA8T1,
			     EK-RA4L1, EK-RA8E2, EK-RA8P1
	1 x Renesas RA board.
	2 x Type-C USB cables.
	1 x PC with at least 2 USB ports (1 for debug and 1 for COM port).
	
4. Hardware Connections:
	Connect the RA board USB debug port to the PC via a USB cable for EP debugging.

	For EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full Speed):
		- Jumper J12 placement is pins 2-3.
	  	- Connect jumper J15 pins.
		- Connect the RA board USB FS port (J11) to the PC via a micro USB cable.

	For EK-RA4E2, EK-RA6E2 (Full Speed):
 	  	- Connect the RA board USB FS port (J11) to the PC via a micro USB cable.

	For MCK-RA8T1 (Full Speed):
	  	- Jumper JP9 placement is pins 2-3.
	  	- Connect jumper JP10 pins.
		- Connect the RA board USB FS port (CN14) to the PC via a Type-C USB cable.

	For EK-RA4L1 (Full Speed):
		- Turn ON S4-4 to select USB device mode.
		- Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		- Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.

	For EK-RA8E2, EK-RA8P1 (Full Speed):
		- Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.

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
		  Use "Browse" to select the .rpd file generated from the secure project (For "e2studio": e2studio\secure_project\Debug,
		  for "Keil": keil\secure_project\Objects, and for "IAR": iar\secure_project\Debug\Exe), then click "Run". If successful,
		  go to the next steps.
		- Only setting used for IAR project: The user can change setting in the Tools -> Option -> Stack ->
		  Uncheck "Stack pointer(s) not valid until program reaches" option to ignore Stack Warning at launch.
   		- Launch the debug session from the non-secure project.
	c) Open J-Link RTT Viewer to view the example project operation.

Note:
1) Open "Device Manager", and check "Human Interface Devices", to see "USB Input Device" has been detected.

2) Lower case a - z and numbers 0 - 9 are sent from the MCU to the host PC.

3) The user is expected to enter data not exceeding 15 bytes in size.

4) The user should view "Example Project for IP Protection" section of application note R11AN0467 on how to build and run
   a trustzone project via link: https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33

5) The user should refer to the "RASC User Guide for MDK and IAR" section for instructions on using RASC for IAR and Keil
   via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-IAR-user-guide

6) For the IAR project, the user can launch the workspace using the .eww file in the non-secure project folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

7) For the KEIL project, the user can launch the workspace using the .uvmpw file in keil folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

8) For EK-RA8P1: When using SEGGER J-Flash Lite to flash the hex file of the non-secure project into the RA board,
   make sure to select the correct flash bank in the "Flash banks" section of the SEGGER J-Flash Lite by ticking
   the checkbox: 0x12000000 MRAM (Non-Secure) (Loader: Default).

9) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x32002590
   b. Keil:	0x320035b0
   c. IAR:	0x32006fc8

10) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
    _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings,
    iar\Debug\List).
