/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the ADC HAL module APIs.
        The project initializes the ADC in single scan or continuous scan mode based on user selection in RA configurator.
        Once initialized,user can initiate the ADC scan and also stop the scan (in the case of continuous scan mode)
        using J-Link RTT Viewer by sending commands. Result and ADC status is displayed on the J-Link RTT Viewer.

	Note: FPB-RA0E1 operates with One-shot or Sequential mode.

2. Hardware Connections:
        EK-RA2A1:
        	1) J2 Connector Pin no:36(AVSS0)--------> Pin no: 34(VREFLO) with Jumper or wire.

        No Hardware connections required for other RA boards.

3. Configuration Settings:
	By default, the example project runs in continuous scan mode on the boards, with the exception as mentioned in the *Note. 
	In order to switch the scan mode, change the 'Mode' field of ADC Driver in the RA configurator to Single Scan.
	
	For FPB-RA0E1: By default, the example project runs in Sequential mode on the board.
		       In order to switch the scan mode, change the 'Conversion operation' field of ADC_D Driver in the 
		       RA configurator to One-shot.
	
Note: 
1) EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4M1 and EK-RA2A2 support only Single Scan mode.
	
2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2206084c
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
