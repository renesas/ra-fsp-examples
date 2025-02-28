/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USBX Device CDC ACM module on Renesas RA MCUs based on Renesas 
	FSP using AzureRTOS. In this example project, user will use the RA board as a CDC device which will enumerate as a 
	COM port when connected to the host PC. Once the COM port is enabled, user can provide input from the Serial terminal. 
	User input will be echoed on the serial terminal. The application status messages will be displayed on the Jlink RTT Viewer.
 
2. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below 
   instructions to observe the operation of the system.

   Supported Boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1 and EK-RA8D1:

   	1) On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device (COMxx) and note down 
           the COM port number for reference in the next step. 
   
   	2) Open the serial port of the device using tera term.
   
   	3) Open the RTT viewer to view the application status messages.
     
3. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.8.0
	e2 studio: Version 2025-01
	SEGGER J-Link RTT Viewer: Version 8.12c
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

        ii. High-Speed :(EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1)
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

	EK-RA6M5, EK-RA8M1, EK-RA8D1 (High Speed)
	       	1. Jumper J7: Connect pins 2-3
        	2. Connect Jumper J17 pins
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	   	   port (J31) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.
		* Note: For EK-RA8D1, User must turn-off SW1-6 to use USBHS.

Note:
1) Need to update src code as below if user changes USB speed: In rm_usbx_pcdc_descriptor.c file
   For High Speed: 
	Comment USB_DT_CONFIGURATION, Uncomment USB_DT_OTHER_SPEED_CONF in g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED]
	Uncomment USB_DT_CONFIGURATION, Comment USB_DT_OTHER_SPEED_CONF in g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED]

   For Full Speed:
	Uncomment USB_DT_CONFIGURATION, Comment USB_DT_OTHER_SPEED_CONF in g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED]
	Comment USB_DT_CONFIGURATION, Uncomment USB_DT_OTHER_SPEED_CONF in g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED]

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000ad8
   b. Keil: 	Not Available 
   c. IAR:  	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
