/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USBX Device CDC ACM module on Renesas RA MCUs based on 
	Renesas FSP using AzureRTOS. In this example project, the user will use the RA board as a CDC device which will 
	enumerate as a COM port when connected to the host PC. Once the COM port is enabled, the user can provide input 
	from the Serial terminal. The user input will be echoed on the serial terminal. The application status messages
	will be displayed on the J-Link RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.8.0
	e2 studio: Version 2025-01
	SEGGER J-Link RTT Viewer: Version 8.12c
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application
	Windows 10 operating system
	USB Serial Drivers (Included in Windows 10)

3. Hardware Requirements:
   	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, 
			     MCK-RA8T1, EK-RA4L1
	1 x RA board 
	2 x Type C USB device cables
	1 x PC with at least 2 USB ports (1 for debug and 1 for COM port)

4. Configuration Settings:
	To select USB Speed in configuration:
    	i.  Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, MCK-RA8T1, EK-RA4L1)
              USB Speed                     :    Full Speed
              USB Module Number             :    USB_IP0 Port

        ii. High-Speed: (For EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1)
              USB Speed                     :    Hi Speed
              USB Module Number             :    USB_IP1 Port

5. Hardware Connections:
	For EK-RA6M1, EK-RA6M2 (Full Speed)
        	1. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  	   port (J11) of the respective board. 
		2. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   	   port (J9) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.

	For EK-RA6M4, EK-RA4M2, EK-RA4M3 (Full Speed)
        	1. Jumper J12 placement is pins 2-3
        	2. Connect Jumper J15 pins
        	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   	   port (J11) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.

	For EK-RA6M3 (High Speed)
	       	1. Jumper J7: Connect pins 2-3
        	2. Connect Jumper J17 pins
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	   	   port (J6) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High Speed)
	       	1. Jumper J7: Connect pins 2-3
        	2. Connect Jumper J17 pins
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	   	   port (J31) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.
		* Note: For EK-RA8D1, the user must turn-off SW1-6 to use USBHS.

	For MCK-RA8T1 (Full Speed)
	       	1. Jumper JP9: Connect pins 2-3
        	2. Connect Jumper JP10 pins
		3. Connect the type C USB end of the type C USB device cable to type C USB Full Speed 
	   	   port (CN14) of the board.  Connect the other end of this cable to USB port of the 
	   	   host PC.

	For EK-RA4L1 (Full Speed)
		1. Turn ON S4-4 to select USB device mode.
		2. Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		3. Connect the RA board USB FS port (J11) to the PC via Type C USB cable.
		4. Connect the RA board USB debug port (J10) to the PC via Type C USB cable for EP debugging.
		
6. Verifying Operation: 
	To run this example project, the user needs to compile and download the example project to the evaluation kit and then follow below 
   	instructions to observe the operation of the system.

   	1) On the host PC, open Windows Device Manager. Expand Ports (COM & LPT), locate USB Serial Device (COMxx) and note down 
           the COM port number for reference in the next step.
   	2) Open the serial port of the device using tera term.
   	3) Open the J-Link RTT Viewer to view the application status messages.

Note:
1) Need to update src code as below if the user changes USB Speed: In rm_usbx_pcdc_descriptor.c file
   	For High Speed: 
	Comment USB_DT_CONFIGURATION, Uncomment USB_DT_OTHER_SPEED_CONF in g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED]
	Uncomment USB_DT_CONFIGURATION, Comment USB_DT_OTHER_SPEED_CONF in g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED]

   	For Full Speed:
	Uncomment USB_DT_CONFIGURATION, Comment USB_DT_OTHER_SPEED_CONF in g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED]
	Comment USB_DT_CONFIGURATION, Uncomment USB_DT_OTHER_SPEED_CONF in g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED]

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000a94
   b. Keil: 	Not Available
   c. IAR:  	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
