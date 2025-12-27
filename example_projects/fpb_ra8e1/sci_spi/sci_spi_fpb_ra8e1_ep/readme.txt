/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     	This Example Project demonstrates the basic usage of SCI_SPI driver.
     	The project writes commands to the temperature module MAX31723PMB1 and reads the temperature value using SCI_SPI 
     	bus which is configured as master.
     	The temperature values are continuously printed on J-Link RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Temperature sensor MAX31723PMB1.
	Some jumper wires.

3. Hardware Connections:
        For EK-RA2A1:
 	-----------     
        	Connect Temperature sensor(J1:1) to PMODB (J6:1)
 
        For EK-RA4M1/ EK-RA6M1/ EK-RA6M2:
	-----------
        	Connect Temperature sensor(J1:1) to PMODA (J5:1)
        
        For EK-RA6M3/ EK-RA6M3G/ EK-RA2L1/ EK-RA4E2/ EK-RA6E2:
	-----------
        	Connect Temperature sensor(J1:1) to PMOD1 (J26:1)	

        For EK-RA4M3/ EK-RA4M2/ EK-RA6M4/ EK-RA6M5/ EK-RA2E1/ EK-RA2E2/ EK-RA8M1/ EK-RA8D1:
	-----------
        	Connect Temperature sensor(J1:1) to PMOD2 (J25:1)

        For FPB-RA2E3:
	-----------
        	Connect Temperature sensor(J1:1) to PMOD1 (Pin 1)

        For RSSK-RA6T1/ FPB-RA6E1/ FPB-RA4E1:
	-----------
        	Temperature sensor(J1:4) to P102
        	Temperature sensor(J1:2) to P101
        	Temperature sensor(J1:3) to P100
        	Temperature sensor(J1:5) to GND
        	Temperature sensor(J1:6) to Vcc
        	Temperature sensor(J1:1) to P103

        For EK-RA4W1:
	-----------
        	Connect Temperature sensor(J1:1) to PMOD (CN6:1)
		
        For MCK-RA6T2:
	-----------
        	Temperature sensor(J1:1) to PA12
       		Temperature sensor(J1:2) to PA09
        	Temperature sensor(J1:3) to PA10
        	Temperature sensor(J1:4) to PA08
        	Temperature sensor(J1:5) to GND
        	Temperature sensor(J1:6) to Vcc

        For MCK-RA4T1/ MCK-RA6T3:
	-----------
        	Temperature sensor(J1:1) to P301 (CN3 pin 8)
		Temperature sensor(J1:2) to P109 (CN5 pin 2)
		Temperature sensor(J1:3) to P110 (CN5 pin 3)
		Temperature sensor(J1:4) to P111 (CN3 pin 3)
		Temperature sensor(J1:5) to GND  (CN3 pin 32)
		Temperature sensor(J1:6) to Vcc  (CN2 pin 24)
	
        For MCK-RA8T1:
	-----------
        	Temperature sensor(J1:1) to P604 (CN4:P16)
        	Temperature sensor(J1:2) to P603 (CN3:P33)
        	Temperature sensor(J1:3) to P602 (CN3:P31)
        	Temperature sensor(J1:4) to P601 (CN4:P3)
        	Temperature sensor(J1:5) to GND  (CN3:P34)
        	Temperature sensor(J1:6) to Vcc  (CN3:P24)
		
        For EK-RA2A2:
	-----------
        	Temperature sensor(J1:1) to P503 (J1:3)
        	Temperature sensor(J1:2) to P501 (J1:1)
        	Temperature sensor(J1:3) to P500 (J4:19)
        	Temperature sensor(J1:4) to P502 (J1:2)
        	Temperature sensor(J1:5) to GND
        	Temperature sensor(J1:6) to Vcc  

        For FPB-RA8E1:
	-----------
        	Connect Temperature sensor(J1:1) to PMOD2 (PMOD2:1)
﻿
Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2206046c
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

3) For GCC ARM Embedded Toolchain: To enable printing floats to the RTT Viewer, edit the project settings and ensure that
   "Use float wih nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf
