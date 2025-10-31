/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of USB HHID driver with FreeRTOS on Renesas RA 
	MCUs based on Renesas FSP. USB HHID driver configures keyboard as a hhid device on pressing key from
	the keyboard, received keycode is decoded and displayed on the J-Link RTT Viewer.
	Error and info messages will be printed on J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3, EK-RA6M3G, EK-RA8M1, EK-RA8D1,
			     MCK-RA8T1, EK-RA4L1
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x OTG cable.
	1 x USB PC Keyboard.

3. Hardware Connections:
	For EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3, EK-RA6M3G, EK-RA8M1, EK-RA8D1 (Full-Speed):
        	Jumper J12 placement is pins 1-2.
        	Remove Jumper J15 pins.
        	Connect USB keyboard to J11 connector with help of OTG cable.

	For MCK-RA8T1 (Full-Speed):
        	Jumper JP9 placement is pins 1-2.
        	Remove Jumper JP10 pins.
        	Connect USB keyboard to CN14 connector with help of OTG cable.

	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB host mode. 
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
        	Connect USB keyboard to J11 connector with help of OTG cable.

Note: 
1) The user is requested to enter key from a to z, A to Z, 0 to 9 and any special character on keyboard.

2) The EP only supports the HP brand keyboard for testing (HP PN: 803181-001).

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001544
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
