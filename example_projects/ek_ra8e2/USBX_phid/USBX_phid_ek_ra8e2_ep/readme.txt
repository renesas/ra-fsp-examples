/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates basic functionality of USBX Peripheral Human Interface Device Class module on Renesas RA MCUs 
	based on Renesas FSP using Azure RTOS. The user will be able to use RA board as a keyboard (HID Device) by connecting it to 
	the Host PC. Once the board is connected, the user can provide any input from RTT Viewer to send character 
	from a - z & numbers 0 - 9 from RA board to host PC.
	USB status and any error messages are displayed on RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4M3, EK-RA4M2, EK-RA8M1, 
			     EK-RA8D1, EK-RA4E2, EK-RA6E2, MCK-RA8T1, EK-RA4L1, EK-RA8E2.
	1 x Renesas RA board.
	2 x Type-C USB cables for programming and debugging.
	1 x PC with at least 2 USB ports.
	
3. Hardware Connections:
	For EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2:
		Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J9) of the board.
		Connect the other end of this cable to USB port of the host PC. 
	
	For EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4M3, EK-RA4M2, EK-RA8M1, EK-RA8D1:
		Jumper J12 placement is pins 2-3
		Connect jumper J15 pins 
		Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J11) of the board.
		Connect the other end of this cable to USB port of the host PC. 

	For EK-RA4E2, EK-RA6E2:
		Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J11) of the board.  
		Connect the other end of this cable to USB port of the host PC.

	For MCK-RA8T1:
		Jumper JP9 placement is pins 2-3.
	 	Connect jumper JP10 pins.
		Connect the Type-C USB end of the Type-C USB cable to Type-C USB Full Speed port (CN14) of the board.  
		Connect the other end of this cable to USB port of the host PC.	
	
	For EK-RA4L1:
		Turn ON S4-4 to select USB device mode. 
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect Type-C USB end of the Type-C USB cable to Type-C USB Full Speed port (J11) of the board.  
		Connect the other end of this cable to USB port of the host PC.

	For EK-RA8E2:
		Connect Type-C USB end of the Type-C USB cable to Type-C USB Full Speed port (J11) of the board.  
		Connect the other end of this cable to USB port of the host PC.

Note:
1) Known limitation: the USBX_phid USB full speed configuration example project does not work on PCs with Crowdstrike
   software installed.

2) Open Device Manager, and check 'Human Interface Devices', to see 'USB Input Device' has been detected.

3) Lower case a - z and numbers 0 - 9 are sent from MCU to the host PC.

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22040160
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
