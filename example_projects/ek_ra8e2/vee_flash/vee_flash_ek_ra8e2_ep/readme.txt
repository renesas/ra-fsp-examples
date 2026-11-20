/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of vee flash driver on Renesas RA MCUs based on
	Renesas FSP. Based on the user input, the EP performs the following operations:
	1. Writing different data types to the different RECORD_IDs of vee flash.
	2. Reading back the data from the different RECORD_IDs.
	3. Format operation.
	4. Get the status of Virtual EEPROM.
	5. Refresh operation.
	Information, success, and error messages will be printed on the terminal.

2. Hardware Requirements:
	Supported RA boards: CK-RA6M5, EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1,
			     EK-RA4E2, EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1,
			     EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8E2, EK-RA8M1,
			     FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA2E3, FPB-RA2T1, FPB-RA4E1, FPB-RA6E1,
			     FPB-RA8E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, RSSK-RA6T1, FPB-RA4E2, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

	For FPB-RA2E3:
		The user must close E25, E28, E46, E47 to use J-Link OB VCOM for serial terminal (UART).

Note:
1) It is important to note that a refresh occurs automatically when a segment lacks space. Applications may force 
   a refresh (option 5) if space is low and more data is expected. However, a refresh should not be forced when
   the flash is empty (e.g., after a format or erase), as there is no data to preserve. In such cases, performing
   a refresh is unnecessary and may lead to unintended behavior or errors during flash operations.

2) For EK-RA8D1, EK-RA8M1, MCK-RA8T1, FPB-RA8E1, EK-RA8E2 boards:
   Initialize device back to factory default by using Renesas Device Partition Manager before run the example project.
   The user can refer Section 5.3 "Setting up Hardware" in the document via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection
   For Renesas Device Partition Manager tool on each IDE:
   - Start E2Studio -> Run -> Renesas Debug Tools -> Renesas Device Partition Manager (Available on e2studio)

3) EK-RA4L1 board must have HOCO running and stable in order to program and erase flash.

4) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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

5) For GCC ARM Embedded Toolchain: To enable printing floats to the terminal, edit the project settings and
   ensure that "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous >
   Use float with nano printf
