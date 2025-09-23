/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project shows the operation of a FileX interface on block media over SDHI driver on RA MCU. In this
	sample code, the project will perform various file operations (open, close, read, write, format), and optionally
	adding time stamp to the file, while creating the file on the SD card (except for MCK-RA8T1, MCK-RA8T2 which use
	MicroSD card). In summary it showcases the FileX functionality using Block Media SDMMC driver on FSP. J-Link RTT
	Viewer is used as user interface. System messages (error and info messages) will be printed on J-Link RTT Viewer
	during the execution of the project.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M4, FPB-RA6E1, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1, MCK-RA8T2
	1 x Renesas RA board.
	1 x USB cable for programming and debugging.
		- Type-C USB cable: MCK-RA8T1, EK-RA8P1, MCK-RA8T2
		- Micro USB cable: the other boards
	For EK-RA6M3, EK-RA6M4, FPB-RA6E1, EK-RA8M1, EK-RA8D1, EK-RA8P1:
		1 x SD card (e.g., https://shop.sandisk.com/en-sg/products/memory-cards/sd-cards/sandisk-ultra-uhs-i-sd?sku=SDSDUNC-032G-ZN6IN).
		1 x Pmod SD: Full sized SD Card Slot (e.g., https://digilent.com/shop/pmod-sd-full-sized-sd-card-slot/).
		10 x Jumper wires.
	For MCK-RA8T1, MCK-RA8T2:
		1 x MicroSD card (e.g., https://shop.sandisk.com/en-ie/products/memory-cards/microsd-cards/sandisk-ultra-lite-uhs-i-microsd?sku=SDSQUNR-032G-GN3MA).

3. Configuration Settings:
	To select Bus Width in SDHI configuration:
		4 Bits Width:
    			Under SDMMC Stack
        			Bus Width: 4 Bits

		1 Bit Width:
    			Under SDMMC Stack
        			Bus Width: 1 Bit

4. Hardware Connections:
	For EK-RA6M3:
		1 Bit Width
			SD0CMD  P412 (J4:33)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J4:36)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P413 (J4:34)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			VCC +3V3                --->    Pmod SD VCC  (J1:6)
			SD0CD   P415 (J4:32)    --->    Pmod SD CD   (J1:9)
			SD0WP   P414 (J4:31)    --->    Pmod SD WP   (J1:10)

		4 Bits Width
			Above mentioned connections for 1 Bit Width. In addition below connections

			SD0DAT1 P410 (J4:35)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J1:38)    --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J2:04)    --->    Pmod SD CS   (J1:1)

	For EK-RA6M4:
		1 Bit Width
			SD0CMD  P412            --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411            --->    Pmod SD MISO (J1:3)
			SD0CLK  P413            --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			VCC +3V3                --->    Pmod SD VCC  (J1:6)
			SD0CD   P210            --->    Pmod SD CD   (J1:9)
			SD0WP   P209            --->    Pmod SD WP   (J1:10)

		4 Bits Width
			Above mentioned connections for 1 Bit Width. In addition below connections

			SD0DAT1 P410            --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206            --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205            --->    Pmod SD CS   (J1:1)

	For FPB-RA6E1:
		1 Bit Width 
			SD0CMD  P412 (J3:20)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J3:21)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P413 (J3:19)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			VCC  +3V3               --->    Pmod SD VCC  (J1:6)
			SD0CD   P210 (J3:35)    --->    Pmod SD CD   (J1:9)
			SD0WP   P209 (J3:36)    --->    Pmod SD WP   (J1:10)

		4 Bits Width
			Above mentioned connections for 1 Bit Width. In addition below connections

			SD0DAT1 P410 (J3:22)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J3:31)    --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J3:32)    --->    Pmod SD CS   (J1:1)

	For EK-RA8M1:
			* Note: The user needs to remove jumper J61 to use SDHI channel 0.
			Hardware connection for SDHI channel 0 as below:
		1 Bit Width
			SD0CMD  P307 (J54:5)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P304 (J54:9)    --->    Pmod SD MISO (J1:3)
			SD0CLK  P308 (J54:4)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			VCC +3V3                --->    Pmod SD VCC  (J1:6)
			SD0CD   P306 (J54:7)    --->    Pmod SD CD   (J1:9)
			SD0WP   P305 (J54:8)    --->    Pmod SD WP   (J1:10)

		4 Bits Width
			Above mentioned connections for 1 Bit Width. In addition below connections

			SD0DAT1 P303 (J54:6)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P302 (J54:14)   --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P301 (J54:12)   --->    Pmod SD CS   (J1:1)

	For EK-RA8D1:
			* Note: The user must turn-off SW1-3, SW1-5, SW1-8 on board to use SDHI channel 1.
			Hardware connection for SDHI channel 1 as below:
		1 Bit Width
			SD1CMD  P401 (J51:7)    --->    Pmod SD MOSI (J1:2)
			SD1DAT0 P402 (J52:15)   --->    Pmod SD MISO (J1:3)
			SD1CLK  P400 (J51:10)   --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			VCC +3V3                --->    Pmod SD VCC  (J1:6)
			SD1CD   P406 (J51:5)    --->    Pmod SD CD   (J1:9)
			SD1WP   P700 (J51:6)    --->    Pmod SD WP   (J1:10)

		4 Bits Width
			Above mentioned connections for 1 Bit Width. In addition below connections

			SD1DAT1 P403 (J51:9)    --->    Pmod SD DAT1 (J1:7)
			SD1DAT2 P404 (J57:1)    --->    Pmod SD DAT2 (J1:8)
			SD1DAT3 P405 (J51:8)    --->    Pmod SD CS   (J1:1)

	For MCK-RA8T1:
			Connect Micro SD card to Micro SD socket (CN12).

	For EK-RA8P1:
			Hardware connection for SDHI channel 0 as below:
		1 Bit Width
			SD0CMD  PD04 (J2:32)    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 PD03 (J2:31)    --->    Pmod SD MISO (J1:3)
			SD0CLK  PD05 (J2:27)    --->    Pmod SD SCK  (J1:4)
			GND                     --->    Pmod SD GND  (J1:5)
			VCC +3V3                --->    Pmod SD VCC  (J1:6)
			SD0CD   P503 (J37:14)   --->    Pmod SD CD   (J1:9)
			SD0WP   PD06 (J3:4)     --->    Pmod SD WP   (J1:10)

		4 Bits Width
			Above mentioned connections for 1 Bit Width. In addition below connections

			SD0DAT1 PD02 (J2:33)    --->    Pmod SD DAT1 (J1:7)
			SD0DAT2 PD01 (J2:34)    --->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P111 (J1:3)     --->    Pmod SD CS   (J1:1)

	For MCK-RA8T2:
			Connect MicroSD card to MicroSD socket (CN17).

Note:
1) Connect Pmod SD to the RA board: Using short wiring connections (Should be shorter than 10cm).

2) Regarding the power supply pin: By default, Users can use Pmod SD GND (J1:5) and Pmod SD VCC (J1:6) as shown in the
   hardware connection, or alternatively, Pmod SD GND (J1:11) and Pmod SD VCC (J1:12).

3) Format the SD/MicroSD card before performing any operation.

4) EP checks if available space on media is enough to create directory and write 10k pre-defined data.
   The user can modify the size of data to be written through macro WRITE_ITEM_SIZE in application code and the
   space availability for the same will be checked by EP. 
   Space required for a directory creation is 512 bytes. DIR_SPACE_REQUIRED macro can be modified accordingly,
   if additional directories are to be created by the user to check available space on media.

5) User input "3" given once, will display required details. Multiple inputs as "3" may not give expected results.

6) The user is expected to enter data not exceeding 20 bytes in size.

7) Operation is not guaranteed for any user input value other than integer, char (e.g., float, special char) through
   J-Link RTT Viewer input.

8) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220010f0
   b. Keil:     Not Available 
   c. IAR:      Not Available
 
9) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
