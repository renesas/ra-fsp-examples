/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionalities of USB PHID driver on Renesas RA MCUs based on
	Renesas FSP. The project configures RA board as a keyboard. On pressing any key in the terminal, the MCU
	sends characters 'a' to 'z' followed by '1' to '9' and '0' to the host PC.

2. Software Requirements:
	Refer to the "Tools" section in the Release Notes to set up all the required software via link:
	https://github.com/renesas/fsp/releases

3. Hardware Requirements:
	Supported RA boards: EK-RA4E2, EK-RA4L1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M4, EK-RA6M5, EK-RA8D1,
			     EK-RA8D2, EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, EK-RA8T2, MCK-RA8T1, MCK-RA8T2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x USB cable for USB communication between the RA board and the host PC (USB cable type varies by board model).
	1 x Host PC with at least 2 available USB ports (1 for debugging and 1 for the COM port).

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full Speed):
		- Jumper J12 placement is pins 2-3.
		- Connect jumper J15 pins.
		- Connect the RA board USB FS port (J11) to the host PC via the appropriate USB cable.

	For EK-RA4E2, EK-RA6E2 (Full Speed):
		- Connect the RA board USB FS port (J11) to the host PC via the appropriate USB cable.

	For MCK-RA8T1 (Full Speed):
		- Jumper JP9 placement is pins 2-3.
		- Connect jumper JP10 pins.
		- Connect the RA board USB FS port (CN14) to the host PC via the appropriate USB cable.

	For EK-RA4L1 (Full Speed):
		- Turn ON S4-4 to select USB device mode.
		- Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		- Connect the RA board USB FS port (J11) to the host PC via the appropriate USB cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8M2, EK-RA8D2, EK-RA8T2 (Full Speed):
		- For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on
		  pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		- For EK-RA8T2: Set the configuration switches (SW4 and SW6) as below to use the on-board debug functionality.
		+-------+-------+-------+-------+-------+-------+-------+-------+
		| SW4-1 | SW4-2 | SW4-3 | SW4-4 | SW4-5 | SW4-6 | SW4-7 | SW4-8 |
		+-------+-------+-------+-------+-------+-------+-------+-------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |
		+-------+-------+-------+-------+-------+-------+-------+-------+
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		- Connect the RA board USB FS port (J11) to the host PC via the appropriate USB cable.

	For MCK-RA8T2 (Full Speed):
		- Jumper JP6 placement is pins 2-3.
		- Connect jumper JP7 pins.
		- Connect the RA board USB FS port (CN18) to the host PC via the appropriate USB cable.

5. Verifying Operation:
	a) Import and build projects: the user must build secure project (_s) first, then build non-secure (_ns) project.
	   Note: For Keil, please note that a licensed version of Keil is required to build this project.
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
	c) Open a serial terminal application (e.g., Tera Term) to view the example project operation.

Note:
1) Open "Device Manager", and check "Human Interface Devices", to see "USB Input Device" has been detected.

2) Lower case a - z and numbers 0 - 9 are sent from the MCU to the host PC.

3) The user is expected to enter data not exceeding 15 bytes in size.

4) The user should view "Example Project for IP Protection" section of application note R11AN0467 on how to build
   and run a trustzone project via link:
   https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33

5) The user should refer to the "RASC User Guide for MDK and IAR" section for instructions on using RASC for IAR
   and Keil via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-IAR-user-guide

6) For the IAR project, the user can launch the workspace using the .eww file in the non-secure project folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

7) For the KEIL project, the user can launch the workspace using the .uvmpw file in keil folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.
   Please ensure that the DLL version matches the required J-Link version before debugging the project.
   Open project, connect the RA board to the host PC, and the DLL version can be found at:
	Project:'Project_name' -> Target_1 -> Options for ('Project_name') Target 'Target_1'... -> Debug ->
	Settings (J-LINK/J-TRACE Cortex) -> Debug -> J-Link/J-Trace Adapter -> dll:

8) For EK-RA8P1, EK-RA8M2, EK-RA8D2, MCK-RA8T2, EK-RA8T2:
   When using SEGGER J-Flash Lite to flash the hex file of the non-secure project into the RA board,
   make sure to select the correct flash bank in the "Flash banks" section of the SEGGER J-Flash Lite
   by ticking the checkbox: 0x12000000 MRAM (Non-Secure) (Loader: Default).

9) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
	a. To make sure the received data shows properly, the user needs to set:
		[Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR].

	b. To echo back characters typed in Tera Term, the user needs to enable it through:
		[Setup] -> [Terminal...] -> Check [Local echo].

	c. The configuration parameters of the serial port on the serial terminal application:
		Port: The port is provided by the J-Link On-Board.
		Speed: 115200
		Data: 8 bit
		Parity: none
		Stop bits: 1 bit
		Flow control: none
