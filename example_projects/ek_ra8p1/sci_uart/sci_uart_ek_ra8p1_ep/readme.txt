/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	The example project demonstrates the typical use of the UART HAL module APIs. The project initializes the UART
	with Baud rate of 115200 bps and GPT in the PWM mode. Using a Terminal Program (like Tera Term), the user can
	provide a value & press enter key to set the intensity of the on-board LED. The range of input values are
	displayed on the J-Link RTT Viewer. Any failure will also be displayed using J-Link RTT Viewer. To see the user
	input values on Serial terminal, enable local echo option.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	LLVM Embedded Toolchain for ARM: Version 18.1.3
	Terminal Console Application: Tera Term or a similar application

	In case of using Keil or IAR IDEs, please refer to the Tools section in the Release Notes to ensure that the 
	appropriate software versions are installed: https://github.com/renesas/fsp/releases

3. Hardware Requirements:
	Supported RA boards: CK-RA6M5, EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, 
			     EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, 
			     EK-RA8D1, EK-RA8M1, FPB-RA2E3, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, 
			     MCK-RA8T1, RSSK-RA6T1, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA2L2, EK-RA8P1
 	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x USB to UART Converter (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
	1 x Micro USB cable used to connect the Pmod USBUART to the RA board.
	Some jumper wires.

4. Hardware Connections:
	- Connect the RA board to the host PC via a Type-C USB cable for programming and debugging.
	- Connect the Pmod USBUART to the host PC via a Micro USB cable.
	- For Pmod USBUART: Jumper JP1 placement is LCL-VCC pins (The attached system board is powered independently
          from the PMOD USBUART).
	- Connect the Pmod USBUART to the RA board as shown below:
    		For EK-RA2A1:
    			RXD MISO P301	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P302 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND

    		For EK-RA4M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6E2, EK-RA4E2, CK-RA6M5, MCK-RA4T1, MCK-RA6T3:
    			RXD MISO P410 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P411 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND

    		For EK-RA6M1, EK-RA6M4, FPB-RA6E1, FPB-RA2E3:
    			RXD MISO P100 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P101 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND
	
    		For RSSK-RA6T1, EK-RA4W1:
    			RXD MISO P206 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P205 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND
	
    		For EK-RA2L1:
    			RXD MISO P301 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P302 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND
    			Connect P414  	---> P505
	
    		For EK-RA4M3:
    			RXD MISO P202 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P203 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND

    		For EK-RA4M2:
    			RXD MISO P601 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P602 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND

    		For EK-RA2E1:
    			RXD MISO P110 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P109 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND
    			Connect P212  	---> P913

    		For EK-RA6M5:
    			RXD MISO P100 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P101 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND
    			Connect P104  	---> P006

    		For FPB-RA4E1:
    			RXD MISO P100 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P101 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND
    			Connect P104  	---> P407     

    		For EK-RA2E2:
    			RXD MISO P401 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P400 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND

    		For MCK-RA6T2:
    			RXD MISO PA10 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI PA09 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND

    		For EK-RA8M1, EK-RA8D1:
    			RXD MISO PA02 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI PA03 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND
	
    		For MCK-RA8T1:
    			RXD MISO P602 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P603 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND    	---> GND Pin of Pmod USBUART (J2:5)
    
    		For EK-RA2A2:
    			RXD MISO P500 	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P501 	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND
    			Connect P403 	---> P308

		For FPB-RA8E1:
    			RXD MISO P602 (J3:5)	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P603 (J3:6)  	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND    (J3:20)  	---> GND Pin of Pmod USBUART (J2:5)

		For EK-RA4L1:
    			RXD MISO P507 (J1:19)  	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P506 (J1:20)  	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND    (J1:25)  	---> GND Pin of Pmod USBUART (J2:5)

		For EK-RA8E2:
    			RXD MISO P414 (J2:21)	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD MOSI P415 (J2:22)	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND    (J2:39)	---> GND Pin of Pmod USBUART (J2:5)

		For EK-RA2L2:
			RXD MISO P301 (J2:24)	---> TXD Pin of Pmod USBUART (J2:3)
			TXD MOSI P302 (J2:23)	---> RXD Pin of Pmod USBUART (J2:2)
			Common GND    (J2:40)	---> GND Pin of Pmod USBUART (J2:5)
			LED pin P204  (J2:17)	---> GPT Output pin P110 (J2:28)

		For EK-RA8P1:
    			RXD0 P602  (J4:8)	---> TXD Pin of Pmod USBUART (J2:3)
    			TXD0 P603  (J4:4)	---> RXD Pin of Pmod USBUART (J2:2)
    			Common GND (J4:19)  	---> GND Pin of Pmod USBUART (J2:5)
﻿
Note:
1) The user can disable SEGGER RTT features by changing the values of the following macros to 1:
	DISABLE_APP_PRINT
	DISABLE_APP_ERR_PRINT
	DISABLE_APP_ERR_TRAP
	DISABLE_APP_READ
	DISABLE_APP_CHECK_DATA
   These macros are defined in common_utils.h, please note that disabling part of the application may break its functionality.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil:	0x22000850
   c. IAR: 0x22000918
	 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
