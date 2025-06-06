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
   the board as external storage, allowing file management operations such as reading and writing data directly on the PC.

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
	- Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, 
	  depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer;
      if supported, it defaults to the serial terminal (UART).
	- To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please refer to the instructions provided in 
	  the 'Special Topic' section of usb_otg_msc_notes.md.

2. Software Requirements
   - Renesas Flexible Software Package (FSP): Version 5.9.0
   - e2 studio: Version 2025-04
   - GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
   - Serial Terminal Console Application: Tera Term or a similar application
   - SEGGER J-Link RTT Viewer: Version 8.12f

3. Hardware Requirements
	Supported RA Boards: EK-RA8D1
	1 x RA Board: EK-RA8D1
	2 x Micro USB cables
	2 x USB OTG cables
	1 x PmodSD module
	1 x Full-sized SD card or micro SD card with an adapter
	11 x Jumper cables (female to female)
	1 x Custom USB pinout board (refer to the schematic design shown in the usb_otg_msc_notes.md file)

4. Hardware Connections
   a. General Setup 
   - EK-RA8D1:
	   - Connect the RA board's USB ID pin (Pin 411 - J52:P18) to the custom board's USB ID pin using a jumper cable.
	   - High-Speed (Default): 
		   - Connect the RA board's USB HS port (J31 connector) to the custom board's USB Type-A female port using a USB OTG cable.
	   - Full-Speed: 
		   - Connect the RA board's USB FS port (J11 connector) to the custom board's USB Type-A female port using a USB OTG cable.
   b. Hardware configuration  
   - EK-RA8D1: Set the configuration switches (SW1) as below.
   	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     OFF    |     OFF     |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
   
   c. PmodSD Setup
   Note: Connect PMOD: use short wiring connections (should be shorter than 10cm).
   - EK-RA8D1:
	   1-Bit Width Connections:  
	   - P401 (CMD) (J51 Pin7) --> PMOD SD Pin2 (MOSI)  
	   - P402 (DAT0) (J52 Pin15) --> PMOD SD Pin3 (MISO)  
	   - P400 (CLK) (J51 Pin10) --> PMOD SD Pin4 (SCLK)  
	   - GND --> PMOD SD Pin5 (GND)  
	   - +3V3 (VCC) --> PMOD SD Pin6 (VCC)  
	   - P406 (CD) (J51 Pin5) --> PMOD SD Pin9 (CD)  
	   - P700 (WP) (J51 Pin6) --> PMOD SD Pin10 (WP)  

	   4-Bit Width Additional Connections:  
	   - P403 (DAT1) (J51 Pin9) --> PMOD SD Pin7 (DAT1)  
	   - P404 (DAT2) (J57 Pin1) --> PMOD SD Pin8 (DAT2)  
	   - P405 (DAT3) (J51 Pin8) --> PMOD SD Pin1 (CS)  

   d. Switching USB Modes 
   - EK-RA8D1:
	   - High-Speed (Default):
		   - USB Device Mode:  
			   - Connect the jumper J17 pins.  
			   - Short Pin 2-3 of jumper J7.  
			   - Connect the custom board USB micro port to the host PC using a USB micro cable.  
		   - USB Host Mode:  
			   - Remove the jumper from J17.
			   - Short Pin 1-2 of jumper J7.
			   - Connect the custom board USB micro port to a USB storage disk using a USB OTG cable.
			   
	   - Full-Speed:
		   - USB Device Mode:  
			   - Connect the jumper J15 pins.  
			   - Short Pin 2-3 of jumper J12.  
			   - Connect the custom board USB micro port to the host PC using a USB micro cable.  
		   - USB Host Mode:  
			   - Remove the jumper from J15.
			   - Short Pin 1-2 of jumper J12.
			   - Connect the custom board USB micro port to a USB storage disk using a USB OTG cable.

5. Verifying Operation
   1. Import the project into e2 studio, generate project content, build, and flash the firmware onto the RA board using a micro USB cable.  
   2. Open a terminal application (e.g., Tera Term or SEGGER RTT Viewer) on the host PC and connect to the RA board’s debug port.
   - Note: For using the serial terminal:
        - Please ensure that the connection to the SEGGER J-Link RTT Viewer has been terminated.
        - The COM port is provided by the J-Link onboard, with a baud rate of 115200 bps, a data length of 8 bits, no parity check,
		one stop bit, and no flow control.   
   3. Switch USB Modes: Follow the "Switching USB Modes" section to configure the board in Device Mode or Host Mode.  
   4. Verify USB Device Mode:
       - The RA board enumerates as a USB MSC device, and the PC recognizes it as external storage.
       - The file system of the SD card should be displayed, and users can perform read/write operations using the PC.  
   5. Verify USB Host Mode:
       - The RA board manages the connected USB storage device and the SD card.
       - Use the terminal menu to:
           - List directories and files on USB and SD media.
           - Format USB and SD media.
           - Create and write files.
           - Copy files between the USB storage device and SD card.  

Note:
	1. Connect PMOD: use short wiring connections (should be shorter than 10cm).
	
	2. For the Serial Terminal application: The macro USE_VIRTUAL_COM is set to 1.
		a. To echo back typed content in Tera Term, go to [Setup] -> [Terminal...] and check [Local echo].
		b. The configuration parameters of the serial port are:
			- COM port: Provided by the J-Link on-board.
			- Baud rate: 115200 bps
			- Data length: 8 bits
			- Parity: None
			- Stop bit: 1 bit
			- Flow control: None