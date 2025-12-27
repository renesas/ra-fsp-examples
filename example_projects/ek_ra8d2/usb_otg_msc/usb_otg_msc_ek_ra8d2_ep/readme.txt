/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about the Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the USB OTG (On-The-Go) functionality of the RA MCU, showcasing its
	ability to dynamically switch between USB host and device roles. The project will perform various USB
	operations using the USB basic driver. The RA board switches roles depending on the type of USB cable
	connected (standard or OTG).

	In USB Device Mode, the RA board acts as a USB Mass Storage Class (MSC) device using an SD card as
	block media. When connected to a PC via a USB micro cable, the RA board enumerates as a USB storage device.
	The PC recognizes the board as external storage, allowing file management operations such as reading and
	writing data directly on the PC.

	In USB Host Mode, the RA board operates as a USB MSC host when a USB OTG cable is connected to its USB
	port along with USB flash drive. In this setup, the RA board manages connected USB storage devices
	(e.g., USB flash drive) as peripheral. In this mode, the SD card and USB storage device function as separate
	file systems. The application also supports file system operations such as listing directories, creating files,
	and copying files between devices.

	The terminal displays real-time updates of USB operations and system status. The available operations include:
		- Listing directories and files on USB or SD media.
		- Formatting USB disk or SD media.
		- Creating files on USB disk or SD media.
		- Copying files between USB disk and SD media.

	Note:
		- Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART)
		  via J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project
		  defaults to the SEGGER J-Link RTT Viewer. If supported, it defaults to the serial terminal (UART).
		- To use the SEGGER J-Link RTT Viewer instead of the serial terminal, please refer to the instructions
		  provided in the "Special Topic" section of usb_otg_msc_notes.md.
		- RA boards that support J-Link OB VCOM: EK-RA8D1, EK-RA8P1, EK-RA8D2.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	SEGGER J-Link RTT Viewer: Version 8.92
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA8D1, EK-RA8P1, EK-RA8D2.
	1 x RA Board.
	2 x USB cables:
		- For EK-RA8P1, EK-RA8D2: 1 x USB micro cable and 1 x USB Type-C cable.
		- Other boards: 2 x USB micro cables.
	2 x USB OTG cables:
		- For EK-RA8P1, EK-RA8D2: 1 x USB micro OTG cable and 1 x USB Type-C OTG cable.
		- For other boards: 2 x USB micro OTG cables.
	1 x Pmod SD: Full-sized SD Card Slot (e.g., https://digilent.com/shop/pmod-sd-full-sized-sd-card-slot/).
	1 x Full-sized SD card or micro SD card with an adapter.
	11 x Jumper cables (female to female).
	1 x Custom USB pinout board (refer to "Hardware Requirements" section of usb_otg_msc_notes.md file).
	1 x USB flash drive.

4. Hardware Connections:
	a. General setup
		For EK-RA8D1:
			- Connect the RA board's USB ID pin P411 (J52:18) to the custom board's USB ID pin using a
			  jumper cable.
			- High-Speed (Default):
				- Connect the RA board's USB HS port (J31) to the custom board's USB Type-A male
				  port using a USB micro OTG cable.
			- Full-Speed: 
				- Connect the RA board's USB FS port (J11) to the custom board's USB Type-A male
				  port using a USB micro OTG cable.

		For EK-RA8P1, EK-RA8D2:
			- Connect the RA board's USB ID pin P411 (J17:15) to the custom board's USB ID pin using a
			  jumper cable.
			- High-Speed (Default):
				- Connect the RA board's USB HS port (J7) to the custom board's USB Type-A male
				  port using a USB Type-C OTG cable.
			- Full-Speed:
				- Connect the RA board's USB FS port (J11) to the custom board's USB Type-A male
			  	  port using a USB Type-C OTG cable.

	b. Hardware configuration  
		For EK-RA8D1: Set the configuration switches (SW1) as below.
			+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
			| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
			+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
			|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     OFF    |     OFF     |     OFF   |
			+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	
	c. Pmod SD setup
		Note: When connecting the PMOD interface, use short wiring connections (should be shorter than 10 cm).
		For EK-RA8D1:
			1-Bit Width Connections:
				SD1CMD  P401 (J51:7)   --->   Pmod SD MOSI (J1:2)
				SD1DAT0 P402 (J52:15)  --->   Pmod SD MISO (J1:3)
				SD1CLK  P400 (J51:10)  --->   Pmod SD SCK  (J1:4)
				GND                    --->   Pmod SD GND  (J1:5)
				+3V3    VCC            --->   Pmod SD VCC  (J1:6)
				SD1CD   P406 (J51:5)   --->   Pmod SD CD   (J1:9)
				SD1WP   P700 (J51:6)   --->   Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections:
				SD1DAT1 P403 (J51:9)   --->   Pmod SD DAT1 (J1:7)
				SD1DAT2 P404 (J57:1)   --->   Pmod SD DAT2 (J1:8)
				SD1DAT3 P405 (J51:8)   --->   Pmod SD CS   (J1:1)

		For EK-RA8P1, EK-RA8D2:
			1-Bit Width Connections:
				SD1CMD  P401 (J17:5)    --->   Pmod SD MOSI (J1:2)
				SD1DAT0 P402 (J17:14)   --->   Pmod SD MISO (J1:3)
				SD1CLK  P400 (J17:6)    --->   Pmod SD SCK  (J1:4)
				GND          (J17:19)   --->   Pmod SD GND  (J1:5)
				+3V3    VCC  (J17:1)    --->   Pmod SD VCC  (J1:6)
				SD1CD   P406 (J41:1)    --->   Pmod SD CD   (J1:9)
				SD1WP   P700 (J3:9)     --->   Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections:
				SD1DAT1 P403 (J17:7)    --->   Pmod SD DAT1 (J1:7)
				SD1DAT2 P404 (J17:10)   --->   Pmod SD DAT2 (J1:8)
				SD1DAT3 P811 (J3:21)    --->   Pmod SD CS   (J1:1)

	d. Switching USB Modes
		For EK-RA8D1, EK-RA8P1, EK-RA8D2:
			High-Speed (Default):
				USB Device Mode:
					- For EK-RA8D1, the user needs to configure the following jumpers:
						+ Connect the jumper J17 pins.
						+ Short Pin 2-3 of jumper J7.
					- Connect the custom board USB micro port to the host PC using a USB micro cable.
				USB Host Mode:
					- For EK-RA8D1, the user needs to configure the following jumpers:
						+ Remove the jumper from J17.
						+ Short Pin 1-2 of jumper J7.
					- Connect the custom board USB micro port to a USB flash drive using a USB micro
					  OTG cable.
			   
			Full-Speed:
				USB Device Mode:
					- For EK-RA8D1, the user needs to configure the following jumpers:
						+ Connect the jumper J15 pins.
						+ Short Pin 2-3 of jumper J12.
					- Connect the custom board USB micro port to the host PC using a USB micro cable.
				USB Host Mode:
					- For EK-RA8D1, the user needs to configure the following jumpers:
						+ Remove the jumper from J15.
						+ Short Pin 1-2 of jumper J12.
					- Connect the custom board USB micro port to a USB flash drive using a USB micro
					  OTG cable.

5. Verifying Operation:
	a. Import the project into e2studio, generate project content, build, and flash the firmware onto the RA board 
	   using a Type-C USB cable.
	   Note: Make sure all hardware connections and settings in the "Hardware Connections" section are complete before
	         proceeding.
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
		Note: The user should use the host PC with full access permissions to perform any file operations on the disk
		      (e.g., create, write, read, open, delete).
	e. Verify USB Host Mode:
		- The RA board manages the connected USB storage device and the SD card.
		- Use the terminal menu to:
			- List directories and files on USB and SD media.
			- Format USB and SD media.
			- Create and write files.
			- Copy files between the USB storage device and SD card.

Note:
1. Connect PMOD: use short wiring connections (should be shorter than 10 cm).

2. For the serial terminal application: The macro USE_VIRTUAL_COM is set to 1.
   a. To echo back typed content in Tera Term, go to [Setup] -> [Terminal...] and check [Local echo].
   b. The configuration parameters of the serial port are:
		- COM port: Provided by the J-Link on-board.
		- Baud rate: 115200 bps
		- Data length: 8 bit
		- Parity: None
		- Stop bit: 1 bit
		- Flow control: None

3. For using J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
