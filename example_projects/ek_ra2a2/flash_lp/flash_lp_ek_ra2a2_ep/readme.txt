/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the FLASH_LP HAL module APIs. The example project demonstrate 
	the code flash and data flash operations once user provides the RTT input from the available commands list. After 
	selecting the flash, user has to give input for the block number on which the operations are to be performed.
	
Note: 
1) If block number less than 10 is given for code flash operations, the board may get locked. 
   This happens due to R_FLASH_LP_AccessWindowSet API.
   User is expected to enter data of size not exceeding 15 bytes.
   Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through RTT.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000fb4
   b. Keil:	0x200007a4
   c. IAR: 0x20000684
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
