/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the FLASH_LP HAL module APIs. The example project
	demonstrates the code flash and data flash operations once the user provides the RTT input from the available
	commands list. After selecting the flash, the user has to give input for the block number on which the
	operations are to be performed.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

Note:
1) If block number less than 10 is given for code flash operations, the board may get locked.
   This happens due to R_FLASH_LP_AccessWindowSet API.

2) The user is expected to enter data not exceeding 15 bytes in size.
   Operation is not guaranteed for any user input value other than integer (e.g., float, char, special char)
   through RTT Viewer.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x200051b8
   b. Keil:	Not Available
   c. IAR:	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
