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

	Note:
		- To display information, users can choose between the SEGGER J-Link RTT Viewer and the Serial
		  Terminal (UART) with J-Link OB VCOM. It is important to note that the user should only operate
		  a single terminal application at a time to avoid conflicts or data inconsistencies.
		- For instructions on how to switch between these options, please refer to the "Verifying Operation"
		  section in the FileX_block_media_sdram_notes.md file.
		- By default, EP information is printed to the host PC using the Serial Terminal for boards that
		  support J-Link OB VCOM. Vice versa, for the RA boards that do not support J-Link OB VCOM, EP uses
		  the SEGGER J-Link RTT Viewer by default instead.
		- RA boards supported for J-Link OB VCOM: EK-RA8D1, EK-RA8P1.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	LLVM Embedded Toolchain for ARM: Version 18.1.3
	Terminal Console Application: Tera Term or a similar application (for boards that support J-Link OB VCOM)

3. Hardware Requirements:
	Supported RA boards: EK-RA8D1, EK-RA8P1
 	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

4. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

	For EK-RA8D1: Set the configuration switches (SW1) as below.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     OFF    |     ON      |    OFF    |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

5. Verifying Operation:
	1. Import the example project. Note that the EP supports the Serial Terminal by default for RA boards that
	   support J-Link OB VCOM.
	2. Generate, and build the example project.
	3. Connect the RA MCU debug port to the host PC via a Type-C USB cable.
	4. Open a Serial Terminal application on the host PC and connect to the COM Port provided by the J-Link on-board or 
	   open J-Link RTT Viewer (In case the user selected SEGGER J-Link RTT Viewer or RA boards do not support J-Link OB VCOM).
	5. Debug or flash the EP to the RA board.
	6. After the main menu is displayed on the terminal application, the user selects options to perform file system
	   management as desired.
		Type '1' and enter to format the media.
		Type '2' and enter to create a new directory.
		Type '3' and enter to get root directory properties.
		Type '4' and enter to delete a directory.
		Type '5' and enter to create an empty file.
		Type '6' and enter to write a fixed content into a file.
		Type '7' and enter to read the first 1 KB of content in a file.
		Type '8' and enter to delete a file.

	Note:
	For Serial Terminal application:
	1) To echo back what was typed in Tera Term, the user needs to enable it through [Setup] -> [Terminal...] -> Check [Local echo].	
	2) The configuration parameters of the serial port on the terminal application are as follows:
		COM port is a port provided by the J-Link on-board.
		Baud rate: 115200 bps
		Data length: 8-bits  
		Parity: none
		Stop bit: 1-bit
		Flow control: none

6. Limitation: 
	Please note that the current configuration supports the FAT12 and FAT16 formats for media storage in SDRAM.
	Future updates will enhance compatibility by supporting larger SDRAM capacities and the FAT32 format.
