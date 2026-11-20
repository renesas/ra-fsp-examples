/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of LittleFS for external flash (e.g., SPI, OSPI, QSPI)
	on RA MCU based on FSP. The project will perform various file system operations based on the user's selection
	from menu options, such as erase entire flash, erase 32 KB flash, format file system, directory management
	(create, get properties, delete), and file management (create, write, read, delete). The terminal application
	is used as the user interface. The menu options and system messages (errors and information messages)
	will be printed on the terminal application during the execution of the project.

2. Hardware Requirements:
	Supported RA board: EK-RA8D1, EK-RA8D2, EK-RA8M2, EK-RA8P1, EK-RA8T2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA8P1: Set the configuration switches (SW4) as below to avoid potential failures.
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	| SW4-1 PMOD1_SEL0 | SW4-2 PMOD1_SEL1 |    SW4-3 OSPI   |   SW4-4 ARDUINO  |    SW4-5 I3C   |    SW4-6 MIPI  | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	|       OFF        |       OFF        |       OFF       |       OFF        |      OFF       |      OFF       |       OFF        |       OFF        |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+

	For EK-RA8D2:
	Set the configuration switches (SW4) as below to avoid potential failures.
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
	| SW4-1 PMOD1 | SW4-2 PMOD1 | SW4-3 Octo-SPI | SW4-4 Arduino/mikroBUS | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS | SW4-8 USBHS |
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
	|     OFF     |     OFF     |      OFF       |          OFF           |    OFF    |    OFF     |     OFF     |     OFF     |
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.

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

4. Software Configurations:
	By default, to create an empty file or open an existed file with malloc
		LittleFS | Use Malloc		: Enabled
		BSP 	 | Heap size (bytes) 	: 0x1000

	To create an empty file or open an existed file without malloc
		LittleFS | Use Malloc		: Disabled
		BSP 	 | Heap size (bytes) 	: 0

5. Verifying Operation:
	1. Import the example project (EP).
	2. Generate, build the EP.
	3. Set up the hardware connection properly before running the EP.
	4. Open a serial terminal application (e.g., Tera Term) to view output log, configure the settings as shown below:
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
	5. Debug or flash the EP to the RA board.
	6. After the main menu is displayed on the terminal application, the user selects options to perform file system
	   management as desired.
		Type '1' and enter to select flash operations menu.
			Type '1' and enter to erase the entire flash.
			Type '2' and enter to erase the first 32 KB of the flash.
			Type '3' and enter to format the file system.
			Type '4' and enter to go back to the main menu.
			Note: After erasing the entire flash or 32 KB flash, the user must format the file system.

		Type '2' and enter to select directory operations menu.
			Type '1' and enter to create a new directory.
			Type '2' and enter to get the root directory properties.
			Type '3' and enter to delete a directory.
			Type '4' and enter to go back to the main menu.

		Type '3' and enter to select file operations menu.
			Type '1' and enter to create an empty file or open an existed file.
			Type '2' and enter to write a fixed content into a file.
			Type '3' and enter to read the entire file and display the first 1 KB of its content.
			Type '4' and enter to delete a file.
			Type '5' and enter to go back to the main menu.
