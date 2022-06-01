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
   
   This Quick Start Guide (QSG) provides an overview of:
   -Quick Start example project that the EK-RA6M3 board comes pre-programmed with. 
   -Instructions for running the Quick Start example project. 
   -Instructions for importing, modifying, and building the Quick Start example project using Flexible Software Package (FSP) and e2 studio Integrated Development
   Environment (IDE).

   To run the Quick Start example project, use the following instructions: 
   1. On power up or RESET, the three user LEDs (blue LED1, green LED2, and red LED3) start blinking at 
      1 Hz frequency and at 90% intensity. 
	  
   Note:  The debug LED (LED5) will blink or light up orange; this can be ignored for now. 
   
   2. Press the first user button (S1) on the EK-RA6M3 board to change the blinking frequency of the user LEDs. 
      With every press of the first user button (S1), the frequency will switch from 1 Hz to 5 Hz to 10 Hz and cycle back. 
	  
   3. Press the second user button (S2) on the board to change the intensity of the user LEDs. With every press of the 
      user button (S2), the intensity will switch from 90% to 50% to 10% and cycle back. 
	  
   4. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device (COMxx) and note down 
      the COM port number for reference in the next step. 
   
   5. Open the serial port of the device using tera term and press Enter key.
   
   6. Follow the instructions displayed in the tera term window. For more details refer to 'EK-RA6M3 - Quick Start Guide (doc num: R20QS0011EU) 
      Link to access Quick Start Guide - https://www.renesas.com/us/en/document/qsg/ek-ra6m3-quick-start-guide
   
2. Hardware and Software Requirements:
   
   Hardware : 
	1) EK-RA6M3 board 
	2) Micro USB device cable - 2Nos.
	3) A PC with at least 1 USB port
    
	Software: 
	1) Serial Terminal/TeraTerm
        2) Windows® 10 operating system 
	3) USB Serial Drivers (included in Windows 10) 
	
3. Hardware settings for the project:
	1. Connect the micro USB end of the micro USB device cable to micro-AB USB  (DEBUG USB)
	   port (J10) of the EK-RA6M3 board.  
	2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED4) 
	   on the EK-RA6M3 board lights up white, indicating that the EK-RA6M3 board is powered on. 
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   port (J11) of the EK-RA6M3 board.  Connect the other end of this cable to USB port of the 
	   host PC.

 
	Note:  If the EK-RA6M3 board is not powered through micro-AB USB (DEBUG USB) port (J10) 
	       from the host PC, then USB CDC functionality of the Quick Start example project 
	       cannot be demonstrated.



