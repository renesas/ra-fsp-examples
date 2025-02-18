/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of USB PHID driver on Renesas RA MCUs based on Renesas FSP.
	The project configures RA board as a Keyboard. On pressing any key from RTT Viewer, MCU sends characters from 
	a - z & numbers 0 - 9 to the host PC.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA4M3, 
			     EK-RA4M2, EK-RA8M1, EK-RA8D1, EK-RA4E2, EK-RA6E2, MCK-RA8T1, EK-RA4L1
	1 x Renesas RA board.
	2 x Type-C USB cables.
	1 x PC with at least 2 USB ports (1 for debug and 1 for COM port).
	
3. Hardware Connections:
	For EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2 (Full Speed):
		Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J9) of the board.
		Connect the other end of this cable to USB port of the host PC. 
	
	For EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA4M3, EK-RA4M2, EK-RA8M1, EK-RA8D1 (Full Speed):
		Jumper J12 placement is pins 2-3.
		Connect jumper J15 pins. 
		Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J11) of the board.
		Connect the other end of this cable to USB port of the host PC. 

	For EK-RA4E2, EK-RA6E2 (Full Speed): 
		Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J11) of the board.  
		Connect the other end of this cable to USB port of the host PC.

	For MCK-RA8T1 (Full Speed):
		Jumper JP9 placement is pins 2-3.
	 	Connect jumper JP10 pins.
		Connect the Type-C USB end of the Type-C USB cable to Type-C USB Full Speed port (CN14) of the board.  
		Connect the other end of this cable to USB port of the host PC.	

	For EK-RA4L1 (Full Speed):
		Turn ON S4-4 to select USB device mode. 
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect the Type-C USB end of the Type-C USB cable to Type-C USB Full Speed port (J11) of the board.  
		Connect the other end of this cable to USB port of the host PC.

Note:
1) Open Device Manager, and check 'Human Interface Devices', to see ' USB Input Device' has been detected.

2) Lower case a - z and numbers 0 - 9 is send from MCU to the host PC.﻿

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200005b4
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
