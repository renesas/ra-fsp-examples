/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     	This project demonstrates the basic functionality of the USBX Peripheral Printer driver on Renesas RA MCUs based on
      	Renesas FSP using Azure RTOS. An RA MCU board will be connected to the PC where the RA board will act as a peripheral 
      	printer and the PC (Windows System) will act as a host device. A print command for a test file will be sent from the 
      	host PC, and printing data will be transferred from the host PC to MCU. The RA MCU will display Windows machine printer 
      	information, settings, and print data on the J-Link RTT Viewer. Error and info messages will be printed
	on J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4E2, EK-RA6E2,
			     EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2
	1 x RA board.
	2 x Type-C USB cables.
	1 x Windows PC with at least 2 USB ports.

3. Configuration Settings:
	To select USB Speed in configuration:
    	i.  Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4E2, EK-RA6E2,
                         EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2)
            USB Speed                     :    Full Speed
            USB Module Number             :    USB_IP0 Port

        ii. High-Speed: (For EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1)
            USB Speed                     :    Hi Speed
            USB Module Number             :    USB_IP1 Port

4. Hardware Connections:
	The user needs to confirm the default USB speed selection used in the example project prior to set up
	the hardware connection.
	
	For EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full Speed):		
		- Connect RA board to Host PC machine using micro USB cable.
		- Connect another cable to the Full Speed port (J11) of the RA MCU board.
		- Jumper J12: Connect pins 2-3
		- Connect Jumper J15 pins
	
	For EK-RA6M3 (High Speed):
		- Connect RA board to Host PC machine using micro USB cable.
		- Connect another cable to the High Speed port (J6) of the RA MCU board.
		- Jumper J7: Connect pins 2-3
		- Connect Jumper J17 pins

	For EK-RA4E2, EK-RA6E2 (Full Speed):
		- Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
        	port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High Speed):
		Note: For EK-RA8D1, The user need to turn OFF SW1-6 to use USBHS		
		- Connect RA board to Host PC machine using micro USB cable.
		- Connect another cable to the High Speed port (J31) of the RA MCU board.
		- Jumper J7: Connect pins 2-3
		- Connect Jumper J17 pins

	For MCK-RA8T1 (Full Speed):
		- Connect RA board to Host PC machine using Type-C cable.
		- Connect another cable to the Full Speed port (CN14) of the RA MCU board.
		- Jumper JP9: Connect pins 2-3
       	 	- Connect Jumper JP10 pins

	For EK-RA4L1 (Full Speed):
		- Turn ON S4-4 to select USB device mode.
		- Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		- Connect the RA board USB FS port (J11) to the PC via Type-C USB cable.
		- Connect the RA board USB debug port (J10) to the PC via Type-C USB cable for EP debugging.
	
	For EK-RA8E2 (Full Speed):
		- Connect the RA board USB FS port (J11) to the PC via Type-C USB cable.
		- Connect the RA board USB debug port (J10) to the PC via Type-C USB cable for EP debugging.

5. Verifying Operation:
	Following is the sequence used by the application.
	a) Connect the RA board (Peripheral Printer) to host PC via two USB Cables: One for USB Debug port 
           and one for USB HS or FS port.
	b) In Device Manger, printer should be detected as "Generic / Text Only" under Print queues section.
	c) On the Host PC, select any text file or print Test page from the printer properties and execute print command.
	d) MCU will receive the print command and start printing the data.
	e) The printer settings and printing data will be displayed on the J-Link RTT Viewer.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220401a0
   b. Keil: 	Not Available
   c. IAR: 	Not Available
   
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
