/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates basic functionality of USBX Peripheral Human Interface Device Class module on Renesas
	RA MCUs based on Renesas FSP using Azure RTOS. The user will be able to use RA board as a keyboard (HID Device)
	by connecting it to the Host PC. Once the board is connected, the user can provide any input from RTT Viewer
	to send character from a - z & numbers 0 - 9 from RA board to host PC. USB status and any error messages are 
	displayed on RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4E2, EK-RA6E2,
			     EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1
	1 x Renesas RA board.
	2 x Type-C USB cables.
	1 x PC with at least 2 USB ports.
	
3. Hardware Connections:
	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:	
		- Jumper J12: Connect pins 2-3.
		- Connect Jumper J15 pins.
		- Connect the RA board USB FS port (J11) to the PC via a micro USB cable.
		- Connect the RA board USB debug port (J10) to the PC via a micro USB cable for EP debugging.

	For EK-RA6M1, EK-RA6M2:
		- Connect the RA board USB FS port (J11) to the PC via a micro USB cable.
		- Connect the RA board USB debug port (J9) to the PC via a micro USB cable for EP debugging.

	For EK-RA4E2, EK-RA6E2:
		- Connect the RA board USB FS port (J11) to the PC via a micro USB cable.
		- Connect the RA board USB debug port (J10) to the PC via a micro USB cable for EP debugging.

	For MCK-RA8T1:
		- Jumper JP9: Connect pins 2-3.
		- Connect Jumper JP10 pins.
		- Connect the RA board USB FS port (CN14) to the PC via a Type-C USB cable.
		- Connect the RA board USB debug port (CN11) to the PC via a Type-C USB cable for EP debugging.
	
	For EK-RA4L1:
		- Turn ON S4-4 to select USB device mode.
		- Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		- Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.
		- Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

	For EK-RA8E2, EK-RA8P1:
		- Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.
		- Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

Note:
1) Known limitation: the USBX_phid USB Full-Speed configuration example project does not work on PCs with Crowdstrike
   software installed.

2) Open "Device Manager", and check "Human Interface Devices", to see "USB Input Device" has been detected.

3) Lower case a - z and numbers 0 - 9 are sent from MCU to the host PC.

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000570
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
