/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the operation of the FileX file system, incorporating LevelX wear leveling
	features on the OSPI block media of the RA MCU. Based on the user's input in terminal, EP performs various
	FileX file system operations, such as erase OSPI Flash, media management (open, format), directory management
	(create, get properties, delete), file management (create, write, read, delete), and sector level operation
	(defragment, write, read, erase). The terminal application is used as the user interface. Errors and
	information messages will be printed on the terminal application.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	E2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA8D1, EK-RA8P1, EK-RA8T2, EK-RA8D2, EK-RA8M2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA8P1: 
		Set the configuration switches (SW4) as below to avoid potential failures.
		+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
		| SW4-1 PMOD1_SEL0 | SW4-2 PMOD1_SEL1 |    SW4-3 OSPI   |   SW4-4 ARDUINO  |    SW4-5 I3C   |    SW4-6 MIPI  | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
		+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
		|       OFF        |       OFF        |       OFF       |       OFF        |      OFF       |      OFF       |       OFF        |       OFF        |
		+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+

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

	For EK-RA8D2:
		Set the configuration switches (SW4) as below to avoid potential failures.
		+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
		| SW4-1 PMOD1 | SW4-2 PMOD1 | SW4-3 Octo-SPI | SW4-4 Arduino/mikroBUS | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS | SW4-8 USBHS |
		+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
		|     OFF     |     OFF     |      OFF       |          OFF           |    OFF    |    OFF     |     OFF     |     OFF     |
		+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+

	For EK-RA8M2: 
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.

5. Menu details:
	In MAIN MENU: The user input options from the terminal will go to the next menu as follows:
		Type '1' to go to FLASH MEDIA MENU.
		Type '2' to go to DIRECTORY MENU.
		Type '3' to go to FILE MENU.
		Type '4' to go to SECTOR MENU.
	In FLASH MEDIA MENU: The user input options will perform flash, media operations as follows:
		Type '1' to erase the entire flash.
		Type '2' to format the media in FAT32 format.
		Type '3' to open the media.
		Type '4' to defragment the Flash.
		Type '5' to go back MAIN MENU.
	In DIRECTORY MENU: The user input options will perform directory operations as follows:
		Type '1' to create a new directory.
		Type '2' to get root directory properties.
		Type '3' to delete a directory.
		Type '4' to go back MAIN MENU.
	In FILE MENU: The user input options will perform file operations as follows:
		Type '1' to create an empty file.
		Type '2' to write a fixed content into a file.
		Type '3' to read the first 1 KB of content in a file.
		Type '4' to delete a file.
		Type '5' to go back MAIN MENU.
	In SECTOR MENU: The user input options will perform sector operations as follows:
		Type '1' and enter sector to write data into the selected sector.
		Type '2' and enter sector to read and display the data in the selected sector.
		Type '3' and enter sector to erase the selected sector.
		Type '4' to go back MAIN MENU.

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
