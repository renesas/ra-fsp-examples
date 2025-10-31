/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        This project demonstrates the basic functionality of the USB PCDC driver on Renesas RA MCUs based on Renesas FSP using FreeRTOS. 
        In this example, the user will be able to use the RA board as a  CDC to UART converter by connecting through a COM port
        from the host PC. The converter performs the data reception and data transmission asynchronously.
        The user can connect any UART device to receive the data typed in Tera Term, and to send any data to the converter to
        display on Tera Term. The user can also change the baud rate, stop bit and parity bit configuration at run time through
        Tera Term. The status of the application will be displayed on JLink RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application
	USB Serial Drivers (included in Windows 10) 
	Windows® 10 operating system

3. Hardware Requirements:

   Hardware : 
	1) RA board
	2) Pmod USBUART	
	2) 2x Micro USB device cable
	3) A PC with at least 3 USB port(1 for debug, 2 for COM port)
	4) Connecting/Jumper wires for shorting pins   

4. Hardware settings for the project:
   Jumper Settings:

	EK-RA6M3 :
	USB PCDC (High Speed):
	    1. Jumper J7: Connect pins 2-3
	    2. Connect Jumper J17 pins
	    3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	       port (J6) of the board.  Connect the other end of this cable to USB port of the 
	       host PC.
	USB PCDC (Full Speed):
	    1. Jumper J12: Connect pins 2-3
	    2. Connect Jumper J15 pins
	    3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	       port (J11) of the board.  Connect the other end of this cable to USB port of the 
	       host PC.
	UART:
	    1. P411 (TXD) to RX pin of external UART device
	    2. P410 (RXD) to TX pin of external UART device
	    3. When using flow control from TeraTerm, the CTS (P413) should be connected to either RTS(P103) or GND.
           For instructions on how to enable and disable flow control, refer the the FSP User's manual. 

	EK-RA4M2:
	USB PCDC (Full Speed):
	    1. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	       port (J11) of the board.  Connect the other end of this cable to USB port of the 
	       host PC.
	    2. Jumper J12 placement is pins 2-3
	    3. Connect jumper J15 pins

	UART:
	    1. P602 (TXD) to RX pin of external UART device
	    2. P601 (RXD) to TX pin of external UART device
	    3. When using flow control from TeraTerm, the CTS (P603) should be connected to GND.
           For instructions on how to enable and disable flow control, refer the the FSP User's manual.

	EK-RA6M4:
	USB PCDC (Full Speed):
		1. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	       port (J11) of the board.  Connect the other end of this cable to USB port of the 
	       host PC.
		2. Jumper J12 placement is pins 2-3
		3. Connect jumper J15 pins

	UART:
		1. P101 (TXD) to RX pin of external UART device
		2. P100 (RXD) to TX pin of external UART device
		3. When using flow control from TeraTerm, the CTS (P414) should be connected to P413.
           For instructions on how to enable and disable flow control, refer the the FSP User's manual.

	EK-RA2A1 :
	USB PCDC (Full Speed):
		1. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J9) of the board.
	       Connect the other end of this cable through OTG Cable to USB port of the host PC.
	UART:
		1. P302 (TXD) to RX pin of external UART device
		2. P301 (RXD) to TX pin of external UART device
		3. When using flow control from TeraTerm, the CTS (P303) should be connected to GND.
           For instructions on how to enable and disable flow control, refer the the FSP User's manual. 
	
	EK-RA8M1:
	USB PCDC (Full Speed):
		1. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
		   port (J11) of the board.  Connect the other end of this cable to USB port of the 
		   host PC.
		2. Jumper J12 placement is pins 2-3.
		3. Connect jumper J15 pins.
	USB PCDC (High Speed):
		1. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
		   port (J31) of the board.  Connect the other end of this cable to USB port of the 
		   host PC.
		2. Jumper J7 placement is pins 2-3.
		3. Connect jumper J17 pins.	

	UART:
		1. PA03 (TXD) to RX pin of external UART device
		2. PA02 (RXD) to TX pin of external UART device
		3. When using flow control from TeraTerm, the CTS (PA05) should be connected to GND.
		   For instructions on how to enable and disable flow control, refer the the FSP User's manual

5. Configuration changes to switch Speed in configurator based on the example project settings: 

     i. Full-Speed : (EK-RA6M4, EK-RA4M2, EK-RA4M3)
        USB Speed                      :    Full Speed
        USB Module Number              :    USB_IP0 Port
        DMA Source Address             :    FS Address
        DMA Destination Address        :    FS Address
        DMAC Transfer Size             :    2 Bytes

    ii. High-Speed : (EK-RA6M3, EK-RA6M5, EK-RA8M1)
        USB Speed                      :    Hi Speed
        USB Module Number              :    USB_IP1 Port
        DMA Source Address             :    HS Address
        DMA Destination Address        :    HS Address
        DMAC Transfer Size             :    4 Bytes
        	
Note:
1. The user can connect any other UART device to communicate with the USB PCDC to UART converter.
2. To enable testing of the converter, the user can also loop back the TX and RX pins on the RA board USB to UART converter.
3. High-speed is supported the default configuration setting in the project for MCUs which support high-speed USB.
   For MCU with full speed USB only, full speed USB will be default configuration setting. For switching from High-speed to full-speed,
   refer instructions in the README.md.


1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000b40
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
