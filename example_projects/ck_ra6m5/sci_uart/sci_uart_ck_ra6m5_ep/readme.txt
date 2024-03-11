/***********************************************************************************************************************
* 
* Copyright [2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
    The example project demonstrates the typical use of the UART HAL module APIs.
    The project initializes the UART with Baud rate of 115200 bps and GPT in the PWM mode.
    Using a Terminal Program (like Tera Term) user can provide a value & press enter key to set the intensity of the on-board LED.
    The range of input values are displayed on the JLinkRTTViewer.
    Any failure will also be displayed using JLinkRTTViewer.
	To see user input values on Serial terminal, enable local echo option.

2. Hardware and Software Requirements:
    External Hardware : CP2102 USB - TTL connector
    Software          : Serial Terminal/TeraTerm

3. Hardware settings for the project:
    Wiring details:

    RA2A1-EK:
    1) RXD MISO P301 -------> TXD Pin of USB-UART
    2) TXD MOSI P302 -------> RXD Pin of USB-UART
    3) Common GND

    RA4M1-EK
    1) RXD MISO P410 -------> TXD Pin of USB-UART
    2) TXD MOSI P411 -------> RXD Pin of USB-UART
    3) Common GND

    RA6M1-EK
    1) RXD MISO P100 -------> TXD Pin of USB-UART
    2) TXD MOSI P101 -------> RXD Pin of USB-UART
    3) Common GND

    RA6M2-EK
    1) RXD MISO P410 -------> TXD Pin of USB-UART
    2) TXD MOSI P411 -------> RXD Pin of USB-UART
    3) Common GND

    RA6M3-EK
    1) RXD MISO P410 -------> TXD Pin of USB-UART
    2) TXD MOSI P411 -------> RXD Pin of USB-UART
    3) Common GND
    
    RA6M3G-EK
    1) RXD MISO P410 -------> TXD Pin of USB-UART
    2) TXD MOSI P411 -------> RXD Pin of USB-UART
    3) Common GND

    RA6M4-EK
    1) RXD MISO P100 -------> TXD Pin of USB-UART
    2) TXD MOSI P101 -------> RXD Pin of USB-UART
    3) Common GND
	
    RA6T1-RSSK
    1) RXD MISO P206 -------> TXD Pin of USB-UART
    2) TXD MOSI P205 -------> RXD Pin of USB-UART
    3) Common GND
	
    RA2L1-EK
    1) RXD MISO P301 -------> TXD Pin of USB-UART
    2) TXD MOSI P302 -------> RXD Pin of USB-UART
    3) Common GND
    4) Connect P414  -------> P505
	
    RA4M3-EK
    1) RXD MISO P202 -------> TXD Pin of USB-UART
    2) TXD MOSI P203 -------> RXD Pin of USB-UART
    3) Common GND

    RA4M2-EK
    1)RXD MISO P601 --------> TXD Pin of USB-UART
    2)TXD MOSI P602 --------> RXD Pin of USB-UART
    3) Common GND

    RA2E1-EK
    1)RXD MISO P110 --------> TXD Pin of USB-UART
    2)TXD MOSI P109 --------> RXD Pin of USB-UART
    3)Common GND
    4)Connect P212 ---------> P913

    RA6M5-EK
    1)RXD MISO P100 --------> TXD Pin of USB-UART
    2)TXD MOSI P101 --------> RXD Pin of USB-UART
    3) Common GND
    Connect P104 to P006

    RA4W1-EK
    1) RXD MISO P206 -------> TXD Pin of USB-UART
    2) TXD MOSI P205 -------> RXD Pin of USB-UART
    3) Common GND

    RA4E1-FPB
    1)RXD MISO P100 --------> TXD Pin of USB-UART
    2)TXD MOSI P101 --------> RXD Pin of USB-UART
    3) Common GND
    4)Connect P104 ---------> P407     

    RA6E1-FPB
    1)RXD MISO P100 --------> TXD Pin of USB-UART
    2)TXD MOSI P101 --------> RXD Pin of USB-UART
    3)Common GND

    RA2E2-EK
    1)RXD MISO P401 --------> TXD Pin of USB-UART
    2)TXD MOSI P400 --------> RXD Pin of USB-UART
    3)Common GND
	
    RA6M5-CK
    1)RXD MISO P410 --------> TXD Pin of USB-UART
    2)TXD MOSI P411 --------> RXD Pin of USB-UART
    3)Common GND

﻿
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200004a4
   b. Keil:	Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
