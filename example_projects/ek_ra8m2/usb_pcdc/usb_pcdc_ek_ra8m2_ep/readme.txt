/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
   	This Example project demonstrates the functionality of usb_pcdc driver on Renesas RA MCUs using RA boards.
   	To run this example project, use the following instructions: 

   	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA4L1:

   		1. On power up or RESET, the three user LEDs (blue LED1, green LED2 and red LED3) start blinking at 
      		   1 Hz frequency and at 90% intensity. 
	  
   		Note: The debug LED (LED5) will blink or light up orange, this can be ignored for now. 

   		2. Press the first user button (S1) on the board to change the blinking frequency of the user LEDs. 
      		   With every press of the first user button (S1), the frequency will switch from 1 Hz to 5 Hz to
		   10 Hz and cycle back. 
	  
   		3. Press the second user button (S2) on the board to change the intensity of the user LEDs. With every
		   press of the user button (S2), the intensity will switch from 90% to 50% to 10% and cycle back. 

   		4. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device
		   (COMxx) and note down the COM port number for reference in the next step. 
   
   		5. Open the serial port of the device using Tera Term and press Enter key.
   
   		6. Follow the instructions displayed in the Tera Term window.
   
   	For EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2:

   		1. On power up or RESET, the user LED (red LED1) start blinking at 1 Hz frequency and at 90% intensity. 
	  
   		Note: The debug LED (LED2) will blink or light up orange, this can be ignored for now. 

   		2. Press the first user button (S1) on the board to change the blinking frequency of the user LED. 
      		   With every press of the first user button (S1), the frequency will switch from 1 Hz to 5 Hz to 10 Hz
		   and cycle back. 
	  
   		3. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device
		   (COMxx) and note down the COM port number for reference in the next step. 
   
   		4. Open the serial port of the device using Tera Term and press Enter key.
   
   		5. Follow the instructions displayed in the Tera Term window.

   	For EK-RA4E2:

   		1. On power up or RESET, the two user LEDs (blue LED1, green LED2) start blinking at 1 Hz frequency and
		   at 90% intensity. 
	  
   		Note: The debug LED (LED6) will blink or light up orange; this can be ignored for now. 

   		2. Press the first user button (S1) on the board to change the blinking frequency of the user LEDs. 
      		   With every press of the first user button (S1), the frequency will switch from 1 Hz to 5 Hz to 10 Hz
		   and cycle back. 
	  
   		3. Press the second user button (S2) on the board to change the intensity of the user LEDs. With every
		   press of the user button (S2), the intensity will switch from 90% to 50% to 10% and cycle back. 

   		4. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device
		   (COMxx) and note down the COM port number for reference in the next step. 
   
   		5. Open the serial port of the device using Tera Term and press Enter key.
   
   		6. Follow the instructions displayed in the Tera Term window.

   	For EK-RA6E2:

   		1. On power up or RESET, the two user LEDs (blue LED1, red LED3) start blinking at 1 Hz frequency and
		   at 90% intensity. 
	  
   		Note: The debug LED (LED6) will blink or light up orange; this can be ignored for now. 

   		2. Press the first user button (S1) on the board to change the blinking frequency of the user LEDs. 
      		   With every press of the first user button (S1), the frequency will switch from 1 Hz to 5 Hz to 10 Hz
		   and cycle back. 
	  
   		3. Press the second user button (S2) on the board to change the intensity of the user LEDs. With every
		   press of the user button (S2), the intensity will switch from 90% to 50% to 10% and cycle back. 

   		4. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device
		   (COMxx) and note down the COM port number for reference in the next step. 
   
   		5. Open the serial port of the device using Tera Term and press Enter key.
   
   		6. Follow the instructions displayed in the Tera Term window. 

   	For MCK-RA8T1:

   		1. On power up or RESET, the four user LEDs (LED1, LED2, LED3 and LED4) start blinking at 1 Hz frequency
		   and at 90% intensity. 
	  	
		Note: The debug LED (LED5) will blink or light up orange, this can be ignored for now. 

   		2. Press the button BTN1 on the PMOD BTN to change the blinking frequency of the user LEDs. 
      		   With every press of the button BTN1, the frequency will switch from 1 Hz to 5 Hz to 10 Hz and cycle back. 
	  
   		3. Press the button BTN3 on the PMOD BTN to change the intensity of the user LEDs. With every press of the 
      		   button BTN3, the intensity will switch from 90% to 50% to 10% and cycle back. 

   		4. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device
		   (COMxx) and note down the COM port number for reference in the next step. 
   
   		5. Open the serial port of the device using Tera Term and press Enter key.
   
   		6. Follow the instructions displayed in the Tera Term window.

  	For EK-RA8E2:

   		1. On power up or RESET, the three user LEDs (blue LED1, green LED2 and red LED3) start blinking at 
      		   1 Hz frequency and at 90% intensity. 
	  
   		Note: The debug LED (LED5) will blink or light up orange, this can be ignored for now. 

   		2. Press the first user button (SW1) on the board to change the blinking frequency of the user LEDs. 
      		   With every press of the first user button (SW1), the frequency will switch from 1 Hz to 5 Hz to
		   10 Hz and cycle back. 
	  
   		3. Press the second user button (SW2) on the board to change the intensity of the user LEDs. With every
		   press of the user button (SW2), the intensity will switch from 90% to 50% to 10% and cycle back. 

   		4. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device
		   (COMxx) and note down the COM port number for reference in the next step. 
   
   		5. Open the serial port of the device using Tera Term and press Enter key.
   
   		6. Follow the instructions displayed in the Tera Term window.

  	For EK-RA8P1:

   		1. On power up or RESET, the three user LEDs (blue LED1, green LED2 and red LED3) start blinking at 
      		   1 Hz frequency and at 90% intensity. 
	  
   		Note: The debug LED (LED5) will blink or light up orange, this can be ignored for now. 

   		2. Press the first user button (SW1) on the board to change the blinking frequency of the user LEDs. 
      		   With every press of the first user button (SW1), the frequency will switch from 1 Hz to 5 Hz to
		   10 Hz and cycle back. 
	  
   		3. Press the second user button (SW2) on the board to change the intensity of the user LEDs. With every
		   press of the user button (SW2), the intensity will switch from 90% to 50% to 10% and cycle back. 

   		4. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device
		   (COMxx) and note down the COM port number for reference in the next step. 
   
   		5. Open the serial port of the device using Tera Term and press Enter key.
   
   		6. Follow the instructions displayed in the Tera Term window.

  	For EK-RA8M2:

   		1. On power up or RESET, the three user LEDs (blue LED1, green LED2 and red LED3) start blinking at 
      		   1 Hz frequency and at 90% intensity. 
	  
   		Note: The debug LED (LED5) will blink or light up orange, this can be ignored for now. 

   		2. Press the first user button (SW1) on the board to change the blinking frequency of the user LEDs. 
      		   With every press of the first user button (SW1), the frequency will switch from 1 Hz to 5 Hz to
		   10 Hz and cycle back. 
	  
   		3. Press the second user button (SW2) on the board to change the intensity of the user LEDs. With every
		   press of the user button (SW2), the intensity will switch from 90% to 50% to 10% and cycle back. 

   		4. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device
		   (COMxx) and note down the COM port number for reference in the next step. 
   
   		5. Open the serial port of the device using Tera Term and press Enter key.
   
   		6. Follow the instructions displayed in the Tera Term window.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application
   
