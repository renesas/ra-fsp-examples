/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the SPI HAL module APIs using SPI and DMAC modules.
	The project configures DMA support SPI channels (Channel 0 and Channel 1) in Master and Slave modes. Once the
	modules are initialized and the SPI channels are configured, Master and Slave can transmit and receive data
	based on commands from the user sent through J-Link RTT Viewer. SPI data is transmitted and received via the
	DMAC module.

2. Hardware Requirements:
	Supported RA boards: EK-RA4C1, EK-RA4M1, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4,
	                     EK-RA6M5, EK-RA8D2, EK-RA8E2, EK-RA8M2, EK-RA8P1, EK-RA8T2, FPB-RA6E1, FPB-RA8E1,
	                     RSSK-RA6T1, EK-RA8P1 V2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	3 x Jumper wires.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA4M1:
		MISO  ----> P410 - P202
		MOSI  ----> P411 - P203
		RSPCK ----> P412 - P204

	For EK-RA4W1:
		MISO  ----> P100 - P110
		MOSI  ----> P101 - P109
		RSPCK ----> P102 - P111

	For EK-RA6M1:
		MISO  ----> P100 - P110
		MOSI  ----> P101 - P109
		RSPCK ----> P102 - P111

	For EK-RA6M2:
		MISO  ----> P410 - P202
		MOSI  ----> P411 - P203
		RSPCK ----> P412 - P204

	For EK-RA6M3:
		MISO  ----> P410 - P202
		MOSI  ----> P411 - P203
		RSPCK ----> P412 - P204

	For EK-RA6M3G:
		MISO  ----> P410 - P202
		MOSI  ----> P411 - P203
		RSPCK ----> P412 - P204

	For EK-RA6M4:
		MISO  ----> P202 - P410 
		MOSI  ----> P203 - P411
		RSPCK ----> P204 - P412

	For EK-RA6M5:
		MISO  ----> P410 - P202
		MOSI  ----> P411 - P203
		RSPCK ----> P412 - P204

	For EK-RA6T1:
		MISO  ----> P100 - P110 
		MOSI  ----> P101 - P109
		RSPCK ----> P102 - P111

	For FPB-RA6E1:
		MISO  ----> P410 - P110
		MOSI  ----> P411 - P109
		RSPCK ----> P412 - P111

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		The user must close E59, E61, E47, and E48 to use P209, P210, P100, and P101.
		MISO  ----> P210 (J2:45) - P100 (J2:27)
		MOSI  ----> P211 (J2:43) - P101 (J2:29)
		RSPCK ----> P209 (J2:39) - P102 (J4:7)

	For EK-RA8D2:
		The user must turn OFF SW4-6 to use P700, P701, P702 for SPI channel 0.
		The user must turn ON SW4-3 and turn OFF SW4-4 to use P100, P101, P102 for SPI channel 1.
		MISO  ----> P700 (J3:9)  - P100 (J2:2)
		MOSI  ----> P701 (J3:12) - P101 (J2:12)
		RSPCK ----> P702 (J3:13) - P102 (J2:13)

	For EK-RA8E2:
		MISO  ----> P700 (J2:10) - P410 (J2:26)
		MOSI  ----> P701 (J2:9)  - P411 (J2:25)
		RSPCK ----> P702 (J2:12) - P412 (J2:23)

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.
		MISO  ----> P609 (J1:13) - P709 (J24:5)
		MOSI  ----> P611 (J1:6)  - P708 (J24:4)
		RSPCK ----> P610 (J1:5)  - P415 (J24:6)

	For EK-RA8P1:
		The user must turn OFF SW4-6 to use P700, P701, P702 for SPI channel 0.
		The user must turn ON SW4-3 and turn OFF SW4-4 to use P100, P101, P102 for SPI channel 1.
		MISO  ----> P700 (J3:9)  - P100 (J2:2)
		MOSI  ----> P701 (J3:12) - P101 (J2:12)
		RSPCK ----> P702 (J3:13) - P102 (J2:13)

	For FPB-RA8E1:
		MISO  ----> P609 (J3:13) - P410 (J2:12)
		MOSI  ----> P611 (J3:12) - P411 (J2:9)
		RSPCK ----> P610 (J3:14) - P412 (J2:10)

	For EK-RA8T2:
		The user must set the configuration switches (SW6 and SW4) as below to use the on-board
		debug functionality.
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+

		+-------+-------+-------+-------+-------+-------+-------+-------+
		| SW4-1 | SW4-2 | SW4-3 | SW4-4 | SW4-5 | SW4-6 | SW4-7 | SW4-8 |
		+-------+-------+-------+-------+-------+-------+-------+-------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |
		+-------+-------+-------+-------+-------+-------+-------+-------+

		MISO  ----> P700 (J17:3) - P100 (J7:5)
		MOSI  ----> P701 (J17:1) - P101 (J7:4)
		RSPCK ----> P702 (J17:8) - P102 (J2:17)

	For EK-RA8P1 V2:
		The user must set the configuration switches (SW6) as below to use the on-board
		debug functionality.
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		The user must turn OFF SW4-6 to use P700, P701, P702 for SPI channel 0.
		The user must turn ON SW4-3 and turn OFF SW4-4 to use P100, P101, P102 for SPI channel 1.
		MISO  ----> P700 (J17:12) - P100 (J3:35)
		MOSI  ----> P701 (J2:19)  - P101 (J3:34)
		RSPCK ----> P702 (J4:16)  - P102 (J3:31)

Note:
1) The user is expected to enter data that is less than 64 bytes in size, including the end‑of‑line character.

2) As SPI bit width is set to 32 bits, each 4 bytes of user data input will be transferred in single SPI transmission.

3) The bytes sent for Slave should be lesser than the bytes sent for Master when using WriteRead operation.

4) Operation is not guaranteed for any user input value other than integer, char (e.g., float, special char)
   through RTT Viewer.

5) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200048c
   b. Keil:     Not Available
   c. IAR:      Not Available

6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
