/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/
1. Project Overview:
	This example project demonstrates the operation of the FileX file system, incorporating LevelX wear leveling
	features on the OSPI block media of the RA MCU. The project will perform various FileX file system operations
	based on the user's selection from the menu option, such as erase OSPI Flash, media management (open, format),
	directory management (create, get properties, delete), file management (create, write, read, delete), and sector 
	level operation (defragment, write, read, erase). The terminal application is used as the user interface. The menu options and
	system messages (errors and information messages) will be printed on the terminal application during the execution
	of the project.

	Note: - To display information, users can choose between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART) 
	        with J-Link OB VCOM. It is important to note that the user should only operate a single terminal application at a time
			to avoid conflicts or data inconsistencies. 
		  - For instructions on how to switch between these options, please refer to the "Verifying Operation" section
			in the FileX_block_media_ospi_LevelX_notes.md file.
			
	      - By default, EP information is printed to the host PC using the Serial Terminal for boards that support J-Link OB VCOM. 
			Vice versa, for the RA boards that do not support J-Link OB VCOM, EP uses the SEGGER J-Link RTT Viewer by default instead.
	      - RA boards supported for J-Link OB VCOM: EK-RA8D1

2. Software:
	Renesas Flexible Software Package (FSP): Version 5.7.0
	E2 studio: Version 2024-10
	SEGGER J-Link RTT Viewer: Version 8.10f
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application (for boards that support J-Link OB VCOM)

3. Hardware Requirements:
	Supported RA boards: EK-RA8D1.
 	1 x Renesas RA board.
	1 x Micro USB cable for programming and debugging. 

4. Hardware Connections:
		Connect the USB Debug port on the RA board to the host PC via a micro USB cable.

5. Verifying Operation:
	1. Import the example project. Note that the EP supports the Serial terminal by default for RA boards that support J-link OB VCOM.
	2. Generate, and build the Example project.
	3. Before running the example project, make sure hardware connections are completed.
	4. Connect the RA MCU debug port to the host PC via a micro USB cable.
	5. Open a Serial terminal application on the host PC and connect to the COM Port provided by the J-Link on-board or 
	   Open J-Link RTT Viewer (In case the user selected SEGGER J-Link RTT Viewer or RA boards do not support J-Link OB VCOM).
	6. Debug or flash the EP project to the RA board.
	7. After the main menu is displayed on the terminal application, the user selects options to perform file system management as desired.

Note: The user needs to type the menu option and hit enter key according to the Menu details.

6. Menu details:

	In MAIN MENU: The user input options from the terminal will go to the next menu as follows:
		Type '1' and enter to go to FLASH MEDIA MENU.
		Type '2' and enter to go to DIRECTORY MENU.
		Type '3' and enter to go to FILE MENU.
		Type '4' and enter to go to SECTOR MENU.
	In FLASH MEDIA MENU: The user input options will perform flash, media operations as follows:
		Type '1' and enter to erase the entire flash.
		Type '2' and enter to format the media in FAT32 format.
		Type '3' and enter to open the media.
		Type '4' and enter to defragment the Flash.
		Type '5' and enter to go back MAIN MENU.
	In DIRECTORY MENU: The user input options will perform directory operations as follows:
		Type '1' and enter to create a new directory.
		Type '2' and enter to get root directory properties.
		Type '3' and enter to delete a directory.
		Type '4' and enter to go back MAIN MENU.
	In FILE MENU: The user input options will perform file operations as follows:
		Type '1' and enter to create an empty file.
		Type '2' and enter to write a fixed content into a file.
		Type '3' and enter to read the first 1 KB of content in a file.
		Type '4' and enter to delete a file.
		Type '5' and enter to go back MAIN MENU.
	In SECTOR MENU: The user input options will perform sector operations as follows:
		Type '1' and enter to write data into the selected sector.
		Type '2' and enter to read and display the data in the selected sector.
		Type '3' and enter to erase the selected sector.
		Type '4' and enter to go back MAIN MENU.

	Note:
	For Serial terminal application:
	1) To echo back what was typed in Tera Term, the user needs to enable it through [Setup] -> [Terminal...] -> Check [Local echo].	
	2) The configuration parameters of the serial port on the terminal application are as follows:
			COM port is a port provided by the J-Link on-board.
			Baud rate: 115200 bps
			Data length: 8-bits  
			Parity: none
			Stop bit: 1-bit
			Flow control: none
			
