/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the FLASH_LP HAL module APIs. The example project demonstrate 
	the code flash and data flash operations once the user provides the RTT input from the available commands list. 
	After selecting the flash, the user has to give input for the block number on which the operations are to be performed.

Note: 
1) If block number less than 10 is given for code flash operations, the board may get locked. 
   This happens due to R_FLASH_LP_AccessWindowSet API.
   The user is expected to enter data of size not exceeding 15 bytes.
   Operation is not guaranteed for any user input value other than integer (i.e. float, char, special char) through RTT Viewer.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004f54
   b. Keil:	0x20004538
   c. IAR: 	0x20004638
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
