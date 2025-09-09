/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
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

    Pin Connection for MCK-RA4T1 
    MISO  ----> P206 (CN3-pin 24) - P100 (TP-17)
    MOSI  ----> P207 (CN3-pin 6)  - P101 (TP-16)
    RSPCK ----> P302 (CN3-pin 5)  - P102 (CN2-pin 31)

NOTE: 
    User is expected to enter data of size not exceeding 64 bytes.
    As SPI bit width is set to 32 bits, each 4 bytes of user data input will be transferred in single SPI transmission.
    The bytes sent for slave should be lesser than the bytes sent for master when using WriteRead operation.
    Operation is not guaranteed for any user input value other than integer,char(i.e. float, special char) through RTT.
﻿
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000d34
   b. Keil:	0x20000c3c
   c. IAR: 0x20000b18
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
