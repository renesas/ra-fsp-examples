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
     The example project demonstrates the typical use of the SPI HAL module APIs using SPI and DTC modules. The project configure 
     SPI channels (Channel 0 and Channel 1) in Master and Slave mode. Once the modules are initialized and the SPI 
     channels are configured, Master and Slave can transmit and receive data based on commands from user sent through JLinkRTTViewer.
     SPI data transmit and receive is done via DTC module.

2. Hardware settings for the project:

    Hardware connections:
        
    Pin Connection for EK-RA2A1
    MISO  ----> P109 - P104
    MOSI  ----> P400 - P105
    RSPCK ----> P110 - P103
    
    Pin Connection for EK-RA4M1
    MISO  ----> P410 - P202
    MOSI  ----> P411 - P203
    RSPCK ----> P412 - P204
	
    Pin Connection for EK-RA4W1
    MISO  ----> P100 - P110
    MOSI  ----> P101 - P109
    RSPCK ----> P102 - P111

    Pin Connection for EK-RA6M1
    MISO  ----> P100 - P110
    MOSI  ----> P101 - P109
    RSPCK ----> P102 - P111

    Pin Connection for EK-RA6M2
    MISO  ----> P410 - P202
    MOSI  ----> P411 - P203
    RSPCK ----> P412 - P204

    Pin Connection for EK-RA6M3
    MISO  ----> P410 - P202
    MOSI  ----> P411 - P203
    RSPCK ----> P412 - P204

    Pin Connection for EK-RA6M3G
    MISO  ----> P410 - P202
    MOSI  ----> P411 - P203
    RSPCK ----> P412 - P204

    Pin Connection for EK-RA6M4
    MISO  ----> P202 - P410 
    MOSI  ----> P203 - P411
    RSPCK ----> P204 - P412
	
    Pin Connection for EK-RA6T1
    MISO  ----> P100 - P110 
    MOSI  ----> P101 - P109
    RSPCK ----> P102 - P111

    Pin Connection for EK-RA2L1 / EK-RA6M5
    MISO  ----> P410 - P202
    MOSI  ----> P411 - P203
    RSPCK ----> P412 - P204

    Pin Connection for FPB-RA6E1
    MISO  ----> P410 - P110
    MOSI  ----> P411 - P109
    RSPCK ----> P412 - P111

    Pin Connection for MCK-RA6T2
    MISO  ----> PE13 - PB14
    MOSI  ----> PE14 - PB15
    RSPCK ----> PE12 - PB13

    Pin Connection for MCK-RA4T1 / MCK-RA6T3 /EK-RA4E2
    MISO  ----> P206 - P100
    MOSI  ----> P207 - P101
    RSPCK ----> P302 - P102

    Pin Connection for EK-RA6E2
    MISO  ----> P110 - P100
    MOSI  ----> P109 - P101
    RSPCK ----> P111 - P102

    Pin Connection for EK-RA8M1
    MISO  ----> P700 - P410
    MOSI  ----> P701 - P411
    RSPCK ----> P702 - P412
    Install jumper J61 to disable Ethernet B (Use P700, P701 for SPI channel 0)

    Pin Connection for EK-RA8D1
    MISO  ----> P700 - P410
    MOSI  ----> P701 - P411
    RSPCK ----> P702 - P412
    Turn OFF SW1-3 and SW1-5 to use P700, P701, P702 for SPI channel 0    

Note: 
    User is expected to enter data of size not exceeding 64 bytes.
    As SPI bit width is set to 32 bits, each 4 bytes of user data input will be transferred in single SPI transmission.
    The bytes sent for slave should be lesser than the bytes sent for master when using WriteRead operation.
    Operation is not guaranteed for any user input value other than integer,char(i.e. float, special char) through RTT.
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000aa8
   b. Keil:	0x22000584
   c. IAR: 0x220005a4
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
