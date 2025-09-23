/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the functionality of AGT in Periodic mode and One-Shot mode. On providing
	any input on the RTT Viewer, AGT channel 0 starts in One-Shot mode. AGT channel 1 starts in Periodic mode when
	AGT channel 0 expires. Timer in Periodic mode expires periodically at a time period specified by the user and
	toggles the on-board LED.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	For FPB-RA2T1:
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

Note:
1) The user is expected to enter data not exceeding 15 bytes in size.

2) Operation is not guaranteed for any user input value other than integer, char (e.g., float, special char)
   through RTT Viewer.

3) The user should select "Input -> Sending -> Send on Enter" for RTT Viewer configuration.

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x22000430
   b. Keil:	0x22000850
   c. IAR:	0x220008f4

5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
