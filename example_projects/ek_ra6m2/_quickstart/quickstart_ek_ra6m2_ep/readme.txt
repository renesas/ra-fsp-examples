/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
   
   This Quick Start Guide (QSG) provides an overview of:
   -Quick Start example project that the EK-RA6M2 board comes pre-programmed with. 
   -Instructions for running the Quick Start example project. 
   -Instructions for importing, modifying, and building the Quick Start example project using Flexible Software Package (FSP) and e2 studio Integrated Development
   Environment (IDE).

   To run the Quick Start example project, use the following instructions: 
   1. On power up or RESET, the  user LED (red LED1) start blinking at 
      1 Hz frequency and at 90% intensity. 
	  
   Note:  The debug LED (LED2) will blink or light up orange; this can be ignored for now. 
   
   2. Press the first user button (S1) on the EK-RA6M2 board to change the blinking frequency of the user LED. 
      With every press of the first user button (S1), the frequency will switch from 1 Hz to 5 Hz to 10 Hz and cycle back. 
	  
   3. On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device (COMxx) and note down 
      the COM port number for reference in the next step. 
   
   4. Open the serial port of the device using tera term and press Enter key.
   
   5. Follow the instructions displayed in the tera term window. For more details refer to 'EK-RA6M2 - Quick Start Guide' (doc num: R20QS0009EU).
      Link to access Quick Start Guide - https://www.renesas.com/us/en/document/qsg/ek-ra6m2-quick-start-guide
	   
2. Hardware and Software Requirements:
   
   Hardware : 
	1) EK-RA6M2 board 
	2) Micro USB device cable - 2Nos.
	3) A PC with at least 1 USB port
    
	Software: 
	1) Serial Terminal/TeraTerm
    2) Windows® 10 operating system 
	3) USB Serial Drivers (included in Windows 10) 
	
3. Hardware settings for the project:

	1. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB)
	   port (J11) of the EK-RA6M2 board.  
	2. Connect the other end of this cable to the USB port of the host PC. Power LED (LED2) 
	   on the EK-RA6M2 board lights up, indicating that the EK-RA6M2 board is powered on. 
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   port (J9) of the EK-RA4M1 board.  Connect the other end of this cable to USB port of the 
	   host PC.

 
	Note:  If the EK-RA6M2 board is not powered through micro-AB USB (DEBUG USB) port (J11) 
	       from the host PC, then USB CDC functionality of the Quick Start example project 
	       cannot be demonstrated.



