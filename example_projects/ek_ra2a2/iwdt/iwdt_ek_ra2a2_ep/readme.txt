/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the typical use of the IWDT HAL module.
	User input initializes the IWDT and start GPT timer. IWDT counter is refreshed
	periodically every 1 second when the GPT timer expires. On entering 2 from RTTViewer,
	IWDT counter stops refreshing and resets the MCU.

Note:   ﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008a4
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
