/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of OSPI_B driver on Renesas RA MCUs based on
	Renesas FSP.
	In the Main menu, based on RTT Viewer input, the user selects mode for opsi_b module.
		1. SPI 1S-1S-1S protocol mode.
		2. OPI 8D-8D-8D protocol mode.
	In the Sub Menu, based on RTT Viewer input, the user selects operations to perform.
		1. Write data to OSPI flash device.
		2. Read back the data from OSPI flash device.
		3. Erase a sector of OSPI flash device.
		4. Go back to main menu.
	On successful completion of each operation, a success message and execution time of operation will be displayed
	on J-link RTT Viewer.
	Error messages and information will be printed on J-link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA8M1, EK-RA8D1, EK-RA8E2, EK-RA8P1, EK-RA8D2
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.
	
4. Hardware Configurations:
	- For EK-RA8P1: Set the configuration switches (SW4) as below to avoid potential failures.
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	| SW4-1 PMOD1_SEL0 | SW4-2 PMOD1_SEL1 |    SW4-3 OSPI   |   SW4-4 ARDUINO  |    SW4-5 I3C   |    SW4-6 MIPI  | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	|       OFF        |       OFF        |       OFF       |       OFF        |      OFF       |      OFF       |       OFF        |       OFF        |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+

	- For EK-RA8D2: Set the configuration switches (SW4) as below to avoid potential failures.
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
	| SW4-1 PMOD1 | SW4-2 PMOD1 | SW4-3 Octo-SPI | SW4-4 Arduino/mikroBUS | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS | SW4-8 USBHS |
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
	|     OFF     |     OFF     |      OFF       |          OFF           |    OFF    |    OFF     |     OFF     |     OFF     |
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220041ac
   b. Keil:     Not Available
   c. IAR:      Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
