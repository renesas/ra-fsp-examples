/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the ADC HAL module APIs.
        The project initializes the ADC in single scan or continuous scan mode based on user selection in RA configurator.
        Once initialized,user can initiate the ADC scan and also stop the scan (in the case of continuous scan mode) using 
	JLinkRTTViewer by sending commands. User provide ADC channel input voltage from 0V to 3.3V with a voltage supply unit 
	at the ADC channel voltage input pin. Once ADC scan is initialized, Window Compare Mode is enabled and compares the	
	ADC channel input voltage with the upper and lower limits. The upper limit and lower limit are configured in RA 
	Configurator. If the ADC channel input voltage is above the upper limit or below the lower limit, it triggers an event 
	and notifies the user to act accordingly. Result and ADC status is displayed on the JLinkRTTViewer.

	Note: FPB-RA0E1 does not support Window Compare Mode and operates with One-shot or Sequential mode.

2. Hardware settings for the project
        Supported Boards: EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, MCK-RA4T1, MCK-RA6T2,
			  MCK-RA6T3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, RSSK-RA6T1, FPB-RA4E1, FPB-RA6E1
			  EK-RA8M1, EK-RA8D1, FPB-RA2E3, MCK-RA8T1, EK-RA2A2, FPB-RA0E1
		
	Hardware requirements:
		- RA board: 1x
		- Micro USB Cable: 1x
		- External Variable DC Power supply: 1x
		- Jumper wires to connect External Variable DC Power supply with RA board.

        Hardware connections:	
		ADC Channel Voltage Input Pin:
		
		Board	 Channel(Pin)
		------------------------------
		EK-RA2A1:
			1)AN01(P501)
			2)J2 Connector Pin no:36(AVSS0)--------> Pin no: 34(VREFL0) with Jumper or wire
		EK-RA2E1:
			AN000(P000)
		EK-RA2E2:
			AN009(P014)
		EK-RA2L1:
			AN000(P000)
		EK-RA4E2:
			AN00(P000)
		EK-RA4M1:
			AN00(P000)
		EK-RA4M2:
			AN000(P000)
		EK-RA4M3:
			AN000(P000)
		EK-RA4W1:
			AN04(P004)
		EK-RA6E2:
			AN00(P000)
		EK-RA6M1:
			AN00(P000)
		EK-RA6M2:
			AN00(P000)
		EK-RA6M3:
			AN00(P000)
		EK-RA6M3G:
			AN00(P000)
		EK-RA6M4:
			AN000(P000)
		EK-RA6M5:
			AN000(P000)
		FPB-RA6E1:
			AN000(P000)
		FPB-RA4E1:
			AN000(P000)
		MCK-RA4T1:
			AN00(P000)
		MCK-RA6T3:
			AN00(P000)
		RSSK-RA6T1:
			AN002(P002)
		EK-RA8M1, EK-RA8D1:
			AN00(P004)
		FPB-RA2E3:
			AN000(P000)
		MCK-RA8T1:
			AN002(P006) (CN1 Pin 9)
		EK-RA2A2:
			AN000(P014)
		FPB-RA0E1:
			AN000(P010)

3. Configuration Settings:
	By default, the example project runs in continuous scan mode on the boards.
	In order to switch the scan mode, change the 'Mode' field of ADC Driver in the RA configurator to Single Scan.
	
	Note: For FPB-RA0E1, by default the example project runs in Sequential mode on the board. 
	      In order to switch the scan mode, change the 'Conversion operation' field of ADC_D Driver in the 
  	      RA configurator to One-shot.

Note: MCK-RA6T2 does not support Window Compare Mode and operates with internal temperature sensor.

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004604
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

3) To enable printing floats to RTT Viewer, edit the project settings and make sure use nano with printf is enabled. 
   The setting can be found by Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Misc > Use float with nano printf
 
