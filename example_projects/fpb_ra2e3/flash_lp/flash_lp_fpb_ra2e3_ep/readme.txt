/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
	The example project demonstrates the typical use of the FLASH_LP HAL module APIs. The example project demonstrate 
	the code flash and data flash operations once user provides the RTT input from the available commands list. After 
	selecting the flash, user has to give input for the block number on which the operations are to be performed.
	
NOTE  : If block number less than 10 is given for code flash operations, the board may get locked. 
	This happens due to R_FLASH_LP_AccessWindowSet API.
	User is expected to enter data of size not exceeding 15 bytes.
	Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through RTT.

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004f0c
   b. Keil:	0x20004684
   c. IAR: 	0x20004684
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
