/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

   This Example project demonstrates the functionality of usb_pcdc driver on Renesas RA MCUs using RA boards.
   
   To run this example project, use the following instructions: 

   Applicable for EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5 and EK-RA8M1:

   1. On power up or RESET, the three user LEDs (blue LED1, green LED2, and red LED3) start blinking at 
      1 Hz frequency and at 90% intensity. 
	  
   Note:  The debug LED (LED5) will blink or light up orange; this can be ignored for now. 

   2. Press the first user button (S1) on the board to change the blinking frequency of the user LEDs. 
      With every press of the first user button (S1), the frequency will switch from 1 Hz to 5 Hz to 10 Hz and cycle back. 
	  
   3. Press the second user button (S2) on the board to change the intensity of the user LEDs. With every press of the 
      user button (S2), the intensity will switch from 90% to 50% to 10% and cycle back. 

   4. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device (COMxx) and note down 
      the COM port number for reference in the next step. 
   
   5. Open the serial port of the device using tera term and press Enter key.
   
   6. Follow the instructions displayed in the tera term window. 
   
   Applicable for EK-RA6M2, EK-RA6M1, EK-RA4M1, EK-RA2A1:

   1. On power up or RESET, the  user LED (red LED1) start blinking at 
      1 Hz frequency and at 90% intensity. 
	  
   Note:  The debug LED (LED2) will blink or light up orange; this can be ignored for now. 

   2. Press the first user button (S1) on the board to change the blinking frequency of the user LED. 
      With every press of the first user button (S1), the frequency will switch from 1 Hz to 5 Hz to 10 Hz and cycle back. 
	  
   3. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device (COMxx) and note down 
      the COM port number for reference in the next step. 
   
   4. Open the serial port of the device using tera term and press Enter key.
   
   5. Follow the instructions displayed in the tera term window.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.6.0
	e2 studio: Version 2024-10
	SEGGER J-Link RTT Viewer: Version 7.98g
	Terminal Console Application: Tera Term or a similar application
	USB Serial Drivers (included in Windows 10) 
	Windows® 10 operating system
   
3. Hardware Requirements:
   
   Hardware : 
	1) RA board 
	2) Micro USB device cable - 2Nos.
	3) A PC with at least 2 USB port(1 for debug and 1 for com port)
    
4. Hardware settings for the project:

Applicable for EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5 and EK-RA8M1:

	1. Connect the micro USB end of the micro USB device cable to micro-AB USB  (DEBUG USB)
	   port (J10) of the respective board.  
	2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	   on the board lights up white, indicating that the connected board is powered on. 
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   port (J11) of the board.  Connect the other end of this cable to USB port of the 
	   host PC.
        4. Jumper J12 placement is pins 2-3.
	5. Connect jumper J15 pins.

Applicable for EK-RA4E2:

	1. Connect the micro USB end of the micro USB device cable to micro-AB USB  (DEBUG USB)
	   port (J10) of the respective board.  
	2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	   on the board lights up white, indicating that the connected board is powered on. 
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   port (J11) of the board.  Connect the other end of this cable to USB port of the 
	   host PC.

	Note:  If the board is not powered through micro-AB USB (DEBUG USB) port (J10) 
	       from the host PC, then the functionality cannot be demonstrated

       Connection on EK_RA6M5:
        1. Connect P103 to P006  
        2. Connect P405 to P007
        3. Connect P512 to P008

Applicable for EK-RA6M2, EK-RA6M1, EK-RA4M1, EK-RA2A1:

    1. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	   port (J11) of the respective board.  
	2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED2) 
	   on the board lights up, indicating that the connected board is powered on. 
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   port (J9) of the board.  Connect the other end of this cable to USB port of the 
	   host PC.

Applicable only for EK-RA2A1: 

        4. Connect VREFLO(J2) pin to AVSS0(J2) through jumper wire to get correct display of MCU die 
           temperature.
 
	Note:  If the respective board is not powered through micro-AB USB (DEBUG USB) port (J11) 
	       from the host PC, then the functionality cannot be demonstrated.


