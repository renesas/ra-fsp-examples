/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the Flash_HP HAL module APIs. The example project demonstrate the
        code flash and data flash operations once, user provides RTT input from the available commands list. 
   	To exit from the demo, user should choose the EXIT option.

NOTE  : User is expected to enter data of size not exceeding 15 bytes.
	Operation is not guaranteed for any user input value, other than integer(i.e. float, char, special char) through RTT.
	Entering any input after EXIT option will result in error trap. Not recommended.

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x1ffe0ecc
   b. Keil:	0x1ffe0dd4
   c. IAR: 0x1ffe0e10
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
