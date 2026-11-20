/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates how the FileX file system operates on the internal SRAM block media of the RA
	MCU. The project will perform various FileX file system operations based on the user's selection from the menu
	options, such as media management (format), directory management (create, get properties, delete), and file
	management (create, write, read, delete). The terminal application is used as the user interface. Errors and
	information messages will be printed on the terminal application.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application 

3. Hardware Requirements:
	Supported RA boards: EK-RA8M1, MCK-RA8T1, EK-RA8D1, EK-RA6M3G, EK-RA6M3, CK-RA6M5, EK-RA6M5, EK-RA6M2,
	                     EK-RA6M4, EK-RA6M1, FPB-RA6E1, FPB-RA4E1, EK-RA4M2, EK-RA4M3, EK-RA8P1, EK-RA8M2,
	                     EK-RA8D2, MCK-RA8T2
 	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.

5. Verifying Operation:
	1. Import, generate, build the example project.
	2. Before running the example project, make sure hardware connections are completed.
	3. Open a serial terminal application (e.g., Tera Term) to view output log, configure the settings.
	4. Debug or flash the EP to the RA board.
	5. After the main menu is displayed on the terminal application, the user selects options to perform file
	   system management as desired.
		* Type '1' and enter to format the media.
		* Type '2' and enter to create a new directory.
		* Type '3' and enter to get root directory properties.
		* Type '4' and enter to delete a directory.
		* Type '5' and enter to create an empty file.
		* Type '6' and enter to write a fixed content into a file.
		* Type '7' and enter to read the first 1 KB of content in a file.
		* Type '8' and enter to delete a file.

Note:
1) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
