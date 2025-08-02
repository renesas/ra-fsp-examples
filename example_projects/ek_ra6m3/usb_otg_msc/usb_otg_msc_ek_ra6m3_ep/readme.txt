/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about the Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview
	This example project demonstrates the USB OTG (On-The-Go) functionality of the RA MCU, showcasing its ability to
	dynamically switch between USB host and device roles. The project will perform various USB operations using the
	USB basic driver. The RA board switches roles depending on the type of USB cable connected (standard or OTG).

	In USB Device Mode, the RA board acts as a USB Mass Storage Class (MSC) device using an SD card as block media.
	When connected to a PC via a USB micro cable, the RA board enumerates as a USB storage device. The PC recognizes
	the board as external storage, allowing file management operations such as reading and writing data directly on 
	the PC.

	In USB Host Mode, the RA board operates as a USB MSC host when a USB OTG cable is connected to its USB port along
	with USB flash drive. In this setup, the RA board manages connected USB storage devices (e.g., USB flash drive)
	as peripheral. In this mode, the SD card and USB storage device function as separate file systems. The application
	also supports file system operations such as listing directories, creating files, and copying files between devices.

	The terminal displays real-time updates of USB operations and system status. The available operations include:
		- Listing directories and files on USB or SD media.
		- Formatting USB disk or SD media.
		- Creating files on USB disk or SD media.
		- Copying files between USB disk and SD media.

	Note: 
		- Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via
		  J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults
		  to the SEGGER J-Link RTT Viewer. If supported, it defaults to the serial terminal (UART).
		- To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please refer to the instructions
		  provided in the "Special Topic" section of usb_otg_msc_notes.md.
		- RA boards that support J-Link OB VCOM: EK-RA8M1, EK-RA8D1, and MCK-RA8T1.

