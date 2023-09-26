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

This project demonstrates the basic functionality of USBX Device CDC ACM module on Renesas RA MCUs based on Renesas FSP using AzureRTOS. In this example project, user will use the RA board as a CDC device which will enumerate as a COM port when connected to the host PC. Once the COM port is enabled, user can provide input from the Serial terminal. User input will be echoed on the serial terminal. The application status messages will be displayed on the Jlink RTT Viewer.
 
 
2. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below instructions to observe    the operation of the system.

   Supported Boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4 and EK-RA6M5:

   	1) On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device (COMxx) and note down 
           the COM port number for reference in the next step. 
   
   	2) Open the serial port of the device using tera term.
   
   	3) Open the RTT viewer to view the application status messages.
     
3. Hardware and Software Requirements:
   
   Hardware : 
	1) RA board 
	2) 2x Micro USB device cable
	3) A PC with at least 2 USB port(1 for debug and 1 for COM port)
    
   Software: 
	1) Serial Terminal/TeraTerm
        2) Windows® 10 operating system 
	3) USB Serial Drivers (included in Windows 10) 
        4) Jlink RTT viewer

4. Configuration changes to switch Speed in configurator: 

    	i.  Full-Speed : (EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4)
            USB Speed                     :    Full Speed
            USB Module Number             :    USB_IP0 Port

        ii. High-Speed :(EK-RA6M3, EK-RA6M5)
            USB Speed                     :    Hi Speed
            USB Module Number             :    USB_IP1 Port
	
5. Hardware settings for the project:
	
    Jumper Settings:
     	
	EK-RA6M1, EK-RA6M2 (Full Speed)
        	1. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  	   port (J11) of the respective board. 
		2. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   	   port (J9) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.

	EK-RA6M4, EK-RA4M2, EK_RA4M3 (Full Speed)
        	1. Jumper J12 placement is pins 2-3
        	2. Connect Jumper J15 pins
        	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   	   port (J11) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.

	EK-RA6M3, EK-RA6M5 (High Speed)
	       	1. Jumper J7: Connect pins 2-3
        	2. Connect Jumper J17 pins
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	   	   port (J6) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.

NOTE:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000abc
