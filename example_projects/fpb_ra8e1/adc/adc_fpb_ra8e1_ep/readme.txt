/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the ADC HAL module APIs.
        The project initializes the ADC in single scan or continuous scan mode based on user selection in RA configurator.
        Once initialized,user can initiate the ADC scan and also stop the scan (in the case of continuous scan mode) using 
	J-Link RTT Viewer by sending commands. User provide ADC channel input voltage from 0V to 3.3V with a voltage supply unit 
	at the ADC channel voltage input pin. Once ADC scan is initialized, Window Compare Mode is enabled and compares the	
	ADC channel input voltage with the upper and lower limits. The upper limit and lower limit are configured in RA 
	Configurator. If the ADC channel input voltage is above the upper limit or below the lower limit, it triggers an event 
	and notifies the user to act accordingly. Result and ADC status is displayed on the J-Link RTT Viewer.

	Note: FPB-RA0E1 does not support Window Compare Mode and operates with One-shot or Sequential mode.

2. Hardware Requirements:
        Supported RA Boards: EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, MCK-RA4T1, 
			     MCK-RA6T2, MCK-RA6T3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, 
			     RSSK-RA6T1, FPB-RA4E1, FPB-RA6E1, EK-RA8M1, EK-RA8D1, FPB-RA2E3, MCK-RA8T1, EK-RA2A2, FPB-RA0E1, 
			     FPB-RA8E1

	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x External Variable DC Power supply.
	Some jumper wires to connect External Variable DC Power supply with RA board.

3. Hardware Connections:	
	ADC Channel Voltage Input Pin:
	For EK-RA2A1:
		AN01(P501)
		J2 Connector Pin no:36(AVSS0)--------> Pin no: 34(VREFL0) with Jumper or wire

	For EK-RA2E1, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, 
	    EK-RA6M5, FPB-RA6E1, FPB-RA4E1, MCK-RA4T1, MCK-RA6T3, FPB-RA2E3:
		AN000(P000)

	For EK-RA2E2:
		AN009(P014)

	For EK-RA4W1:
		AN04(P004)

	For RSSK-RA6T1:
		AN002(P002)

	For EK-RA8M1, EK-RA8D1:
		AN00(P004)

	For MCK-RA8T1:
		AN002(P006) (CN1 Pin 9)

	For EK-RA2A2:
		AN000(P014)

	For FPB-RA0E1:
		AN000(P010)

	For FPB-RA8E1:
		AN00(P004) (J1 pin 20)

4. Configuration Settings:
	By default, the example project runs in continuous scan mode on the boards.
	In order to switch the scan mode, change the 'Mode' field of ADC Driver in the RA configurator to Single Scan.
	
	For FPB-RA0E1: By default the example project runs in Sequential mode on the board. 
		       In order to switch the scan mode, change the 'Conversion operation' field of ADC_D Driver in the 
		       RA configurator to One-shot.

Note: 
1) MCK-RA6T2 does not support Window Compare Mode and operates with internal temperature sensor.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060040
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

4) For GCC ARM Embedded Toolchain: To enable printing floats to RTT Viewer, edit the project settings and ensure that
   "Use float with nano printf" is enabled. The setting can be found by 
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Misc > Use float with nano printf
 
