/***********************************************************************************************************************
* 
* Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
*
* This software is supplied by Renesas Electronics America Inc. and may only be used with products of Renesas Electronics Corp.
* and its affiliates (“Renesas”).  No other uses are authorized.  This software is protected under all applicable laws, 
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
* 
**********************************************************************************************************************/

1. Project Overview:
     This Example Project demonstrates the basic usage of SCI_SPI driver.
     The project writes commands to the temperature module MAX31723PMB1 and reads the temperature value using SCI_SPI bus which is configured as master, 
     The temperature values are continuously printed on JLink RTT Viewer.

2. Hardware settings for the project:


        Hardware connections:

        EK_RA2A1:     
        Connect Temperature sensor(J1:1) to PMODB(J6:1)
 
        EK_RA4M1/ EK_RA6M1/ EK_RA6M2 :
        Connect Temperature sensor(J1:1) to PMODA(J5:1)
        
        EK_RA6M3/ EK_RA6M3G/EK_RA2L1:
        Connect Temperature sensor(J1:1) to PMOD1(J26:1)	

        EK_RA4M3, EK_RA4M2,EK_RA6M4,EK_RA6M5,EK_RA2E1 and EK_RA2E2:
        Connect Temperature sensor(J1:1) to PMOD2(J25:1)

	RSSK_RA6T1/FPB-RA6E1/FPB-RA4E1:
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
	Temperature sensor(J1:3) to PA10
	Temperature sensor(J1:4) to PA08
	Temperature sensor(J1:5) to GND
	Temperature sensor(J1:6) to Vcc

        Temperature sensor used: MAX31723
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000b9c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