3. Hardware Requirements:
	1 x Renesas RA board.
   	2 x Type-C USB cables.
	1 x PC with at least 2 USB ports (1 for debug and 1 for COM port).
    
4. Hardware Connections:
	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1 and EK-RA8D1:
		1. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB)
	   	   port (J10) of the respective board.
		2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	           on the board lights up white, indicating that the connected board is powered on.
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	           port (J11) of the board. Connect the other end of this cable to USB port of the host PC.
        	4. Jumper J12 placement is pins 2-3.
		5. Connect jumper J15 pins.

       		Connection on EK-RA6M5:
        		1. Connect P103 to P006
        		2. Connect P405 to P007
        		3. Connect P512 to P008

	For EK-RA4E2, EK-RA6E2:
		1. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB)
	   	   port (J10) of the respective board.
		2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	   	   on the board lights up white, indicating that the connected board is powered on.
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	           port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

	For EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2:
    		1. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	   	   port (J11) of the respective board.
    		2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED2) 
	   	   on the board lights up, indicating that the connected board is powered on.
    		3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   	   port (J9) of the board. Connect the other end of this cable to USB port of the host PC.

		Note*: For EK-RA2A1, connect VREFLO (J2) pin to AVSS0 (J2) through jumper wire to get correct
		       display of MCU die temperature.

	For MCK-RA8T1:
		1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB (DEBUG USB) 
	   	   port (CN11) of the respective board.
    		2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED6) 
	   	   on the board lights up, indicating that the connected board is powered on.
    		3. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed 
	   	   port (CN14) of the board. Connect the other end of this cable to USB port of the host PC.
    		4. Jumper JP9 placement is pins 2-3.
    		5. Connect jumper JP10 pins.
    
       		Connection on MCK-RA8T1:
        		1. Connect PA14 (CN2:19) to P112 (CN2:1)
        		2. Connect P606 (CN4:18) to P410 (CN1:27)
        		3. Connect PMOD BTN (J1:1) to PMOD (CN6:1)

	For EK-RA4L1:
		1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB (DEBUG USB) 
	   	   port (J10) of the respective board.
    		2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	   	   on the board lights up, indicating that the connected board is powered on.
    		3. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed 
	   	   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.
    		4. The user must turn ON S4-4 to select USB device mode.
    		5. Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.

	For EK-RA8E2:
		1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB (DEBUG USB) 
	   	   port (J10) of the respective board.
    		2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	   	   on the board lights up, indicating that the connected board is powered on.
    		3. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed 
	   	   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

	For EK-RA8P1:
		1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB (DEBUG USB) 
	   	   port (J10) of the respective board.
    		2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	   	   on the board lights up, indicating that the connected board is powered on. 
    		3. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed 
	   	   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

	For EK-RA8M2:
		1. The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		   and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		2. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB (DEBUG USB) 
	   	   port (J10) of the respective board.
    		3. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	   	   on the board lights up, indicating that the connected board is powered on. 
    		4. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed 
	   	   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.
