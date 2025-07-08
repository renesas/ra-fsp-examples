/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
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

    EK-RA2A1:
    1) RXD MISO P301 -------> TXD Pin of USB-UART
    2) TXD MOSI P302 -------> RXD Pin of USB-UART
    3) Common GND

    EK-RA4M1:
    1) RXD MISO P410 -------> TXD Pin of USB-UART
    2) TXD MOSI P411 -------> RXD Pin of USB-UART
    3) Common GND

    EK-RA6M1:
    1) RXD MISO P100 -------> TXD Pin of USB-UART
    2) TXD MOSI P101 -------> RXD Pin of USB-UART
    3) Common GND

    EK-RA6M2:
    1) RXD MISO P410 -------> TXD Pin of USB-UART
    2) TXD MOSI P411 -------> RXD Pin of USB-UART
    3) Common GND

    EK-RA6M3:
    1) RXD MISO P410 -------> TXD Pin of USB-UART
    2) TXD MOSI P411 -------> RXD Pin of USB-UART
    3) Common GND
    
    EK-RA6M3G:
    1) RXD MISO P410 -------> TXD Pin of USB-UART
    2) TXD MOSI P411 -------> RXD Pin of USB-UART
    3) Common GND

    EK-RA6M4:
    1) RXD MISO P100 -------> TXD Pin of USB-UART
    2) TXD MOSI P101 -------> RXD Pin of USB-UART
    3) Common GND
	
    RSSK-RA6T1
    1) RXD MISO P206 -------> TXD Pin of USB-UART
    2) TXD MOSI P205 -------> RXD Pin of USB-UART
    3) Common GND
	
    EK-RA2L1:
    1) RXD MISO P301 -------> TXD Pin of USB-UART
    2) TXD MOSI P302 -------> RXD Pin of USB-UART
    3) Common GND
    4) Connect P414  -------> P505
	
    EK-RA4M3:
    1) RXD MISO P202 -------> TXD Pin of USB-UART
    2) TXD MOSI P203 -------> RXD Pin of USB-UART
    3) Common GND

    EK-RA4M2:
    1) RXD MISO P601 --------> TXD Pin of USB-UART
    2) TXD MOSI P602 --------> RXD Pin of USB-UART
    3) Common GND

    EK-RA2E1:
    1) RXD MISO P110 --------> TXD Pin of USB-UART
    2) TXD MOSI P109 --------> RXD Pin of USB-UART
    3) Common GND
    4) Connect P212 ---------> P913

    EK-RA6M5:
    1) RXD MISO P100 --------> TXD Pin of USB-UART
    2) TXD MOSI P101 --------> RXD Pin of USB-UART
    3) Common GND
    4) Connect P104 to P006

    EK-RA4W1:
    1) RXD MISO P206 -------> TXD Pin of USB-UART
    2) TXD MOSI P205 -------> RXD Pin of USB-UART
    3) Common GND

    FPB-RA4E1:
    1) RXD MISO P100 --------> TXD Pin of USB-UART
    2) TXD MOSI P101 --------> RXD Pin of USB-UART
    3) Common GND
    4) Connect P104 ---------> P407     

    FPB-RA6E1:
    1) RXD MISO P100 --------> TXD Pin of USB-UART
    2) TXD MOSI P101 --------> RXD Pin of USB-UART
    3) Common GND

    EK-RA2E2:
    1) RXD MISO P401 --------> TXD Pin of USB-UART
    2) TXD MOSI P400 --------> RXD Pin of USB-UART
    3) Common GND

    MCK-RA6T2:
    1) RXD MISO PA10 --------> TXD Pin of USB-UART
    2) TXD MOSI PA09 --------> RXD Pin of USB-UART
    3) Common GND﻿

    EK-RA6E2 / EK-RA4E2:
    1) RXD MISO P410 --------> TXD Pin of USB-UART
    2) TXD MOSI P411 --------> RXD Pin of USB-UART
    3) Common GND

    EK-RA8M1 / EK-RA8D1:
    1) RXD MISO PA02 --------> TXD Pin of USB-UART
    2) TXD MOSI PA03 --------> RXD Pin of USB-UART
    3) Common GND﻿

    FPB-RA2E3:
    1) RXD MISO P100 -------> TXD Pin of USB-UART
    2) TXD MOSI P101 -------> RXD Pin of USB-UART
    3) Common GND

    MCK-RA8T1
    1) RXD MISO P602 (CN3:P31) --------> TXD Pin of USB-UART
    2) TXD MOSI P603 (CN3:P33) --------> RXD Pin of USB-UART
    3) Common GND    (CN3:P34) --------> GND Pin of USB-UART

Note:
	Users can disable SEGGER RTT features by changing the values of the following macros to 1:
		DISABLE_APP_PRINT
		DISABLE_APP_ERR_PRINT
		DISABLE_APP_ERR_TRAP
		DISABLE_APP_READ
		DISABLE_APP_CHECK_DATA
	These macros are defined in common_utils.h, Please note that disabling part of the application may break its functionality

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220008c0
   b. Keil:	0x22000850
   c. IAR: 0x22000900
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
