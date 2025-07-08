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

    Following is the sequence used by the application.

	1. Connect two USB Cables 
	   a. One from the RA board (USB Debug port) to the PC for Debugging.
	   b. One from the RA board (USB HS/FS port) to the printer.
	2. The printer information will be displayed on the J-Link RTT Viewer.
	3. On the Host device, execute print command.
	4. MCU will send the printing data to the printer.


2. Hardware Requirement:

	1. RA MCU board
	2. 2 x Micro USB cable (One for programming and debugging, one for connect between RA board and printer)
	3. 1 x Micro USB-OTG cable for connecting printer
	4. Printer device (with USB interface)

3. Hardware Connections:
	User needs to confirm the default USB speed selection used in the example project prior to set up the hardware connection.
	
	Supported Board (High Speed)- EK-RA6M5, EK-RA6M3:		
		1.Connect DEBUG port of the RA MCU board to the Host PC using a micro USB cable.
		2.Connect the USB High Speed USB port of the RA MCU board to the Printer device using another micro USB cable.

	Supported Board (Full Speed)- EK-RA6M4, EK-RA8M1, EK-RA8D1:		
		1.Connect DEBUG port of the RA MCU board to the Host PC using a micro USB cable.
		2.Connect the USB FullSpeed USB port of the RA MCU board to the Printer device using another micro USB cable.

4. Hardware settings for the project:
	
	Jumper Settings:
	EK-RA6M3(High Speed)
		1. Jumper J7: Connect pins 1-2
		2. Remove Jumper J17 pins
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
           	port (J6) of the board. Connect the other end of this cable to USB port of the Printer device.

	EK-RA6M5 (High Speed)
		1. Jumper J7: Connect pins 1-2
		2. Remove Jumper J17 pins
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
		port (J31) of the board. Connect the other end of this cable to USB port of the Printer device.

	EK-RA6M4/EK-RA8M1/EK-RA8D1 (Full Speed)
		1. Jumper J12: Connect pins 1-2
		2. Remove Jumper J15 pins
		3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
		port (J11) of the board. Connect the other end of this cable to USB port of the Printer device.
			

NOTE:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220136d0
   b. Keil: Not Available 
   c. IAR: Not Available
   
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).