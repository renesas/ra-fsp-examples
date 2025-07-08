/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the functionality of AGT in periodic mode and one-shot mode.  On providing any 
input on the RTTviewer, AGT channel 0 starts in one-shot mode. AGT channel 1 starts in periodic mode when AGT channel 0 expires.
Timer in periodic mode expires periodically at a time period specified by user and toggles the on-board LED.
	
NOTE:
1. User is expected to enter data of size not exceeding above 15 bytes.
   Operation is not guaranteed for any user input value other than integer,char(i.e. float, special char) through RTT.


	





 
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008f8
   b. Keil:	0x20000840
   c. IAR: 0x200008cc
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
