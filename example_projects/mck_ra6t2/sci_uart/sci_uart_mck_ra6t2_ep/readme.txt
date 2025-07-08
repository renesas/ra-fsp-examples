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
    
    RA6T2-MCK
    1)RXD MISO PA10 --------> TXD Pin of USB-UART
    2)TXD MOSI PA09 --------> RXD Pin of USB-UART
    3)Common GND

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
   a. e2studio: 0x200008a8
   b. Keil:	0x2000088c
   c. IAR: 0x200008f4
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
