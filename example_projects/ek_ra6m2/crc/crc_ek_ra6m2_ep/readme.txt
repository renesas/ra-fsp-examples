/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the CRC HAL module APIs.
	The project demonstrates CRC operation for data transmission in normal mode and reception in snoop mode/normal mode 
	through sci interface.
	On Pressing any key through RTT Viewer CRC value in normal mode is calculated for 4 bytes of data.
	The calculated CRC value along with input data is transmitted and received on sci_uart through loop-back.
	Once the transfer is complete, and if CRC value for received data is zero and transmit and receive buffer are equal then On-board 
	LED blinks as sign of successful CRC operation. On data mismatch, LED stays ON. Failure and status messages are displayed on 
	RTT Viewer.

Note:
* For any event or API failure appropriate messages is displayed on RTT viewer.
* User can change the polynomial to CRC_16 and CRC_CCITT and bit order from MSB to LSB from CRC configurator and observe the results.
* The application does not work for CRC_32 bit polynomial.
* Please refer the latest FSP User Manual for comparison with popular online CRC calculator and the associated limitations are captured in it.
* For reception, following boards support reception in normal mode (snoop mode is not available): 
	RA4E2-EK, RA4M2-EK, RA4M3-EK, RA6E2-EK, RA6M4-EK, RA6M5-EK, RA4E1-FPB, RA6E1-FPB.
	
2. Hardware Settings:
        Single jumper wires is required to establish loop back connection for SCI UART within the board with TXD and RXD pins 
        shorted as mentioned below
        
        RA6M2_EK
        -------
        Channel 3 has been used by SCI_UART Loopback operation.
        SCI3 P408  ----> RXD      
        SCI3 P409  ----> TXD

        RA2A1-EK
        -------
        Channel 1 has been used by SCI_UART Loopback operation.
        SCI1  P411 ----> RXD 
        SCI1  P410 ----> TXD 

        RA4M1-EK
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0  P410 ----> RXD 
        SCI0  P411 ----> TXD 

        RA6M1-EK, RA4M3-EK and RA4M2-EK, EK-RA6M5
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0 P100 ----> RXD 
        SCI0 P101 ----> TXD 

        RA6M3-EK and RA6M3G-EK
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0 P410 ----> RXD 
        SCI0 P411 ----> TXD 

        RA4W1-EK
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0 P104 ----> RXD 
        SCI0 P101 ----> TXD 

        RA6M4-EK
        -------
        Channel 1 has been used by SCI_UART Loopback operation.
        SCI0 P708 ----> RXD 
        SCI0 P709 ----> TXD 

        RA2L1-EK
        -------
        Channel 1 has been used by SCI_UART Loopback operation.
        SCI1 P212 ----> RXD
        SCI1 P213 ----> TXD

        RA6T1-RSSK
        -------
        Channel 4 has been used by SCI_UART Loopback operation.
        SCI4 P206 ----> RXD 
        SCI4 P205 ----> TXD

        RA2E1-EK
        -------
        Channel 1 has been used by SCI_UART Loopback operation.
        SCI1 P502 ----> RXD 
        SCI1 P501 ----> TXD

        RA4E1-FPB
        Channel 3 has been used by SCI_UART Loopback operation.
        SCI3 P408 ----> RXD 	
        SCI3 P409 ----> TXD

        RA6E1-FPB
        -------
        Channel 2 has been used by SCI_UART Loopback operation.
        SCI2 P301 ----> RXD 
        SCI2 P302 ----> TXD

        RA2E2-EK
        -------
        Channel 9 has been used by SCI_UART Loopback operation.
        SCI9 P401 ----> RXD 
        SCI9 P400 ----> TXD
		
		RA6T2-MCK
        -------
        Channel 1 has been used by SCI_UART Loopback operation.
        SCI1 PB08 ----> RXD 
        SCI1 PB09 ----> TXD