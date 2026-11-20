/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the IIC Master HAL module APIs. The project initializes
	IIC Master module with standard rate and interfaces with Pmod ACL Board for ADXL345. On power up after
	establishing the connection of sensor with RA board, it displays accelerometer axis data on the terminal.
	Additionally, any error messages will be displayed on the terminal.

2. Software Requirements:
	Refer to the "Tools" section in the FSP release notes to set up all the required software via the link below:
	https://github.com/renesas/fsp/releases

3. Hardware Requirements:
	Supported RA boards: EK-RA4C1, EK-RA4L1, EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2,
			     EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, FPB-RA8E1, MCK-RA8T1, MCK-RA8T2.
	1 x Renesas RA board.
	1 x Pmod ACL (e.g., https://digilent.com/reference/pmod/pmodacl/start).
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	Some jumper wires used to connect the RA board to the Pmod ACL.

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA6M4, EK-RA6M5:
		SDA0 P401  ----> SDA (on Pmod ACL)
		SCL0 P400  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For EK-RA4M3:
		SDA0 P401  ----> SDA (on Pmod ACL)
		SCL0 P408  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For EK-RA4M2:
		SDA1 P206  ----> SDA (on Pmod ACL)
		SCL1 P205  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For EK-RA8M1, EK-RA8D1:
		SDA1 P511  ----> SDA (on Pmod ACL)
		SCL1 P512  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For MCK-RA8T1:
		SDA1 P206 (CN2:24) ----> SDA (on Pmod ACL)
		SCL1 P205 (CN2:25) ----> SCL (on Pmod ACL)
		VCC pin   (CN1:24) ----> VCC (on Pmod ACL)
		GND pin   (CN1:34) ----> GND (on Pmod ACL)

	For FPB-RA8E1:
		SDA1 P511 (J4:4)  ----> SDA (on Pmod ACL)
		SCL1 P512 (J4:3)  ----> SCL (on Pmod ACL)
		VCC pin   (J4:1)  ----> VCC (on Pmod ACL)
		GND pin   (J4:2)  ----> GND (on Pmod ACL)

	For EK-RA4L1:
		Place J15 jumper to use P100, P101 for I2C channel 0.
		SDA0 P100 (J3:2)  ----> SDA (on Pmod ACL)
		SCL0 P101 (J3:4)  ----> SCL (on Pmod ACL)
		VCC pin   (J3:1)  ----> VCC (on Pmod ACL)
		GND pin   (J3:25) ----> GND (on Pmod ACL)

	For EK-RA8E2:
		SDA1 P511 (J4:3)  ----> SDA (on Pmod ACL)
		SCL1 P512 (J4:5)  ----> SCL (on Pmod ACL)
		VCC pin   (J4:1)  ----> VCC (on Pmod ACL)
		GND pin   (J4:19) ----> GND (on Pmod ACL)

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		SDA1 P401 (J1:4)  ----> SDA (on Pmod ACL)
		SCL1 P400 (J1:3)  ----> SCL (on Pmod ACL)
		VCC pin   (J4:1)  ----> VCC (on Pmod ACL)
		GND pin   (J4:5)  ----> GND (on Pmod ACL)

	For EK-RA8P1:
		SDA1 P511 (J1:2)  ----> SDA (on Pmod ACL)
		SCL1 P512 (J1:4)  ----> SCL (on Pmod ACL)
		VCC pin   (J1:5)  ----> VCC (on Pmod ACL)
		GND pin   (J1:40) ----> GND (on Pmod ACL)

	For EK-RA8D2:
		SDA1 P511 (J1:2)  ----> SDA (on Pmod ACL)
		SCL1 P512 (J1:4)  ----> SCL (on Pmod ACL)
		VCC pin   (J4:5)  ----> VCC (on Pmod ACL)
		GND pin   (J4:39) ----> GND (on Pmod ACL)

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.
		SDA1 P511 (J4:14) ----> SDA (on Pmod ACL)
		SCL1 P512 (J4:11) ----> SCL (on Pmod ACL)
		VCC pin   (J4:1)  ----> VCC (on Pmod ACL)
		GND pin   (J4:19) ----> GND (on Pmod ACL)

	For MCK-RA8T2:
		SDA2 P514 (CN2:24) ----> SDA (on Pmod ACL)
		SCL2 P515 (CN2:25) ----> SCL (on Pmod ACL)
		VCC pin   (CN1:24) ----> VCC (on Pmod ACL)
		GND pin   (CN1:26) ----> GND (on Pmod ACL)

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
		- Only setting used for IAR project: The user can change setting in the Tools -> Options -> Stack ->
		  Uncheck "Stack pointer(s) not valid until program reaches" option to ignore Stack Warning at launch.
   		- Launch the debug session from the non-secure project.
	c) Open a serial terminal application (e.g., Tera Term) to view the example project operation.

Note:
1) For the functioning of IIC Master on all the boards except for EK-RA8M1, EK-RA8D1, FPB-RA8E1, EK-RA4L1, EK-RA8E2,
   EK-RA4C1, EK-RA8P1, EK-RA8D2, EK-RA8M2, MCK-RA8T2, external pull up resistors of value 3.9 or 4.7K ohms are required
   to be connected on I2C (SDA/SCL) lines.

2) The user should view "Example Project for IP Protection" section of application note R11AN0467 on how to build
   and run a trustzone project via link:
   https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33

3) The user should refer to the "RASC User Guide for MDK and IAR" section for instructions on using RASC for IAR
   and Keil via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-IAR-user-guide

4) For the IAR project, the user can launch the workspace using the .eww file in the non-secure project folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

5) For the KEIL project, the user can launch the workspace using the .uvmpw file in keil folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.
   Please ensure that the DLL version matches the required J-Link version before debugging the project.
   Open project, connect the RA board to the host PC, and the DLL version can be found at:
	Project:'Project_name' -> Target_1 -> Options for ('Project_name') Target 'Target_1'... -> Debug ->
	Settings (J-LINK/J-TRACE Cortex) -> Debug -> J-Link/J-Trace Adapter -> dll:

6) For EK-RA8P1, MCK-RA8T2, EK-RA8D2, EK-RA8M2: When using SEGGER J-Flash Lite to flash the hex file of the
   non-secure project into the RA board, make sure to select the correct flash bank in the "Flash banks" section
   of the SEGGER J-Flash Lite by ticking the checkbox: 0x12000000 MRAM (Non-Secure) (Loader: Default).

7) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
