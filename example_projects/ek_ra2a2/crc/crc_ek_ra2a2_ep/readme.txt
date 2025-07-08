/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the CRC HAL module APIs.
        The project demonstrates CRC operation for data transmission in normal mode and reception in snoop mode through sci interface.
	If board not support snoop mode (EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3), reception in normal mode through sci interface.
        On Pressing any key through RTT Viewer CRC value in normal mode is calculated for 4 bytes of data.
        The calculated CRC value along with input data is transmitted and received on sci_uart through loop-back.
        Once the transfer is complete, and if CRC value for snoop mode is zero and transmit and receive buffer are equal then On-board
        LED blinks as sign of successful CRC operation. On data mismatch, LED stays ON. Failure and status messages are displayed on
        RTT Viewer.

Note:
a. For any event or API failure appropriate messages is displayed on RTT viewer.
b. User can change the polynomial to CRC_16 and CRC_CCITT and bit order from MSB to LSB from CRC configurator and observe the results.
c. The application does not work for CRC_32 bit polynomial.
d. Please refer the latest FSP User Manual for comparison with popular online CRC calculator and the associated limitations are captured in it.

2. Hardware Settings:
        Single jumper wires is required to establish loop back connection for SCI UART within the board with TXD and RXD pins 
        shorted as mentioned below:
        
        EK-RA6M2
        -------
        Channel 3 has been used by SCI_UART Loopback operation.
        SCI3 P408  ----> RXD      
        SCI3 P409  ----> TXD

        EK-RA2A1
        -------
        Channel 1 has been used by SCI_UART Loopback operation.
        SCI1  P411 ----> RXD 
        SCI1  P410 ----> TXD 

        EK-RA4M1
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0  P410 ----> RXD 
        SCI0  P411 ----> TXD 

        EK-RA6M1, EK-RA4M3, EK-RA4M2, EK-RA6M5, FPB-RA2E3
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0 P100 ----> RXD 
        SCI0 P101 ----> TXD 

        EK-RA6M3 and EK-RA6M3G
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0 P410 ----> RXD 
        SCI0 P411 ----> TXD 

        EK-RA4W1
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0 P104 ----> RXD 
        SCI0 P101 ----> TXD 

        EK-RA6M4 and EK-RA2L1
        -------
        Channel 1 has been used by SCI_UART Loopback operation.
        SCI1 P212 ----> RXD 
        SCI1 P213 ----> TXD

        RSSK-RA6T1
        -------
        Channel 4 has been used by SCI_UART Loopback operation.
        SCI4 P206 ----> RXD 
        SCI4 P205 ----> TXD

        EK-RA2E1
        -------
        Channel 1 has been used by SCI_UART Loopback operation.
        SCI1 P502 ----> RXD 
        SCI1 P501 ----> TXD

        FPB-RA4E1, EK-RA8M1 and EK-RA8D1
        Channel 3 has been used by SCI_UART Loopback operation.
        SCI3 P408 ----> RXD 	
        SCI3 P409 ----> TXD

        FPB-RA6E1
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0 P100 ----> RXD 
        SCI0 P101 ----> TXD

        EK-RA2E2
        -------
        Channel 9 has been used by SCI_UART Loopback operation.
        SCI9 P401 ----> RXD 
        SCI9 P400 ----> TXD

	MCK-RA6T2
        -------
        Channel 1 has been used by SCI_UART Loopback operation.
        SCI1 P408 ----> RXD 
        SCI1 P409 ----> TXD

	EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0  P410 ----> RXD 
        SCI0  P411 ----> TXD

	MCK-RA8T1
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI1 P602 (CN3 Pin 31) ----> RXD
        SCI1 P603 (CN3 Pin 33) ----> TXD

	EK-RA2A2
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0 P101 ----> RXD 
        SCI0 P100 ----> TXD

Note:		﻿
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000c4c
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
