/***********************************************************************************************************************
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
* **********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the CRC HAL module APIs.
        The project demonstrates CRC operation for data transmission in normal mode and reception in snoop mode through sci interface.
	If board not support snoop mode (RA4E2), reception in normal mode through sci interface.
        On Pressing any key through RTT Viewer CRC value in normal mode is calculated for 4 bytes of data.
        The calculated CRC value along with input data is transmitted and received on sci_uart through loop-back.
        Once the transfer is complete, and if CRC value for snoop mode is zero and transmit and receive buffer are equal then On-board
        LED blinks as sign of successful CRC operation. On data mismatch, LED stays ON. Failure and status messages are displayed on
        RTT Viewer.

Note:
* For any event or API failure appropriate messages is displayed on RTT viewer.
* User can change the polynomial to CRC_16 and CRC_CCITT and bit order from MSB to LSB from CRC configurator and observe the results.
* The application does not work for CRC_32 bit polynomial.
* Please refer the latest FSP User Manual for comparison with popular online CRC calculator and the associated limitations are captured in it.

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

        RA6M4-EK and RA2L1-EK
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
        SCI3 P408 ----> RXD 	
        SCI3 P409 ----> TXD

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
        SCI1 P408 ----> RXD 
        SCI1 P409 ----> TXD
	
	RA4E2-EK
        -------
        Channel 0 has been used by SCI_UART Loopback operation.
        SCI0  P410 ----> RXD 
        SCI0  P411 ----> TXD 

        1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
        RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000044
        b. Keil: Not Available 
        c. IAR: Not Available
 
        2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
        in .map file generated in the build configuration folder (Debug/Release).	