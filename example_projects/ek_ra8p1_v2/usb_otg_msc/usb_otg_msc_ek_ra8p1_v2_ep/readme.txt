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
		- RA boards that support J-Link OB VCOM: EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2,
		  MCK-RA8T2, EK-RA8P1 V2.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	SEGGER J-Link RTT Viewer: Version 9.64
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1,
			     MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2, MCK-RA8T2, EK-RA8P1 V2.
	1 x RA Board.
	2 x USB cables:
		- For MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2, MCK-RA8T2, EK-RA8P1 V2: 1 x USB micro cable and
		  1 x USB Type-C cable.
		- Other boards: 2 x USB micro cables.
	2 x USB OTG cables:
		- For MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2, MCK-RA8T2, EK-RA8P1 V2: 1 x USB micro OTG cable
		  and 1 x USB Type-C OTG cable.
		- For other boards: 2 x USB micro OTG cables.
	1 x Custom USB pinout board (refer to "Hardware Requirements" section of usb_otg_msc_notes.md file).
	1 x USB flash drive.
	For MCK-RA8T1, MCK-RA8T2:
		1 x MicroSD card (e.g., https://shop.sandisk.com/en-ie/products/memory-cards/microsd-cards/sandisk-ultra-lite-uhs-i-microsd?sku=SDSQUNR-032G-GN3MA).
		1 x Jumper wire (male to female).
	For other boards:
		1 x Pmod SD: Full-sized SD card slot (e.g., https://digilent.com/shop/pmod-sd-full-sized-sd-card-slot/).
		1 x SD card (e.g., https://shop.sandisk.com/en-sg/products/memory-cards/sd-cards/sandisk-ultra-uhs-i-sd?sku=SDSDUNC-032G-ZN6IN).
		Some jumper wires (Should be shorter than 10cm).

4. Hardware Connections:
	a. General setup
		For EK-RA4M2, EK-RA4M3, EK-RA6M4:
			- Connect the RA board's USB ID pin to the custom board's USB ID pin using a jumper wire.
				- USB ID pin P408 (J1:26) for EK-RA4M2.
				- USB ID pin P408 (J1:38) for EK-RA4M3, EK-RA6M4.
			- Full-Speed (Default):
				- Connect the RA board's USB FS port (J11) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.

		For EK-RA6M3, EK-RA6M3G:
			- Connect the RA board's USB ID pin P408 (J3:37) to the custom board's USB ID pin using a
			  jumper wire.
			- High-Speed (Default):
				- Connect the RA board's USB HS port (J6) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.
			- Full-Speed:
				- Connect the RA board's USB FS port (J11) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.

		For EK-RA6M5, EK-RA8M1, EK-RA8D1:
			- Connect the RA board's USB ID pin to the custom board's USB ID pin using a jumper wire.
				- USB ID pin P408 (J1:38) for EK-RA6M5.
				- USB ID pin P411 (J51:39) for EK-RA8M1.
				- USB ID pin P411 (J52:18) for EK-RA8D1.
			- High-Speed (Default):
				- Connect the RA board's USB HS port (J31) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.
			- Full-Speed:
				- Connect the RA board's USB FS port (J11) to the custom board's USB Type-A male
				  port using a USB Micro OTG cable.

		For MCK-RA8T1:
			- Connect the RA board's USB ID pin P411 (CN6:9) to the custom board's USB ID pin using a
			  jumper wire.
			- Full-Speed (Default):
				- Connect the RA board's USB FS port (CN14) to the custom board's USB Type-A male
				  port using a USB Type-C OTG cable.

		For EK-RA8P1, EK-RA8D2:
			- Connect the RA board's USB ID pin P411 (J17:15) to the custom board's USB ID pin using a
			  jumper wire.
			- High-Speed (Default):
				- Connect the RA board's USB HS port (J7) to the custom board's USB Type-A male
				  port using a USB Type-C OTG cable.
			- Full-Speed:
				- Connect the RA board's USB FS port (J11) to the custom board's USB Type-A male
			  	  port using a USB Type-C OTG cable.

		For EK-RA8M2:
			- Connect the RA board's USB ID pin P411 (J2:8) to the custom board's USB ID pin using a
			  jumper wire.
			- High-Speed (Default):
				- Connect the RA board's USB HS port (J7) to the custom board's USB Type-A male
				  port using a USB Type-C OTG cable.
			- Full-Speed:
				- Connect the RA board's USB FS port (J11) to the custom board's USB Type-A male
			  	  port using a USB Type-C OTG cable.

		For MCK-RA8T2:
			- Connect the RA board's USB ID pin P411 (CN6:4) to the custom board's USB ID pin using a
			  jumper wire.
			- Full-Speed (Default):
				- Connect the RA board's USB FS port (CN18) to the custom board's USB Type-A male
				  port using a USB Type-C OTG cable.

		For EK-RA8P1 V2:
			- Connect the RA board's USB ID pin P411 (J2:13) to the custom board's USB ID pin using a
			  jumper wire.
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

		For EK-RA8M1: Remove jumper J61 to use SHDI channel 0.

		For EK-RA8M2:
			The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
			3-4, 5-6, 7-8 to use the on-board debug functionality.

		For EK-RA8P1 V2:
			The user must set the configuration switches (SW6) as below to use the on-board
			debug functionality.
			+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
			| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
			+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
			|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
			+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
			The user must open E9, E17 to use PD05, PD04 for SDHI channel 0.

	c. Pmod SD setup
		Note: When connecting to the Pmod SD, using short wiring connections (Should be shorter than 10cm).
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
				SD0WP   P209 (J4:P20)   --->    Pmod SD WP   (J1:10)

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
			Connect MicroSD card to MicroSD socket (CN12).

		For EK-RA8P1, EK-RA8D2:
			1-Bit Width Connections:
				SD1CMD  P401 (J17:5)	--->	Pmod SD MOSI (J1:2)
				SD1DAT0 P402 (J17:14)	--->	Pmod SD MISO (J1:3)
				SD1CLK  P400 (J17:6)	--->	Pmod SD SCK  (J1:4)
				GND			--->	Pmod SD GND  (J1:5)
				+3V3(VCC)		--->	Pmod SD VCC  (J1:6)
				SD1CD   P406 (J41:1)	--->	Pmod SD CD   (J1:9)
				SD1WP   P700 (J3:9)	--->	Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections:
				SD1DAT1 P403 (J17:7)	--->	Pmod SD DAT1 (J1:7)
				SD1DAT2 P404 (J17:10)	--->	Pmod SD DAT2 (J1:8)
				SD1DAT3 P811 (J3:21)	--->	Pmod SD CS   (J1:1)

		For MCK-RA8T2:
			Connect MicroSD card to MicroSD socket (CN17).

		For EK-RA8M2:
			1-Bit Width Connections:
				SD1CMD  P202 (J3:29)    --->	Pmod SD MOSI (J1:2)
				SD1DAT0 P313 (J3:30)    --->	Pmod SD MISO (J1:3)
				SD1CLK  P203 (J3:31)    --->	Pmod SD SCK  (J1:4)
				GND          (J3:39)    --->	Pmod SD GND  (J1:5)
				+3V3    VCC  (J3:1)     --->	Pmod SD VCC  (J1:6)
				SD1CD   P205 (J3:35)    --->	Pmod SD CD   (J1:9)
				SD1WP   P204 (J3:33)    --->	Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections:
				SD1DAT1 P314 (J3:27)    --->	Pmod SD DAT1 (J1:7)
				SD1DAT2 P810 (J3:13)    --->	Pmod SD DAT2 (J1:8)
				SD1DAT3 P811 (J3:15)    --->	Pmod SD CS   (J1:1)

		For EK-RA8P1 V2:
			1-Bit Width Connections:
				SD0CMD  PD04 (J17:7)	--->	Pmod SD MOSI (J1:2)
				SD0DAT0 P304 (J6:13) 	---> 	Pmod SD MISO (J1:3)
				SD0CLK  PD05 (J17:19) 	---> 	Pmod SD SCK  (J1:4)
				GND          (J2:27) 	---> 	Pmod SD GND  (J1:5)
				VCC +3V3     (J2:1) 	---> 	Pmod SD VCC  (J1:6)
				SD0CD   P503 (J37:13) 	---> 	Pmod SD CD   (J1:9)
				SD0WP   PD06 (J2:2) 	---> 	Pmod SD WP   (J1:10)

			4-Bit Width Additional Connections:
				SD0DAT1 P302 (J14:25)  	---> 	Pmod SD DAT1 (J1:7)
				SD0DAT2 PD01 (J2:22) 	---> 	Pmod SD DAT2 (J1:8)
				SD0DAT3 P300 (J37:26) 	---> 	Pmod SD CS   (J1:1)

	d. Switching USB modes
		For EK-RA4M2, EK-RA4M3, EK-RA6M4:
			Full-Speed (Default):
				USB Device Mode:
					- Connect the jumper J15 pins.
					- Short Pin 2-3 of jumper J12.
					- Connect the custom board USB Micro port to the host PC using a USB Micro cable.
				USB Host Mode:
					- Remove the jumper from J15.
					- Short Pin 1-2 of jumper J12.
					- Connect the custom board USB Micro port to a USB storage disk using a USB Micro
					  OTG cable.
 
		For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8D1, EK-RA8M1:
			High-Speed (Default):
				USB Device Mode:
					- Connect the jumper J17 pins.
					- Short Pin 2-3 of jumper J7.
					- Connect the custom board USB micro port to the host PC using a USB micro cable.
				USB Host Mode:
					- Remove the jumper from J17.
					- Short Pin 1-2 of jumper J7.
					- Connect the custom board USB micro port to a USB flash drive using a USB micro
					  OTG cable.

			Full-Speed:
				USB Device Mode:
					- Connect the jumper J15 pins.
					- Short Pin 2-3 of jumper J12.
					- Connect the custom board USB micro port to the host PC using a USB micro cable.
				USB Host Mode:
					- Remove the jumper from J15.
					- Short Pin 1-2 of jumper J12.
					- Connect the custom board USB micro port to a USB flash drive using a USB micro
					  OTG cable.

		For MCK-RA8T1:
			Full-Speed (Default):
				USB Device Mode:
					- Connect the jumper JP10 pins.
					- Short Pin 2-3 of jumper JP9.
					- Connect the custom board USB Micro port to the host PC using a USB Micro cable.
				USB Host Mode:
					- Remove the jumper from JP10.
					- Short Pin 1-2 of jumper JP9.
					- Connect the custom board USB Micro port to a USB storage disk using a USB Micro
					  OTG cable.

		For EK-RA8P1, EK-RA8D2, EK-RA8M2, EK-RA8P1 V2:
			High-Speed (Default):
				USB Device Mode:
					- Connect the custom board USB micro port to the host PC using a USB micro cable.
				USB Host Mode:
					- Connect the custom board USB micro port to a USB flash drive using a USB micro
					  OTG cable.

			Full-Speed:
				USB Device Mode:
					- Connect the custom board USB micro port to the host PC using a USB micro cable.
				USB Host Mode:
					- Connect the custom board USB micro port to a USB flash drive using a USB micro
					  OTG cable.

		For MCK-RA8T2:
			Full-Speed (Default):
				USB Device Mode:
					- Connect the jumper JP7 pins.
					- Short Pin 2-3 of jumper JP6.
					- Connect the custom board USB Micro port to the host PC using a USB Micro cable.
				USB Host Mode:
					- Remove the jumper from JP7.
					- Short Pin 1-2 of jumper JP6.
					- Connect the custom board USB Micro port to a USB storage disk using a USB Micro
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
		- The file system of the MicroSD/SD card should be displayed, and users can perform read/write operations
		  using the PC.
		Note: The user should use the host PC with full access permissions to perform any file operations on the disk
		      (e.g., create, write, read, open, delete).
	e. Verify USB Host Mode:
		- The RA board manages the connected USB storage device and the MicroSD/SD card.
		- Use the terminal menu to:
			- List directories and files on USB and SD media.
			- Format USB and SD media.
			- Create and write files.
			- Copy files between the USB storage device and MicroSD/SD card.

Note:
1. Connect to Pmod: Using short wiring connections (Should be shorter than 10 cm).

2. For using the serial terminal application: The macro USE_VIRTUAL_COM is set to 1.
	a. To echo back typed content in Tera Term, go to [Setup] -> [Terminal...] and check [Local echo].
	b. The configuration parameters of the serial port are:
		- Port: Provided by the J-Link on-board.
		- Speed: 115200
		- Data: 8 bit
		- Parity: none
		- Stop bits: 1 bit
		- Flow control: none

3. For using J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
