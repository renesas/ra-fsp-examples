/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of SDHI driver with FreeRTOS+FAT file system on Renesas
	RA MCUs based on Renesas FSP. FreeRTOS+FAT uses the underlying Block media driver which utilizes SDHI driver to
	perform file operations on the SD Card (except for MCK-RA8T1, MCK-RA8T2 which use MicroSD Card). Error and info
	messages will be printed on J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, FPB-RA6E1, EK-RA8M1,
	                     EK-RA8D1, MCK-RA8T1, EK-RA8P1, MCK-RA8T2, EK-RA8M2.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	For MCK-RA8T1, MCK-RA8T2:
		1 x Micro SD Card (e.g., https://shop.sandisk.com/en-ie/products/memory-cards/microsd-cards/sandisk-ultra-lite-uhs-i-microsd?sku=SDSQUNR-032G-GN3MA).
	For all supported boards except MCK-RA8T1 and MCK-RA8T2:
		1 x SD Card (e.g., https://shop.sandisk.com/en-sg/products/memory-cards/sd-cards/sandisk-ultra-uhs-i-sd?sku=SDSDUNC-032G-ZN6IN).
		1 x Pmod SD: Full sized SD Card Slot (e.g., https://digilent.com/shop/pmod-sd-full-sized-sd-card-slot/).
		10 x Jumper wires.

3. Software Configuration:
	Change the Bus Width properties in the configurator:
	For 4 Bits Width:
		SD/MMC (r_sdhi) Stack | Bus Width: 4 Bits

	For 1 Bit Width:
		SD/MMC (r_sdhi) Stack | Bus Width: 1 Bit

4. Hardware Connections:
	For EK-RA6M3, EK-RA6M3G:
		For 1 Bit Width:
			SD0CMD  P412    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411   	--->    Pmod SD MISO (J1:3)
			SD0CLK  P413   	--->    Pmod SD SCK  (J1:4)
			GND          	--->    Pmod SD GND  (J1:5)
			VCC +3V3  	--->    Pmod SD VCC  (J1:6)
			SD0CD   P415  	--->    Pmod SD CD   (J1:9)
			SD0WP   P414  	--->    Pmod SD WP   (J1:10)

		For 4 Bits Width:
			Above mentioned connections for 1 Bit Width. In addition below connections
			SD0DAT1 P410   	--->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206   	--->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205   	--->    Pmod SD CS   (J1:1)

	For EK-RA6M1, EK-RA6M2, EK-RA6M4:
		For 1 Bit Width:
			SD0CMD  P412    --->    Pmod SD MOSI (J1:2)
			SD0DAT0 P411   	--->    Pmod SD MISO (J1:3)
			SD0CLK  P413   	--->    Pmod SD SCK  (J1:4)
			GND          	--->    Pmod SD GND  (J1:5)
			VCC +3V3    	--->    Pmod SD VCC  (J1:6)
			SD0CD   P210   	--->    Pmod SD CD   (J1:9)
			SD0WP   P209   	--->    Pmod SD WP   (J1:10)

		For 4 Bits Width:
			Above mentioned connections for 1 Bit Width. In addition below connections
			For EK-RA6M1, EK-RA6M2: Tracecut Jumper E15 and solder to E12
			SD0DAT1 P410   	--->    Pmod SD DAT1 (J1:7)
			SD0DAT2 P206   	--->    Pmod SD DAT2 (J1:8)
			SD0DAT3 P205   	--->    Pmod SD CS   (J1:1)

	For EK-RA6M5:
		For 1 Bit Width:
			SD0CMD  P412 (J1:34)	--->	Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J1:35) 	---> 	Pmod SD MISO (J1:3)
			SD0CLK  P413 (J1:33) 	---> 	Pmod SD SCK  (J1:4)
			GND 			---> 	Pmod SD GND  (J1:5)
			VCC +3V3 		---> 	Pmod SD VCC  (J1:6)
			SD0CD   P210 (J2:17) 	---> 	Pmod SD CD   (J1:9)
			SD0WP   P209 (J2:18) 	---> 	Pmod SD WP   (J1:10)

		For 4 Bits Width:
			Above mentioned connections for 1 Bit Width.In addition below connections
			SD0DAT1 P410 (J1:36) 	---> 	Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J2:4) 	---> 	Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J2:5) 	---> 	Pmod SD CS   (J1:1)

	For FPB-RA6E1:
		For 1 Bit Width:
			SD0CMD  P412 (J3:20) 	---> 	Pmod SD MOSI (J1:2)
			SD0DAT0 P411 (J3:21) 	---> 	Pmod SD MISO (J1:3)
			SD0CLK  P413 (J3:19) 	---> 	Pmod SD SCK  (J1:4)
			GND 			---> 	Pmod SD GND  (J1:5)
			VCC  +3V3 		---> 	Pmod SD VCC  (J1:6)
			SD0CD   P210 (J3:35) 	---> 	Pmod SD CD   (J1:9)
			SD0WP   P209 (J3:36) 	---> 	Pmod SD WP   (J1:10)

		For 4 Bits Width:
			Above mentioned connections for 1 Bit Width. In addition below connections
			SD0DAT1 P410 (J3:22) 	---> 	Pmod SD DAT1 (J1:7)
			SD0DAT2 P206 (J3:31) 	---> 	Pmod SD DAT2 (J1:8)
			SD0DAT3 P205 (J3:32) 	---> 	Pmod SD CS   (J1:1)

	For EK-RA8M1:
		Note: The user needs to remove jumper J61 to use SDHI channel 0.
		Hardware connection for SDHI channel 0 as below:
		For 1 Bit Width:
			SD0CMD  P307 (J54:5) 	---> 	Pmod SD MOSI (J1:2)
			SD0DAT0 P304 (J54:9) 	---> 	Pmod SD MISO (J1:3)
			SD0CLK  P308 (J54:4) 	---> 	Pmod SD SCK  (J1:4)
			GND 			---> 	Pmod SD GND  (J1:5)
			VCC +3V3 		---> 	Pmod SD VCC  (J1:6)
			SD0CD   P306 (J54:7) 	---> 	Pmod SD CD   (J1:9)
			SD0WP   P305 (J54:8) 	---> 	Pmod SD WP   (J1:10)

		For 4 Bits Width:
			Above mentioned connections for 1 Bit Width. In addition below connections
			SD0DAT1 P303 (J54:6)  	---> 	Pmod SD DAT1 (J1:7)
			SD0DAT2 P302 (J54:14) 	---> 	Pmod SD DAT2 (J1:8)
			SD0DAT3 P301 (J54:12) 	---> 	Pmod SD CS   (J1:1)

	For EK-RA8D1:
		Note: The user must turn OFF SW1-3, SW1-5, SW1-8 on board to use SDHI channel 1.
		Hardware connection for SDHI channel 1 as below:
		For 1 Bit Width:
			SD1CMD  P401 (J51:7) 	---> 	Pmod SD MOSI (J1:2)
			SD1DAT0 P402 (J52:15) 	---> 	Pmod SD MISO (J1:3)
			SD1CLK  P400 (J51:10) 	---> 	Pmod SD SCK  (J1:4)
			GND 			---> 	Pmod SD GND  (J1:5)
			VCC +3V3 		---> 	Pmod SD VCC  (J1:6)
			SD1CD   P406 (J51:5) 	---> 	Pmod SD CD   (J1:9)
			SD1WP   P700 (J51:6) 	---> 	Pmod SD WP   (J1:10)

		For 4 Bits Width:
			Above mentioned connections for 1 Bit Width. In addition below connections
			SD1DAT1 P403 (J51:9)  	---> 	Pmod SD DAT1 (J1:7)
			SD1DAT2 P404 (J57:1)  	---> 	Pmod SD DAT2 (J1:8)
			SD1DAT3 P405 (J51:8)  	---> 	Pmod SD CS   (J1:1)

	For MCK-RA8T1
		Connect microSD Card to microSD Socket (CN12).

	For EK-RA8P1:
		Hardware connection for SDHI channel 0 as below:
		For 1 Bit Width:
			SD0CMD  PD04 (J2:32)	--->	Pmod SD MOSI (J1:2)
			SD0DAT0 PD03 (J2:31) 	---> 	Pmod SD MISO (J1:3)
			SD0CLK  PD05 (J2:27) 	---> 	Pmod SD SCK  (J1:4)
			GND 	     (J2:39)  	---> 	Pmod SD GND  (J1:5)
			VCC +3V3     (J2:1)	---> 	Pmod SD VCC  (J1:6)
			SD0CD   P503 (J37:14) 	---> 	Pmod SD CD   (J1:9)
			SD0WP   PD06 (J3:4) 	---> 	Pmod SD WP   (J1:10)

		For 4 Bits Width:
			Above mentioned connections for 1 Bit Width. In addition below connections
			SD0DAT1 PD02 (J2:33)  	---> 	Pmod SD DAT1 (J1:7)
			SD0DAT2 PD01 (J2:34) 	---> 	Pmod SD DAT2 (J1:8)
			SD0DAT3 P111 (J1:3) 	---> 	Pmod SD CS   (J1:1)

	For MCK-RA8T2
		Connect microSD Card to microSD Socket (CN17).

	For EK-RA8M2:
		Note: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		      3-4, 5-6, 7-8 to use the on-board debug functionality.

		Hardware connection for SDHI channel 1 as below:
		For 1 Bit Width:
			SD1CMD  P202 (J3:29)	--->	Pmod SD MOSI (J1:2)
			SD1DAT0 P313 (J3:30) 	---> 	Pmod SD MISO (J1:3)
			SD1CLK  P203 (J3:31) 	---> 	Pmod SD SCK  (J1:4)
			GND 	     (J3:39)  	---> 	Pmod SD GND  (J1:5)
			VCC +3V3     (J3:1)	---> 	Pmod SD VCC  (J1:6)
			SD1CD   P205 (J3:35) 	---> 	Pmod SD CD   (J1:9)
			SD1WP   P204 (J3:33) 	---> 	Pmod SD WP   (J1:10)

		For 4 Bits Width:
			Above mentioned connections for 1 Bit Width. In addition below connections
			SD1DAT1 P314 (J3:27)  	---> 	Pmod SD DAT1 (J1:7)
			SD1DAT2 P810 (J3:13) 	---> 	Pmod SD DAT2 (J1:8)
			SD1DAT3 P811 (J3:15) 	---> 	Pmod SD CS   (J1:1)

Note:
1) Connect the Pmod SD to the RA board: Using short wiring connections (should be shorter than 10cm).

2) Regarding the power supply pin: By default, the user can use Pmod SD GND (J1:5) and Pmod SD VCC (J1:6) as shown
   in the hardware connection, or alternatively, Pmod SD GND (J1:11) and Pmod SD VCC (J1:12).

3) Format the SD/MicroSD Card before performing any operation.

4) Data written can also be manually verified in file ra_sdhi.txt.

5) The user is expected to execute option 3 (Safely Remove SD/MicroSD Card) before removing SD/MicroSD Card else
   further file operations may fail and SD/MicroSD Card data may get corrupted.

6) The user must remove the SD/MicroSD Card after executing option 3 (Safely Remove SD/MicroSD Card), and re-insert
   it after executing option 5 (Initialize FreeRTOS+FAT).

7) The user is expected to enter J-Link RTT Viewer input data not exceeding 20 bytes in size.

8) Operation is not guaranteed for any user input value other than integer, char (e.g., float, special char) through
   J-Link RTT Viewer input.

9) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000490
   b. Keil:	Not Available
   c. IAR:	Not Available

10) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
    _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
