/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
   	The example project demonstrates the typical use of the WDT HAL module APIs.
   	User can give input through J-Link RTT Viewer to start the WDT. WDT gets refreshed periodically through GPT timer.
   	User can press push button to stop the GPT timer which in turn stops refreshing WDT timer.
   	Approximately after 2 seconds, WDT resets the MCU and turn LED ON until user gives the input once again.

2. Hardware Connections:
	For EK-RA2A1/EK-RA2E1/EK-RA2L1/EK-RA4M1/EK-RA4M2/EK-RA4M3/EK-RA4W1/EK-RA6M1/EK-RA6M2/EK-RA6M4/EK-RA6M5/
	    EK-RA4E2/EK-RA6E2/FPB-RA4E1/FPB-RA6E1/MCK-RA4T1/MCK-RA6T3/FPB-RA8E1 board: 
		Use Switch S1 to stop WDT from refreshing.

	For EK-RA2E2/RSSK-RA6T1/EK-RA6M3/EK-RA6M3G/MCK-RA6T2/EK-RA2A2/EK-RA8M1/EK-RA8D1 board: 
		Use Switch S2 to stop WDT from refreshing.

	For FPB-RA2E3 board: 
		Connect wire to short P200/NMI (J3-21) to P206 (J3-18).
		Use Switch S1 to stop WDT from refreshing.

	For MCK-RA8T1 board:
		Connect Pmod BTN (J1:1) to PMOD(CN6:1).
		Use Switch BTN1 (push button on Pmod BTN) to stop WDT from refreshing.﻿

Note:
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060040
   b. Keil:	0x22060850
   c. IAR: 0x220608e8
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).