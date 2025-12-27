/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionalities of the MIPI DSI device driver on Renesas RA MCUs
	based on Renesas FSP. On successful initialization of the MIPI DSI module, the EP will display 8-color bars on
	the MIPI LCD (external LCD connected to RA MCU) using the graphics and MIPI-DSI modules in FSP. A user menu will
	be provided over the RTT Viewer. The user can choose the time to enter Ultra-Low-Power State (ULPS) and touch
	the screen or wait 30s to exit this mode. Status information and error messages will be printed on J-Link RTT
	Viewer during the execution of the project.

2. Hardware Requirements:
	Supported RA boards: EK-RA8D1, EK-RA8P1, EK-RA8D2
	1 x Renesas RA board.
	1 x MIPI Graphics Expansion Board.
	1 x Type-C USB cable for programming and debugging.
	
3. Hardware Connections:
	For EK-RA8D1:
		1. Connect the USB Debug port on RA board to the PC using a micro USB cable.
		2. Connect the MIPI Graphics Expansion Board to J58 of RA board.
		3. Set the configuration switches (SW1) on the RA board as below:
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |       OFF    |     OFF    |      OFF   |     OFF    |      ON     |    OFF    |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

	For EK-RA8P1, EK-RA8D2:
		1. Connect the USB Debug port on RA board to the PC using a Type-C USB cable.
		2. Connect the MIPI Graphics Expansion Board to J32 of RA board.
		3. Set the configuration switches (SW4) on the RA board as below:
		+------------------+------------------+------------+---------------+-----------+------------+------------------+------------------+
		| SW4-1 PMOD1_SEL0 | SW4-2 PMOD1_SEL1 | SW4-3 OSPI | SW4-4 ARDUINO | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
		+------------------+------------------+------------+---------------+-----------+------------+------------------+------------------+
		|       OFF        |       OFF        |     OFF    |      OFF      |    OFF    |     ON     |       OFF        |       OFF        |
		+------------------+------------------+------------+---------------+-----------+------------+------------------+------------------+

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200044c
   b. Keil: Not Available
   c. IAR: Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called 
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
