/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of the USBX Host Printer driver on Renesas RA MCUs based on 
    	Renesas FSP using Azure RTOS. An RA MCU board will be connected to the PC where the RA board will act as a Host
    	device and the Printer will act as a peripheral device. A print command for a test page will be sent from the 
    	RA Board, and printing data will be transferred from the host device to printer. RA MCU will display printer 
    	information, error and info messages will be printed on J-Link RTT Viewer.

    	Following is the sequence used by the application:
		1. Connect two USB Cables 
	   		a. One from the RA board (USB Debug port) to the PC for Debugging.
	   		b. One from the RA board (USB HS/FS port) to the printer.
		2. The printer information will be displayed on the J-Link RTT Viewer.
		3. On the Host device, execute print command.
		4. MCU will send the printing data to the printer.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M5, EK-RA6M3, EK-RA6M4, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2.
	1 x Renesas RA board.
	1 x Type-C USB cable (For programming and debugging).
	1 x OTG cable (For connecting printer).
	1 x Printer device (With USB interface).
	1 x Printer's USB cable for connecting the Printer to board via USB.

3. Hardware Connections:
	The user needs to confirm the default USB speed selection used in the example project prior to set up the hardware connection.

	For EK-RA6M3 (High Speed):
		1. Jumper J7: Connect pins 1-2.
		2. Remove Jumper J17 pins.
		3. Connect DEBUG port of the RA MCU board to the Host PC using a micro USB cable.
		4. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed.
           	port (J6) of the board. Connect the other end of this cable to USB port of the Printer device.

	For EK-RA6M5 (High Speed):
		1. Jumper J7: Connect pins 1-2.
		2. Remove Jumper J17 pins.
		3. Connect DEBUG port of the RA MCU board to the Host PC using a micro USB cable.
		4. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed.
		port (J31) of the board. Connect the other end of this cable to USB port of the Printer device.

	For EK-RA6M4, EK-RA8M1, EK-RA8D1 (Full Speed):
		1. Jumper J12: Connect pins 1-2.
		2. Remove Jumper J15 pins.
		3. Connect DEBUG port of the RA MCU board to the Host PC using a micro USB cable.
		4. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed. 
		port (J11) of the board. Connect the other end of this cable to USB port of the Printer device.

	For MCK-RA8T1 (Full Speed):
		1. Jumper JP9: Connect pins 1-2.
		2. Remove Jumper JP10 pins.
		3. Connect DEBUG port of the RA MCU board to the Host PC using a Type-C USB cable.
		4. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed. 
	  	port (CN14) of the board. Connect the other end of this cable to USB port of the Printer device.

	For EK-RA4L1 (Full Speed):
		1. Turn OFF S4-4 to use USB Host Controller.
		2. Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		3. Connect DEBUG port of the RA MCU board to the Host PC using a Type-C USB cable.
		4. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed. 
	  	port (J11) of the board. Connect the other end of this cable to USB port of the Printer device.
	
	For EK-RA8E2 (Full Speed):
		1. Connect DEBUG port of the RA MCU board to the Host PC using a Type-C USB cable.
		2. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed. 
	  	port (J11) of the board. Connect the other end of this cable to USB port of the Printer device.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220511c0
   b. Keil: 	Not Available
   c. IAR: 	Not Available
   
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).