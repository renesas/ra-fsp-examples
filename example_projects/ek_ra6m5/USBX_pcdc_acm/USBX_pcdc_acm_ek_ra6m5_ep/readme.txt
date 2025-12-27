/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

This project demonstrates the basic functionality of USBX Device CDC ACM module on Renesas RA MCUs based on Renesas FSP using AzureRTOS. In this example project, user will use the RA board as a CDC device which will enumerate as a COM port when connected to the host PC. Once the COM port is enabled, user can provide input from the Serial terminal. User input will be echoed on the serial terminal. The application status messages will be displayed on the Jlink RTT Viewer.
 
 
2. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below instructions to observe    the operation of the system.

   Supported Boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4 and EK-RA6M5:

   	1) On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device (COMxx) and note down 
           the COM port number for reference in the next step. 
   
   	2) Open the serial port of the device using tera term.
   
   	3) Open the RTT viewer to view the application status messages.
     
3. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	SEGGER J-Link RTT Viewer: Version 8.92
	Terminal Console Application: Tera Term or a similar application
	USB Serial Drivers (included in Windows 10) 
	Windows® 10 operating system

4. Hardware Requirements:
   
   Hardware : 
	1) RA board 
	2) 2x Micro USB device cable
	3) A PC with at least 2 USB port(1 for debug and 1 for COM port)

5. Configuration changes to switch Speed in configurator: 

    	i.  Full-Speed : (EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4)
            USB Speed                     :    Full Speed
            USB Module Number             :    USB_IP0 Port

        ii. High-Speed :(EK-RA6M3, EK-RA6M5)
            USB Speed                     :    Hi Speed
            USB Module Number             :    USB_IP1 Port
	
6. Hardware settings for the project:
	
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

	EK-RA6M3 (High Speed)
	       	1. Jumper J7: Connect pins 2-3
        	2. Connect Jumper J17 pins
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	   	   port (J6) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.

	EK-RA6M5 (High Speed)
	       	1. Jumper J7: Connect pins 2-3
        	2. Connect Jumper J17 pins
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	   	   port (J31) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.

NOTE:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000ecc
