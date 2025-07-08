/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     This Example Project demonstrates the basic usage of SCI_SPI driver.
     The project writes commands to the slave sensor and reads the temperature value using SCI_SPI bus configured as master .
     The temperature values are continuously printed on JLink RTT Viewer.

2. Hardware settings for the project:


        Hardware connections:

        EK_RA2A1:     
        Connect Temperature sensor(J1:1) to PMODB(J6:1)
 
        EK_RA4M1/ EK_RA6M1/ EK_RA6M2 :
        Connect Temperature sensor(J1:1) to PMODA(J5:1)
        
        EK_RA6M3/ EK_RA6M3G / EK_RA2L1 / EK_RA4E2 / EK_RA6E2:
        Connect Temperature sensor(J1:1) to PMOD1(J26:1)	

        EK_RA4M3/ EK_RA4M2/ EK_RA6M4/ EK_RA6M5/ EK_RA2E1/ EK_RA2E2:
        Connect Temperature sensor(J1:1) to PMOD2(J25:1)

	EK_RA6T1/FPB-RA6E1/FPB-RA4E1:
	Temperature sensor(J1:4) to P102
	Temperature sensor(J1:2) to P101
	Temperature sensor(J1:3) to P100
	Temperature sensor(J1:5) to GND
	Temperature sensor(J1:6) to Vcc
	Temperature sensor(J1:1) to P103

        EK_RA4W1:
        Connect Temperature sensor(J1:1) to PMOD(CN6:1)

        MCK_RA6T2:
        Temperature sensor(J1:1) to PA12
	Temperature sensor(J1:2) to PA09
	Temperature sensor(J1:3) to PA08
	Temperature sensor(J1:4) to PA10
	Temperature sensor(J1:5) to GND
	Temperature sensor(J1:6) to Vcc

        Temperature sensor used: MAX31723


1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000e44
   b. Keil:	0x20000c3c
   c. IAR: 0x20000918
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
