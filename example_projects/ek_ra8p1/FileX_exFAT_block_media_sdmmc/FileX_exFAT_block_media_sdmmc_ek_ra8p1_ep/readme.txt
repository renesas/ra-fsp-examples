/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project shows the operation of the FileX file system on block media via the SDHI driver on the
	RA MCU. The FileX file system with exFAT format enabled, the project will perform various file system operations
	based on the user's selection from the menu options, such as media management (open, get property, format, close),
	directory management (create, get property, delete), and file management (create, write, read, delete).
	The J-Link RTT Viewer is used as the user interface. Menu options and system messages (errors and information
	messages) will be printed on J-Link RTT Viewer during the execution of the project.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, FPB-RA6E1,
			     EK-RA8M1, EK-RA8D1, EK-RA8P1
 	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x SD card (e.g., https://shop.sandisk.com/en-sg/products/memory-cards/sd-cards/sandisk-ultra-uhs-i-sd?sku=SDSDUNC-032G-ZN6IN).
	1 x Pmod SD (e.g., https://digilent.com/shop/pmod-sd-full-sized-sd-card-slot/).
	Some jumper wires (Should be shorter than 10cm).

3. Software Configuration:
	Change the Bus Width properties in the configurator:
	For 4 Bits Width:
		SD/MMC (r_sdhi) Stack | Bus Width: 4 Bits

	For 1 Bit Width:
		SD/MMC (r_sdhi) Stack | Bus Width: 1 Bit

4. Hardware Connections:
	For EK-RA4M2: (SDHI 4 bits width by default)
		1 Bit Width:
			SD0CMD  P412 (J1:22)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J1:21)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P413 (J1:19)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SD0CD   P210 (J4:13)    --->    Pmod SD CD   (J1:9)
			SD0WP   P209 (J4:14)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			SD0DAT1 P410 (J1:24)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J4:9)     --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J4:10)    --->    Pmod SD CS   (J1:1)

	For EK-RA4M3: (SDHI 4 bits width by default)
		1 Bit Width:
			SDOCMD  P412 (J1:34)    --->    Pmod SD MOSI (J1:2)
			SDODAT0 P411 (J1:35)    --->    Pmod SD MISO (J1:3)
			SDOCLK  P413 (J1:33)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SDOCD   P210 (J4:19)    --->    Pmod SD CD   (J1:9)
			SDOWP   P209 (J4:20)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			SDODAT1 P410 (J1:36)    --->    Pmod SD DAT1 (J1:7)
			SDODAT2 P206 (J4:10)    --->    Pmod SD DAT2 (J1:8)
			SDODAT3 P205 (J4:11)    --->    Pmod SD CS   (J1:1)

	For EK-RA6M1: (SDHI 4 bits width by default)
		1 Bit Width:
			SDOCMD  P412 (J2:1)     --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J2:20)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P413 (J2:3)     --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SD0CD   P210 (J4:22)    --->    Pmod SD CD   (J1:9)
			SD0WP   P209 (J4:24)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Tracecut Jumper E15 and solder to E12. Above mentioned connections for 1 Bit Width.
		In addition below connections:
			SD0DAT1 P410 (J4:14)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J4:12)    --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J4:10)    --->    Pmod SD CS   (J1:1)
		
	For EK-RA6M2: (SDHI 4 bits width by default)
		1 Bit Width:
			SD0CMD  P412 (J2:1)     --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J2:20)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P413 (J2:8)     --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SD0CD   P210 (J1:28)    --->    Pmod SD CD   (J1:9)
			SD0WP   P209 (J1:30)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Tracecut Jumper E15 and solder to E12. Above mentioned connections for 1 Bit Width.
		In addition below connections:
			SD0DAT1 P410 (J2:2)     --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J1:3)     --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J2:14)    --->    Pmod SD CS   (J1:1)
		
	For EK-RA6M3/EK-RA6M3G: (SDHI 4 bits width by default)
		1 Bit Width:
			SD0CMD  P412 (J3:33)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J3:36)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P413 (J3:34)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SD0CD   P415 (J3:32)    --->    Pmod SD CD   (J1:9)
			SD0WP   P414 (J3:31)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			SD0DAT1 P410 (J3:35)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J1:38)    --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J2:4)     --->    Pmod SD CS   (J1:1)
	
	For EK-RA6M4: (SDHI 4 bits width by default)
		1 Bit Width:
			SD0CMD  P412 (J1:34)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J1:35)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P413 (J1:33)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SD0CD   P210 (J4:19)    --->    Pmod SD CD   (J1:9)
			SD0WP   P209 (J4:20)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			SD0DAT1 P410 (J1:36)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J4:10)    --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J4:11)    --->    Pmod SD CS   (J1:1)
			
	For EK-RA6M5: (SDHI 4 bits width by default)
		1 Bit Width:
			SD0CMD  P412 (J1:34)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J1:35)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P413 (J1:33)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SD0CD   P210 (J2:17)    --->    Pmod SD CD   (J1:9)
			SD0WP   P209 (J2:18)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			SD0DAT1 P410 (J1:36)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J2:4)     --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J2:5)     --->    Pmod SD CS   (J1:1)

	For FPB-RA6E1: (SDHI 4 bits width by default)
		1 Bit Width: 
			SD0CMD  P412 (J3:20)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J3:21)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P413 (J3:19)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SD0CD   P210 (J3:35)    --->    Pmod SD CD   (J1:9)
			SD0WP   P209 (J3:36)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			SD0DAT1 P410 (J3:22)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J3:31)    --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J3:32)    --->    Pmod SD CS   (J1:1)
		
	For EK-RA8D1: The user must turn-off SW1-3, SW1-5, SW1-8 on board to use SDHI channel 1 (SDHI 4 bits width
		      by default).
		1 Bit Width: 
			SD1CMD  P401 (J51:7)    --->    Pmod SD MOSI (J1:2)
			SD1DAT0 P402 (J52:15)   --->    Pmod SD MISO (J1:3)
			SD1CLK  P400 (J51:10)   --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SD1CD   P406 (J51:5)    --->    Pmod SD CD   (J1:9)
			SD1WP   P700 (J51:6)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Above mentioned connections for 1 bit width. In addition below connections:
			SD1DAT1 P403 (J51:9)    --->    Pmod SD DAT1 (J1:7)
			SD1DAT2 P404 (J57:1)    --->    Pmod SD DAT2 (J1:8)
			SD1DAT3 P405 (J51:8)    --->    Pmod SD CS   (J1:1)
			
	For EK-RA8M1: J61 on board must be removed (SDHI 4 bits width by default).
		1 Bit Width: 
			SD0CMD  P307 (J54:5)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P304 (J54:9)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P308 (J54:4)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			+3V3(VCC)               --->    Pmod SD VCC  (J1:6)
			SD0CD   P306 (J54:7)    --->    Pmod SD CD   (J1:9)
			SD0WP   P305 (J54:8)    --->    Pmod SD WP   (J1:10)

		4 Bits Width: Above mentioned connections for 1 bit width. In addition below connections:
			SD0DAT1 P303 (J54:6)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P302 (J54:14)   --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P301 (J54:12)   --->    Pmod SD CS   (J1:1)

	For EK-RA8P1: (SDHI 4 bits width by default)
		1-Bit Width Connections:  
			SD1CMD  P401 (J17:5)    --->    Pmod SD MOSI (J1:2)
			SD1DAT0 P402 (J17:14)   --->    Pmod SD MISO (J1:3)
			SD1CLK  P400 (J17:6)    --->    Pmod SD SCK  (J1:4)
			GND          (J17:19)   --->    Pmod SD GND  (J1:5)
			+3V3    VCC  (J17:1)    --->    Pmod SD VCC  (J1:6)
			SD1CD   P406 (J41:1)    --->    Pmod SD CD   (J1:9)
			SD1WP   P700 (J3:9)     --->    Pmod SD WP   (J1:10)

		4-Bit Width Additional Connections:  
			SD1DAT1 P403 (J17:7)    --->    Pmod SD DAT1 (J1:7)
			SD1DAT2 P404 (J17:10)   --->    Pmod SD DAT2 (J1:8)
			SD1DAT3 P811 (J3:21)    --->    Pmod SD CS   (J1:1)

