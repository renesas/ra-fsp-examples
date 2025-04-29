/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the RTC HAL module APIs. 
	The project initializes the RTC module and allows the user to set RTC Calendar time and Calendar alarm.
	User can enable periodic interrupt and can also view the current RTC calendar time.
        On enabling periodic interrupt, on-board LED toggles every 1 second and on occurrence of Calendar alarm. LED is turned ON.
	
NOTE:
   1. User is expected to enter Time values to set Calendar time and Calendar alarm in 24-hours format.
        Sample Input: 23:10:2019 16:14:55 (DD:MM:YYYY HH:MM:SS)
   2. User is expected to enter data of size not exceeding above 15 bytes.
   3. Operation is not guaranteed for any user input value other than integer,char(i.e. float, special char) through RTT.﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200004b4
   b. Keil:	0x20000008
   c. IAR: 0x2000010c
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
