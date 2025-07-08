/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	The example project demonstrates the typical use of the UART HAL module APIs.
    	The project initializes the UART with Baud rate of 115200 bps and GPT in the PWM mode.
  	Using a Terminal Program (like Tera Term) user can provide a value & press enter key to set the intensity of the 
	on-board LED.
    	The range of input values are displayed on the J-Link RTT Viewer.
    	Any failure will also be displayed using J-Link RTT Viewer.
	To see user input values on Serial terminal, enable local echo option.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.0.0
	e2 studio: Version 2025-04.1
	SEGGER J-Link RTT Viewer: Version 8.44a
	LLVM Embedded Toolchain for ARM: Version 18.1.3
	Terminal Console Application: Tera Term or a similar application.

3. Hardware Requirements:
	Supported RA boards: CK-RA6M5, EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, 
			     EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, 
			     EK-RA8D1, EK-RA8M1, FPB-RA2E3, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, 
			     RSSK-RA6T1, FPB-RA8E1
 	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x CP2102 USB - TTL connector.

4. Hardware Connections:

    	For EK-RA2A1:
    		RXD MISO P301 -------> TXD Pin of USB-UART
    		TXD MOSI P302 -------> RXD Pin of USB-UART
    		Common GND

    	For EK-RA4M1/EK-RA6M2/EK-RA6M3/EK-RA6M3G/EK-RA6E2/EK-RA4E2:
    		RXD MISO P410 -------> TXD Pin of USB-UART
    		TXD MOSI P411 -------> RXD Pin of USB-UART
    		Common GND

    	For EK-RA6M1/EK-RA6M4/FPB-RA6E1/FPB-RA2E3:
    		RXD MISO P100 -------> TXD Pin of USB-UART
    		TXD MOSI P101 -------> RXD Pin of USB-UART
    		Common GND
	
    	For RSSK-RA6T1/EK-RA4W1:
    		RXD MISO P206 -------> TXD Pin of USB-UART
    		TXD MOSI P205 -------> RXD Pin of USB-UART
    		Common GND
	
    	For EK-RA2L1:
    		RXD MISO P301 -------> TXD Pin of USB-UART
    		TXD MOSI P302 -------> RXD Pin of USB-UART
    		Common GND
    		Connect P414  -------> P505
	
    	For EK-RA4M3:
    		RXD MISO P202 -------> TXD Pin of USB-UART
    		TXD MOSI P203 -------> RXD Pin of USB-UART
    		Common GND

    	For EK-RA4M2:
    		RXD MISO P601 --------> TXD Pin of USB-UART
    		TXD MOSI P602 --------> RXD Pin of USB-UART
    		Common GND

    	For EK-RA2E1:
    		RXD MISO P110 --------> TXD Pin of USB-UART
    		TXD MOSI P109 --------> RXD Pin of USB-UART
    		Common GND
    		Connect P212 ---------> P913

    	For EK-RA6M5:
    		RXD MISO P100 --------> TXD Pin of USB-UART
    		TXD MOSI P101 --------> RXD Pin of USB-UART
    		Common GND
    		Connect P104  --------> P006

    	For FPB-RA4E1:
    		RXD MISO P100 --------> TXD Pin of USB-UART
    		TXD MOSI P101 --------> RXD Pin of USB-UART
    		Common GND
    		Connect P104 ---------> P407     

    	For EK-RA2E2:
    		RXD MISO P401 --------> TXD Pin of USB-UART
    		TXD MOSI P400 --------> RXD Pin of USB-UART
    		Common GND

    	For MCK-RA6T2:
    		RXD MISO PA10 --------> TXD Pin of USB-UART
    		TXD MOSI PA09 --------> RXD Pin of USB-UART
    		Common GND

    	For EK-RA8M1/EK-RA8D1:
    		RXD MISO PA02 --------> TXD Pin of USB-UARTE
    		TXD MOSI PA03 --------> RXD Pin of USB-UART
    		Common GND
	
    	For MCK-RA8T1:
    		RXD MISO P602  --------> TXD Pin of USB-UART
    		TXD MOSI P603  --------> RXD Pin of USB-UART
    		Common GND     --------> GND Pin of USB-UART
    
    	For EK-RA2A2:
    		RXD MISO P500 	-------> TXD Pin of USB-UART
    		TXD MOSI P501 	-------> RXD Pin of USB-UART
    		Common GND
    		Connect P403 	-------> P308

	For FPB-RA8E1:
    		RXD MISO P602 (J3 Pin 05)  --------> TXD Pin of USB-UART
    		TXD MOSI P603 (J3 Pin 06)  --------> RXD Pin of USB-UART
    		Common GND    (J3 Pin 20)  --------> GND Pin of USB-UART
﻿
Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060040
   b. Keil:	0x22060850
   c. IAR: 0x22060900
	 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
