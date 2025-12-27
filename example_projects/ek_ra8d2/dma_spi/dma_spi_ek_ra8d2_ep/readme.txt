/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the SPI HAL module APIs using SPI and DMAC modules.
	The project configures DMA support SPI channels (Channel 0 and Channel 1) in Master and Slave mode. Once the
	modules are initialized and the SPI channels are configured, Master and Slave can transmit and receive data
	based on commands from user sent through J-Link RTT Viewer. SPI data transmit and receive are done via
	DMAC module.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M1, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5,
	                     FPB-RA6E1, RSSK-RA6T1, EK-RA4C1, EK-RA8D2.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	3 x Jumper wires.

3. Hardware Connections:
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

Note: 
1) The user is expected to enter data that is less than 64 bytes in size.

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
