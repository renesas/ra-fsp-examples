/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionalities of USB HHID driver with FreeRTOS on Renesas RA 
	MCUs based on Renesas FSP. The USB HHID driver configures keyboard as a HHID device. On pressing a key from
	the keyboard, the received keycode is decoded and displayed on the J-Link RTT Viewer. Error and info messages
	will also be printed on the J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3, EK-RA6M3G, EK-RA8M1, EK-RA8D1,
	                     MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x OTG cable.
	1 x Wired USB keyboard (e.g., HP PN: 803181-001).

3. Hardware Connections:
	For EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3, EK-RA6M3G, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Jumper J12 placement is pins 1-2.
		Remove jumper J15 pins.
		Connect a Wired USB keyboard to J11 connector via an OTG cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9 placement is pins 1-2.
		Remove jumper JP10 pins.
		Connect a Wired USB keyboard to CN14 connector via an OTG cable.

	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB host mode. 
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect a Wired USB keyboard to J11 connector via an OTG cable.

	For EK-RA8E2, EK-RA8P1 (Full-Speed):
		Connect a Wired USB keyboard to J11 connector via an OTG cable.

	For MCK-RA8T2 (Full-Speed):
		Jumper JP6 placement is pins 1-2.
		Remove jumper JP7 pins.
		Connect a Wired USB keyboard to CN18 connector via an OTG cable.

Note:
1) The user is requested to enter a key from a–z, A–Z, 0–9, and special characters (e.g., !, @, #, $, %, ^, &, *, (, ))
   on keyboard.

2) The EP has been confirmed to work properly with the HP brand keyboard (HP PN: 803181-001), while some other keyboard
   models may not be compatible.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000d2c
   b. Keil:     Not Available
   c. IAR:      Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
