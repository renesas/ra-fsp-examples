/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     This Example Project demonstrates the basic usage of SCI_SPI driver.
     The project writes commands to the temperature module MAX31723PMB1 and reads the temperature value using SCI_SPI bus which is configured as master, 
     The temperature values are continuously printed on JLink RTT Viewer.

2. Hardware settings for the project:

        Hardware connections:

        EK-RA2A1:     
        Connect Temperature sensor(J1:1) to PMODB(J6:1)
 
        EK-RA4M1/ EK-RA6M1/ EK-RA6M2 :
        Connect Temperature sensor(J1:1) to PMODA(J5:1)
        
        EK-RA6M3/ EK-RA6M3G/ EK-RA2L1/ EK-RA4E2/ EK-RA6E2:
        Connect Temperature sensor(J1:1) to PMOD1(J26:1)	

        EK-RA4M3/ EK-RA4M2/ EK-RA6M4/ EK-RA6M5/ EK-RA2E1/ EK-RA2E2/ EK-RA8M1/ EK-RA8D1:
        Connect Temperature sensor(J1:1) to PMOD2(J25:1)

	FPB-RA2E3
        Connect Temperature sensor(J1:1) to PMOD1(Pin 1)

	RSSK-RA6T1/FPB-RA6E1/FPB-RA4E1:
	Temperature sensor(J1:4) to P102
	Temperature sensor(J1:2) to P101
	Temperature sensor(J1:3) to P100
	Temperature sensor(J1:5) to GND
	Temperature sensor(J1:6) to Vcc
	Temperature sensor(J1:1) to P103

        EK-RA4W1:
        Connect Temperature sensor(J1:1) to PMOD(CN6:1)
		
        MCK-RA6T2:
        Temperature sensor(J1:1) to PA12
	Temperature sensor(J1:2) to PA09
	Temperature sensor(J1:3) to PA10
	Temperature sensor(J1:4) to PA08
	Temperature sensor(J1:5) to GND
	Temperature sensor(J1:6) to Vcc

        Temperature sensor used: MAX31723
﻿
Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220007c8
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

3) To enable printing floats to RTT Viewer, edit the project settings and make sure use nano with printf is enabled. 
   The setting can be found by Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Misc > Use float with nano printf
