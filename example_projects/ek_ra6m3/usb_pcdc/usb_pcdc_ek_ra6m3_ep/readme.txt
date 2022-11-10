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

   This Example project demonstrates the functionality of usb_pcdc driver on Renesas RA MCUs using RA boards.
   
   To run this example project, use the following instructions: 

   Applicable for EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4 and EK-RA6M5:

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

   
2. Hardware and Software Requirements:
   
   Hardware : 
	1) RA board 
	2) Micro USB device cable - 2Nos.
	3) A PC with at least 2 USB port(1 for debug and 1 for com port)
    
	Software: 
	1) Serial Terminal/TeraTerm
        2) Windows® 10 operating system 
	3) USB Serial Drivers (included in Windows 10) 
	
3. Hardware settings for the project:

Applicable for EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4 and EK-RA6M5:

	1. Connect the micro USB end of the micro USB device cable to micro-AB USB  (DEBUG USB)
	   port (J10) of the respective board.  
	2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	   on the board lights up white, indicating that the connected board is powered on. 
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   port (J11) of the board.  Connect the other end of this cable to USB port of the 
	   host PC.
        4. Jumper J12 placement is pins 2-3
	5. Connect jumper J15 pins

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

    Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086