2. Software Requirements
	Renesas Flexible Software Package (FSP): Version 6.0.0
	e2 studio: Version 2025-04.1
	SEGGER J-Link RTT Viewer: Version 8.44a
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1.
		For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
			1 x RA Board.
			2 x USB Micro cables.
			2 x USB Micro OTG cables.
			1 x Pmod SD: Full-sized SD Card Slot (e.g., https://digilent.com/shop/pmod-sd-full-sized-sd-card-slot/).
			1 x Full-sized SD card or micro SD card with an adapter.
			11 x Jumper cables (female to female).
			1 x Custom USB pinout board (refer to the schematic design shown in the usb_otg_msc_notes.md file).
			1 x USB Pendrive.
		
		For MCK-RA8T1:
			1 x RA Board.
			1 x USB Type-C cable.
			1 x USB Micro cable.
			1 x USB Micro OTG cable.
			1 x USB Type-C OTG cable.
			1 x Micro SD card.
			1 x Jumper cable (male to female).
			1 x Custom USB pinout board (refer to the schematic design shown in the usb_otg_msc_notes.md file).
			1 x USB Pendrive.

4. Hardware Connections
	a. General Setup
		For EK-RA4M2, EK-RA4M3, EK-RA6M4:
			- Connect the RA board's USB ID pin to the custom board's USB ID pin using a jumper cable.
				- USB ID pin P408 (J1:26) for EK-RA4M2.
				- USB ID pin P408 (J1:38) for EK-RA4M3, EK-RA6M4.
			- Full-Speed (Default):
				- Connect the RA board's USB FS port (J11 connector) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.

		For EK-RA6M3, EK-RA6M3G:
			- Connect the RA board's USB ID pin (Pin 408 - J3:37) to the custom board's USB ID pin using a
			  jumper cable.
			- High-Speed (Default):
				- Connect the RA board's USB HS port (J6 connector) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.
			- Full-Speed:
				- Connect the RA board's USB FS port (J11 connector) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.

		For EK-RA6M5, EK-RA8M1, EK-RA8D1:
			- Connect the RA board's USB ID pin to the custom board's USB ID pin using a jumper cable.
				- USB ID pin P408 (J1:38) for EK-RA6M5
				- USB ID pin P411 (J51:39) for EK-RA8M1
				- USB ID pin P411 (J52:18) for EK-RA8D1
			- High-Speed (Default): 
				- Connect the RA board's USB HS port (J31 connector) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.
			- Full-Speed: 
				- Connect the RA board's USB FS port (J11 connector) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.

		For MCK-RA8T1:
			- Connect the RA board's USB ID pin (Pin 411 - CN6:9) to the custom board's USB ID pin using a
			  jumper cable.
			- Full-Speed (Default): 
				- Connect the RA board's USB FS port (CN14 connector) to the custom board's USB Type-A male
				  port using a USB Type-C OTG cable.

	b. Hardware configuration  
		For EK-RA8D1: Set the configuration switches (SW1) as below.
			+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
			| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
			+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
			|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     OFF    |     OFF     |     OFF   |
			+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

		For EK-RA8M1: Remove jumper J61 to use SHDI channel 0.
	
	c. Pmod SD Setup
		Note: When connecting the PMOD interface, use short wiring connections (should be shorter than 10cm).
		For EK-RA4M2:
			1-Bit Width Connections:  
				SD0CMD  P412 (J1:22)	--->	Pmod SD MOSI (J1:2)
				SD0DAT0 P411 (J1:21)	--->	Pmod SD MISO (J1:3)
				SD0CLK  P413 (J1:19)	--->	Pmod SD SCK  (J1:4)
				GND			--->	Pmod SD GND  (J1:5)
				+3V3(VCC)		--->	Pmod SD VCC  (J1:6)
				SD0CD   P210 (J4:13)	--->	Pmod SD CD   (J1:9)
				SD0WP   P209 (J4:14)	--->	Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections:  
				SD0DAT1 P410 (J1:24)	--->	Pmod SD DAT1 (J1:7)
				SD0DAT2 P206 (J4:9)	--->	Pmod SD DAT2 (J1:8)
				SD0DAT3 P205 (J4:10)	--->	Pmod SD CS   (J1:1)

		For EK-RA4M3, EK-RA6M4:
			1-Bit Width Connections:  
				SD0CMD  P412 (J1:P34)   --->    Pmod SD MOSI (J1:2)
				SD0DAT0 P411 (J1:P35)   --->    Pmod SD MISO (J1:3)
				SD0CLK  P413 (J1:P33)   --->    Pmod SD SCK  (J1:4)
				GND                     --->    Pmod SD GND  (J1:5)
				+3V3(VCC)             	--->    Pmod SD VCC  (J1:6)
				SD0CD   P210 (J4:P19)   --->    Pmod SD CD   (J1:9)
				SD0WP   P20  (J4:P20)   --->    Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections:  
				SD0DAT1 P410 (J1:36)	--->	Pmod SD DAT1 (J1:7)
				SD0DAT2 P206 (J4:10)	--->	Pmod SD DAT2 (J1:8)
				SD0DAT3 P205 (J4:11)	--->	Pmod SD CS   (J1:1) 

		For EK-RA6M3, EK-RA6M3G:
			1-Bit Width Connections:  
				SD0CMD  P412 (J3:33)	--->	Pmod SD MOSI (J1:2)
				SD0DAT0 P411 (J3:36)	--->	Pmod SD MISO (J1:3)
				SD0CLK  P413 (J3:34)	--->	Pmod SD SCK  (J1:4)
				GND			--->	Pmod SD GND  (J1:5)
				+3V3(VCC)		--->	Pmod SD VCC  (J1:6)
				SD0CD   P415 (J3:32)	--->	Pmod SD CD   (J1:9)
				SD0WP   P414 (J3:31)	--->	Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections: 
				SD0DAT1 P410 (J3:35)	--->	Pmod SD DAT1 (J1:7)
				SD0DAT2 P206 (J1:38)	--->	Pmod SD DAT2 (J1:8)
				SD0DAT3 P205 (J2:4)	--->	Pmod SD CS   (J1:1)

		For EK-RA6M5:
			1-Bit Width Connections:  
				SD0CMD  P412 (J1:34)	--->    Pmod SD MOSI (J1:2)
				SD0DAT0 P411 (J1:35)	--->    Pmod SD MISO (J1:3)
				SD0CLK  P413 (J1:33)	--->    Pmod SD SCK  (J1:4)
				GND			--->    Pmod SD GND  (J1:5)
				+3V3(VCC)		--->    Pmod SD VCC  (J1:6)
				SD0CD   P210 (J2:17)	--->    Pmod SD CD   (J1:9)
				SD0WP   P209 (J2:18)	--->    Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections:  
				SD0DAT1 P410 (J1:36)	--->    Pmod SD DAT1 (J1:7)
				SD0DAT2 P206 (J2:4)	--->    Pmod SD DAT2 (J1:8)
				SD0DAT3 P205 (J2:5)	--->    Pmod SD CS   (J1:1) 
		
		For EK-RA8D1:
			1-Bit Width Connections:  
				SD1CMD  P401 (J51:7)	--->	Pmod SD MOSI (J1:2)
				SD1DAT0 P402 (J52:15)	--->	Pmod SD MISO (J1:3)
				SD1CLK  P400 (J51:10)	--->	Pmod SD SCK  (J1:4)
				GND 			--->	Pmod SD GND  (J1:5)
				+3V3(VCC) 		--->	Pmod SD VCC  (J1:6)
				SD1CD   P406 (J51:5)	--->	Pmod SD CD   (J1:9)
				SD1WP   P700 (J51:6)	--->	Pmod SD WP   (J1:10) 

			4-Bit Width Additional Connections:  
				SD1DAT1 P403 (J51:9)	--->	Pmod SD DAT1 (J1:7)
				SD1DAT2 P404 (J57:1)	--->	Pmod SD DAT2 (J1:8)
				SD1DAT3 P405 (J51:8)	--->	Pmod SD CS   (J1:1) 

		For EK-RA8M1:
			*Note: The user needs to remove jumper J61 to use SHDI channel 0.
			Hardware connection for SDHI channel 0 as below:	
			1-Bit Width Connections: 
				SD0CMD  P307 (J54:5)	--->	Pmod SD MOSI (J1:2)
				SD0DAT0 P304 (J54:9)	--->	Pmod SD MISO (J1:3)
				SD0CLK  P308 (J54:4)	--->	Pmod SD SCK  (J1:4)
				GND			--->	Pmod SD GND  (J1:5)
				+3V3(VCC)		--->	Pmod SD VCC  (J1:6)
				SD0CD   P306 (J54:7)    --->   	Pmod SD CD   (J1:9)
				SD0WP   P305 (J54:8)    --->   	Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections:  
				SD0DAT1 P303 (J54:6)	--->	Pmod SD DAT1 (J1:7)
				SD0DAT2 P302 (J54:14)	--->	Pmod SD DAT2 (J1:8)
				SD0DAT3 P301 (J54:12)	--->	Pmod SD CS   (J1:1)

		For MCK-RA8T1:
			Connect microSD Card to microSD Socket (CN12).

	d. Switching USB Modes 
		For EK-RA4M2, EK-RA4M3, EK-RA6M4:
			- Full-Speed (Default):
				- USB Device Mode:  
					- Connect the jumper J15 pins.  
					- Short Pin 2-3 of jumper J12.  
					- Connect the custom board USB Micro port to the host PC using a USB Micro cable.  
				- USB Host Mode:  
					- Remove the jumper from J15.
					- Short Pin 1-2 of jumper J12.
					- Connect the custom board USB Micro port to a USB storage disk using a USB Micro OTG cable.
					
		For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8D1, EK-RA8M1:
			- High-Speed (Default):
				- USB Device Mode:  
					- Connect the jumper J17 pins.  
					- Short Pin 2-3 of jumper J7.  
					- Connect the custom board USB Micro port to the host PC using a USB Micro cable.  
				- USB Host Mode:  
					- Remove the jumper from J17.
					- Short Pin 1-2 of jumper J7.
					- Connect the custom board USB Micro port to a USB storage disk using a USB Micro OTG cable.
					
			- Full-Speed:
				- USB Device Mode:  
					- Connect the jumper J15 pins.  
					- Short Pin 2-3 of jumper J12.  
					- Connect the custom board USB micro port to the host PC using a USB Micro cable.  
				- USB Host Mode:  
					- Remove the jumper from J15.
					- Short Pin 1-2 of jumper J12.
					- Connect the custom board USB micro port to a USB storage disk using a USB Micro OTG cable.

		- MCK-RA8T1
			- Full-Speed (Default):
				- USB Device Mode:  
					- Connect the jumper JP10 pins.  
					- Short Pin 2-3 of jumper JP9.  
					- Connect the custom board USB Micro port to the host PC using a USB Micro cable.  
				- USB Host Mode:  
					- Remove the jumper from JP10.
					- Short Pin 1-2 of jumper JP9.
					- Connect the custom board USB Micro port to a USB storage disk using a USB Micro OTG cable.

5. Verifying Operation
   	a. Import the project into e2 studio, generate project content, build, and flash the firmware onto the RA board 
	   using a Micro USB cable.  
   	b. Open a terminal application (e.g., Tera Term or J-Link RTT Viewer) on the host PC and connect to the RA board’s
	   debug port. 
	   Note: For using the serial terminal:
           - Please ensure that the connection to the SEGGER J-Link RTT Viewer has been terminated.
           - The COM port is provided by the J-Link onboard, with a baud rate of 115200 bps, a data length of 8 bits,
    	     no parity check, one stop bit, and no flow control.   
   	c. Switch USB Modes: Refer to the "Switching USB Modes" section to configure the board in Device Mode or Host Mode.  
   	d. Verify USB Device Mode:
       	   - The RA board enumerates as a USB MSC device, and the PC recognizes it as external storage.
           - The file system of the SD card should be displayed, and users can perform read/write operations using the PC.
   	e. Verify USB Host Mode:
       	   - The RA board manages the connected USB storage device and the SD card.
           - Use the terminal menu to:
           	- List directories and files on USB and SD media.
           	- Format USB and SD media.
           	- Create and write files.
           	- Copy files between the USB storage device and SD card.  

Note:
    1. Connect PMOD: use short wiring connections (should be shorter than 10cm).
	
    2. Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
       RTT Block address for hex file committed in repository are as follows:
       a. e2studio: 0x1ffe3484
       b. Keil: 	Not Available 
       c. IAR: 	    Not Available
    
    3. If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
       in .map file generated in the build configuration folder (Debug/Release).
