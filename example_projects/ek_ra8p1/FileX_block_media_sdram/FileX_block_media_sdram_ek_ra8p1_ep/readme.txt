/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project showcases how the FileX file system operates on the SDRAM block media of the RA MCU.
	The project will perform various FileX file system operations based on the user's selection from the menu
	options, such as media management (format), directory management (create, get properties, delete), and file
	management (create, write, read, delete). The terminal application is used as the user interface. The menu
	options and system messages (errors and information messages) will be printed on the terminal application
	during the execution of the project.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA8D1, EK-RA8P1, EK-RA8D2, EK-RA8T2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA8D1: Set the configuration switches (SW1) as below.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     OFF    |     ON      |    OFF    |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

	For EK-RA8T2:
	The user must set the configuration switches (SW6 and SW4) as below to use the on-board debug functionality.
	+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
	| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
	+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
	|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
	+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+

	+-------+-------+-------+-------+-------+-------+-------+-------+
	| SW4-1 | SW4-2 | SW4-3 | SW4-4 | SW4-5 | SW4-6 | SW4-7 | SW4-8 |
	+-------+-------+-------+-------+-------+-------+-------+-------+
	|  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |
	+-------+-------+-------+-------+-------+-------+-------+-------+

5. Verifying Operation:
	1. Import, generate, and build the example project.
	2. Before running the example project, make sure hardware connections are done.
	3. Open a serial terminal application (e.g., Tera Term) to view output log, configure the settings as shown below:
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
	4. Debug or flash the EP to the RA board.
	5. After the main menu is displayed on the terminal application, the user selects options to perform file system
	   management as desired.
		Type '1' and enter to format the media.
		Type '2' and enter to create a new directory.
		Type '3' and enter to get root directory properties.
		Type '4' and enter to delete a directory.
		Type '5' and enter to create an empty file.
		Type '6' and enter to write a fixed content into a file.
		Type '7' and enter to read the first 1 KB of content in a file.
		Type '8' and enter to delete a file.
