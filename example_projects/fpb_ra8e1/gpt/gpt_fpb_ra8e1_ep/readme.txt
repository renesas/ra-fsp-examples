/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates typical use of GPT HAL module APIs.
        User has the provision to input value as per displayed menu through J-Link RTT Viewer to select different GPT 
	supported modes (Periodic, PWM, One-Shot).
        In periodic mode, user can enter the time period within the permitted ranges to change the frequency of the user LED.
        In PWM mode, user can enter the duty cycle within the specified range to adjust the intensity of the user LED.
        In One-Shot mode, output will be displayed on J-Link RTT Viewer.
        Any failure will also be displayed on J-Link RTT Viewer.

2. Hardware Connections:
    	For EK-RA2L1:
    		Connect P414(J1 Pin 04) to P505(J3 Pin 06)

    	For EK-RA2E1:
    		Connect P212(J2 Pin 04) to P913(J2 Pin 12)

    	For EK-RA6M5:
    		Connect P414(J1 Pin 32) to P008(J4 Pin 26)

    	For FPB-RA4E1:
    		Connect P104(J4 Pin 21) to P407(J3 Pin 25)

    	For EK-RA2A2:
    		Connect P105(J4 Pin 16) to P307(J3 Pin 16)

Note: 
1) Timer period range in millisecond as per EP configuration: 
   a. EK-RA6M1/EK-RA6M2/EK-RA6M3/EK-RA6M3G/RSSK-RA6T1/MCK-RA6T2/EK-RA8M1/EK-RA8D1/MCK-RA8T1/FPB-RA8E1  	--      (0 to 35791) ms 
   b. EK-RA4M1/EK-RA2L1/EK-RA2E1/FPB-RA2E3                  						--      (0 to 89478) ms
   c. EK-RA2A1/EK-RA2E2/EK-RA2A2           	         						--      (0 to 1398)  ms
   d. EK-RA4W1                               								--      (0 to 2097)  ms
   e. EK-RA6M5/EK-RA6M4/EK-RA4M3/EK-RA4M2/FPB-RA4E1	   						--      (0 to 42949) ms
   f. FPB-RA6E1/MCK-RA4T1/MCK-RA6T3                            						--      (0 to 671)   ms        

2) User is expected to enter data of size not exceeding above 15 bytes.
   Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through RTT.﻿

3) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060040
   b. Keil:	0x22060008
   c. IAR: 0x22060234
	 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
