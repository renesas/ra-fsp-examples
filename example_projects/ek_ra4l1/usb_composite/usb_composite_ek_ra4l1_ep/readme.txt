/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USB Composite driver on Renesas RA MCUs based on Renesas
 	FSP using FreeRTOS. In this example, the application will configure the MCU as a CDC device as well as a Mass
	storage device. Tera term is used to communicate with the CDC interface of the MCU. Data written by the tera term
	will be echoed back on the terminal by RA board. The user at the same time can read/write the data from/to the Mass
 	storage device using host machine.

2. Hardware Requirements:
	1 x Renesas RA board.
	2 x Type-C USB cables.
	1 x Host PC with at least 2 USB ports (1 for debug and 1 for COM port).

3. Hardware Connections:
	For EK-RA6M1, EK-RA6M2 (Full-Speed): 
		+ Connect the micro USB end of the micro USB device cable to micro-AB USB Full-Speed 
		  port (J9) of the board. Connect the other end of this cable to USB port of the host PC.

	For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M3, EK-RA6M3G, EK-RA6M5, RK-RA8M1, EK-RA8D1 (Full-Speed): 
		+ Jumper J12: Connect pins 2-3.
		+ Connect the jumper J15 pins. 
		+ Connect the micro USB end of the micro USB device cable to micro-AB USB Full-Speed 
		  port (J11) of the board. Connect the other end of this cable to USB port of the host PC. 

	For MCK-RA8T1 (Full-Speed): 
		+ Jumper JP9: Connect pins 2-3.
		+ Connect the jumper JP10 pins.
		+ Connect the Type-C end of the Type-C USB device cable to Type-C USB Full-Speed 
		  port (CN14) of the board. Connect the other end of this cable to USB port of the host PC.

	For EK-RA4L1 (Full-Speed):
		+ Turn ON S4-4 to select USB device mode.
		+ Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		+ Connect the Type-C end of the Type-C USB device cable to Type-C USB Full-Speed 
		  port (J11) of the board. Connect the other end of this cable to USB port of the host PC.
	
	For EK-RA6M3, EK-RA6M3G (High-Speed): 
		+ Jumper J7: Connect pins 2-3.
		+ Connect the jumper J17 pins.
		+ Connect the micro USB end of the micro USB device cable to micro-AB USB High-Speed 
		  port (J6) of the board. Connect the other end of this cable to USB port of the host PC. 
		
	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed): 
		+ For EK-RA8D1, the user needs to turn OFF SW1-6 to use USB HS.
		+ Jumper J7: Connect pins 2-3.
		+ Connect the jumper J17 pins.
		+ Connect the micro USB end of the micro USB device cable to micro-AB USB High-Speed 
		  port (J31) of the board. Connect the other end of this cable to USB port of the host PC. 

4. USB Disconnect Behaviour:
        The application also handles removing & connecting the Peripheral USB Cable use case.
        Ex: If the user removes the peripheral USB cable from the board, the Composite USB example project will be running 
        and it waits for the cable to be connected. Once the Cable is connected back, the example project works the same.
        
        Note : PMSC Disk content is retained only when the Peripheral USB cable is removed & connected back. 
               If the user removes the power cable of board, the disk content will be lost as the disk is implemented on RAM.

﻿Note:
1) By default, the EP runs in Full-Speed mode, except for EK-RA6M3, EK-RA6M3G and EK-RA8M1 boards.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000fe4
   b. Keil:     Not Available
   c. IAR:      Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