5. Verifying Operation:
	a) Import, generate, build the example project.
	b) Connect the RA MCU debug port to the host PC via a Type-C USB cable.
	c) Connect the Pmod SD to the RA board: Using short wiring connections (should be shorter than 10cm).
	d) Insert the SD card into the Pmod SD.
	e) Debug or flash the example project to the RA board.
	f) Open the J-Link RTT Viewer.
	g) After the main menu is displayed on the RTT Viewer, the user selects options to perform file system management
	   as desired.

Note:
1) SD card must be in exFAT format before verifying the EP.

2) In case the SD is not in exFAT format, the user can format the SD card in exFAT format using this EP using
   the following procedure: Open Media -> Format Media -> Close Media -> Open Media.

3) The Media must be opened before performing other operations.
	a) In MAIN MENU: The user input option from the RTT Viewer will go to the next menu as follows:
		Type '1' and enter to go to MEDIA MENU.
		Type '2' and enter to go to DIRECTORY MENU.
		Type '3' and enter to go to FILE MENU.

	b) In MEDIA MENU: The user input options will perform media operations as follows:
		Type '1' and enter to open the media.
		Type '2' and enter to get media properties.
		Type '3' and enter to format the media in exFAT format.
		Type '4' and enter to close the media.
		Type '5' and enter to go back MAIN MENU.

	c) In DIRECTORY MENU: The user input options will perform directory operations as follows:
		Type '1' and enter to create a new directory.
		Type '2' and enter to get root directory properties.
		Type '3' and enter to delete a directory.
		Type '4' and enter to go back MAIN MENU.

	d) In FILE MENU: The user input options will perform file operations as follows:
		Type '1' and enter to create an empty file.
		Type '2' and enter to write more than 4 GB of fixed content into a file.
		Type '3' and enter to read the first 1 KB of content in a file.
		Type '4' and enter to delete a file.
		Type '5' and enter to go back MAIN MENU.

4) The EP will write more than 4 GB of fixed data to the file. It takes few minutes to create a larger file.
   Please validate the file size and contents on the PC.

5) Segger RTT block address may need to be updated to observe the EP operation using a hex file with RTT-viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x220004f0
   b. Keil:	Not Available
   c. IAR:	Not Available

6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
