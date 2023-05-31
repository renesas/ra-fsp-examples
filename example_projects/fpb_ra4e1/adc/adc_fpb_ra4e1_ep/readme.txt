/***********************************************************************************************************************
* Copyright [2022] Renesas Electronics Corporation and/or its affiliates. All Rights Reserved.
*
* This software is supplied by Renesas Electronics America Inc. and may only be used with products of Renesas Electronics Corp.
* and its affiliates (“Renesas”). No other uses are authorized. This software is protected under all applicable laws, 
* including copyright laws.
* Renesas reserves the right to change or discontinue this software.
* THE SOFTWARE IS DELIVERED TO YOU “AS IS,” AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT 
* PERMISSIBLE UNDER APPLICABLE LAW,DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE.  TO THE MAXIMUM 
* EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE (OR ANY PERSON 
* OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING, WITHOUT LIMITATION, 
* ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES;
* ANY LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF 
* THE POSSIBILITY OF SUCH LOSS,DAMAGES, CLAIMS OR COSTS.
* **********************************************************************************************************************/

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
			  MCK-RA6T3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, RSSK-RA6T1, FPB-RA4E1 and FPB-RA6E1
      
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
			AN005(P014)

3. Configuration Settings:
        The example project, by default, runs in continuous scan mode on the boards.
	In order to switch the scan mode, change the 'Mode' field of ADC Driver in the RA configurator to Single Scan.

*Note: MCK-RA6T2 does not support Window Compare Mode and operates with internal temperature sensor.
﻿
4. Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000644
   b. Keil: Not Available 
   c. IAR: Not Available
 
5. If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
