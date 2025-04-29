/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

      This project demonstrates the basic functionality of the USBX Peripheral Printer driver on Renesas RA MCUs based on 
      Renesas FSP using Azure RTOS. An RA MCU board will be connected to the PC where the RA board will act as a peripheral 
      printer and the PC (Windows System) will act as a host device. A print command for a test file will be sent from the 
      host PC, and printing data will be transferred from the host PC to MCU. RA MCU will display Windows machine printer 
      information, settings, and printing data on the J-Link RTT Viewer. Error and info messages will be printed on J-Link RTT Viewer.

      Following is the sequence used by the application.

        1. Connect two USB Cables to the PC : One for USB Debug port and other for USB HS/FS port.
        2. In Device Manger, printer should be detected as "Generic / Text Only" under Print queues section.
        3. On the Host PC, select any text file or print Test page from the printer properties and execute print command.
        4. MCU will receive the print command and start printing the data.
        5. The printer settings and printing data will be displayed on the J-Link RTT Viewer.


2. Hardware Requirement:

	1. RA MCU board
        2. 2x Micro USB cables or 2x Type C cables (For MCK-RA8T1) 
        3. Host System (Windows PC) with at least 2 USB ports

3. Hardware Connections:

        Supported Board (Full Speed)- EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5 and MCK-RA8T1:		
	1. Connect RA board to Host PC machine using micro USB cable.
        2. Connect another cable to the Full Speed port of the RA MCU board.

 	Supported Board (High Speed)- EK-RA6M3 and EK-RA6M5:		
	1. Connect RA board to Host PC machine using micro USB cable.
        2. Connect another cable to the High Speed port of the RA MCU board.

4. Hardware settings for the project:
	
    Jumper Settings:
	
	EK-RA6M1, EK-RA6M2 (Full Speed)
	1. Connect the micro USB end of the micro USB device cable to micro-AB USB Full-Speed 
	port (J9) of the board. Connect the other end of this cable to USB port of the host PC.
	
	EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4 and EK-RA6M5 (Full Speed)
	1. Jumper J12: Connect pins 2-3
        2. Connect Jumper J15 pins
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
           port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

	EK-RA6M3 (High Speed)
	1. Jumper J7: Connect pins 2-3
        2. Connect Jumper J17 pins
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed
           port (J6) of the board. Connect the other end of this cable to USB port of the host PC.
		   
	EK-RA6M5 (High Speed)
	1. Jumper J7: Connect pins 2-3
        2. Connect Jumper J17 pins
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed
           port (J31) of the board. Connect the other end of this cable to USB port of the host PC.

     	MCK-RA8T1 (Full Speed)
	1. Jumper JP9: Connect pins 2-3
        2. Connect Jumper JP10 pins
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
           port (CN14) of the board. Connect the other end of this cable to USB port of the host PC.

NOTE:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000a40
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).