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


2. Hardware settings for the project
        Supported Boards- EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, MCK-RA4T1, MCK-RA6T2,
			  MCK-RA6T3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, RSSK-RA6T1, FPB-RA4E1, FPB-RA6E1,
			  EK-RA8M1, EK-RA8D1, MCK-RA8T1
      
        Hardware connections:
		Connect the ground (GND) of the external voltage supply with the board ground, and connect the voltage supply to 
		the ADC Channel Voltage Input Pin as shown below:

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
			AN003(P008)
		EK-RA8M1, EK-RA8D1:
			AN00(P004)
		FPB-RA2E3:
			AN000(P000)
		MCK-RA8T1:
			AN002(P006) (CN1 Pin 9)

3. Configuration Settings:
        The example project, by default, runs in continuous scan mode on the boards.
	In order to switch the scan mode, change the 'Mode' field of ADC Driver in the RA configurator to Single Scan.

*Note: MCK-RA6T2 does not support Window Compare Mode and operates with internal temperature sensor.

4. Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000e20
   b. Keil: Not Available 
   c. IAR: Not Available
 
5. If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).