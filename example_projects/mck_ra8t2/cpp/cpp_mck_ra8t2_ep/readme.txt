/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the ADC HAL module APIs. The project initializes the ADC in
	Single Scan or Continuous Scan mode based on user selection in RA Configurator. Once initialized, the user can
	initiate the ADC scan and also stop the scan (in the case of Continuous Scan mode) using J-Link RTT Viewer by
	sending commands. Result and ADC status are displayed on the J-Link RTT Viewer.

	Note: FPB-RA0E1, FPB-RA0E2 operates with One-shot or Sequential mode.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	For EK-RA2A1:
		1) Connect AVSS0 (J2:36) to VREFL0 (J2:34) using a jumper wire.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	No hardware connections required for other RA boards.

4. Configuration Settings:
	By default, the example project runs in Continuous Scan mode on the boards, with the exception as mentioned
	in the "Note 1". In order to switch the scan mode, change the "Mode" field of ADC Driver in the RA configurator
	to Single Scan.
	
	For FPB-RA0E1, FPB-RA0E2: 
		By default, the example project runs in Sequential mode on the board. In order to switch the scan mode, 
		change the "Conversion operation" field of ADC_D Driver in the RA Configurator to One-shot.

	For MCK-RA6T2, EK-RA8P1, MCK-RA8T2: 
		By default, the example project runs in Continuous Scan mode on the board. In order to switch the scan mode,
		change the "Scan mode" field of ADC_B Driver in the RA Configurator to Single Scan.

Note: 
1) EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4M1, EK-RA2A2, EK-RA4L1, EK-RA2L2, FPB-RA2T1 support only Single Scan mode.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil: 	Not Available
   c. IAR: 	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
