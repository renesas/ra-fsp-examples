/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project showcases how the FileX file system operates on the internal SRAM block media of the RA MCU.
	The project will perform various FileX file system operations based on the user's selection from the menu options, 
	such as media management (format), directory management (create, get properties, delete), and file management (create,
	write, read, delete). The terminal application is used as the user interface. The menu options and system messages 
	(errors and information messages) will be printed on the terminal application during the execution of the project.

	Note: - To display information, users can select between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART) 
	        with J-Link OB VCOM. 
	      - By default, EP information is printed to the host PC using the Serial Terminal for boards that support J-Link OB VCOM. 
		For RA boards that do not support J-Link OB VCOM, EP information is instead printed via the SEGGER J-Link RTT Viewer
	      - RA boards supported for J-Link OB VCOM: EK-RA8M1, MCK-RA8T1, EK-RA8D1.
	      - To utilize the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please follow the instructions 
	        in FileX_block_media_sram_notes.md file.

2. Software:
	Renesas Flexible Software Package (FSP): Version 5.9.0
	e2 studio: Version 2025-04
	SEGGER J-Link RTT Viewer: Version 8.12f
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application 

3. Hardware Requirements:
	Supported RA boards: EK-RA8M1, MCK-RA8T1, EK-RA8D1, EK-RA6M3G, EK-RA6M3, CK-RA6M5, EK-RA6M5, EK-RA6M2, EK-RA6M4, EK-RA6M1,
			     FPB-RA6E1, FPB-RA4E1, EK-RA4M2, EK-RA4M3.
 	1 x Renesas RA board.
	1 x Micro USB cable for programming and debugging or 1x type C USB cable (For MCK-RA8T1). 

4. Hardware Connections:
	For EK-RA8M1, EK-RA8D1, EK-RA6M3G, EK-RA6M3, CK-RA6M5, EK-RA6M5, EK-RA6M2, EK-RA6M4, EK-RA6M1, FPB-RA6E1, FPB-RA4E1, 
	    EK-RA4M2, EK-RA4M3:
		Connect the USB Debug port on the RA board to the host PC via a micro USB cable.
	For MCK-RA8T1: 
		Connect the USB Debug port on the RA board to the host PC via a type C USB cable.

5. Verifying Operation:
	1. Import the example project. Note that the EP supports the Serial terminal by default for RA boards that support J-link OB VCOM.
	2. Generate, build the Example project.
	3. Connect the RA MCU debug port to the host PC via a micro USB cable or type C USB cable (For MCK-RA8T1).
	4. Open a Serial terminal application on the host PC and connect to the COM Port provided by the J-Link on-board or 
	   Open J-Link RTT Viewer (In case user selected SEGGER J-Link RTT Viewer or RA boards do not support J-Link OB VCOM).
	5. Debug or flash the EP project to the RA board.
	6. After the main menu is displayed on the terminal application, the user selects options to perform file system management as desired.
		Type '1' and enter to format the media.
		Type '2' and enter to create a new directory.
		Type '3' and enter to get root directory properties.
		Type '4' and enter to delete a directory.
		Type '5' and enter to create an empty file.
		Type '6' and enter to write a fixed content into a file.
		Type '7' and enter to read the first 1 KB of content in a file.
		Type '8' and enter to delete a file.

	Note:
	For Serial terminal application:
	1) To echo back what was typed in Tera Term, the user needs to enable it through [Setup] -> [Terminal...] -> Check [Local echo].	
	
	2) The configuration parameters of the serial port on the terminal application are as follows:
		COM port is port provided by the J-Link on-board.
		Baud rate: 115200 bps
		Data length: 8-bits  
		Parity: none
		Stop bit: 1-bit
		Flow control: none
	
	For SEGGER J-Link RTT Viewer application:
	1) Segger RTT block address may need to be updated to observe the EP operation using a hex file with RTT-viewer.
	RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000092c
		b. Keil: Not Available
		c. IAR: Not Available
	
	2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
	in .map file generated in the build configuration folder (Debug/Release).
