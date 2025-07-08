/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
   The example project demonstrates the typical use of the WDT HAL module APIs.
   User can give input through JLinkRTTViewer to start the WDT. WDT gets refreshed periodically through GPT timer.
   User can press push button to stop the GPT timer which in turn stops refreshing WDT timer.
   Approximately after 2 seconds, WDT resets the MCU and turn LED ON until user gives the input once again.
   
Note: 
1. On RA2L1-EK/RA2E1-EK/RA4M2-EK/RA4M3-EK/RA6M4-EK/RA4E1-FPB/RA6E1-FPB board, use Switch S1 to stop WDT from refreshing.
2. On RA2E2-EK/RA6T1-RSSK/RA6M3-EK/RA6T2-MCK board, use Switch S2 to stop WDT from refreshing.﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008a8
   b. Keil:	0x20000840
   c. IAR: 0x200008b8
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
