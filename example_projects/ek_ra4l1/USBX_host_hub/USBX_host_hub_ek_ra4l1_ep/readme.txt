/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of USBX Host Hub driver with AZURE RTOS on Renesas RA MCUs based
    	on Renesas FSP. The USB Host Hub functionality is demonstration is done by using HHID devices. USBX HHID driver enumerates 
    	Human Interface Devices limited to keyboard or mouse using USBX middleware. Upon connecting a keyboard and mouse to the hub, 
    	pressing a key from the keyboard, the received keycode is displayed on the J-Link RTT Viewer. When the mouse is moved, the 
    	x and y axis position will be printed on J-Link RTT Viewer. Error and info messages will be printed on J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x OTG cable.
	1 x USB Hub.
	1 x USB PC Keyboard.
	1 x USB Mouse.

3. Hardware Connections:
    	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
        	Jumper J12 placement is pins 1-2.
        	Remove Jumper J15 pins.
        	Connect the USB Hub to J11 connector with OTG cable.
        	Connect mouse and keyboard to the Hub.
    	
	For MCK-RA8T1 (Full-Speed):
        	Jumper JP9 placement is pins 1-2
        	Remove Jumper JP10 pins
        	Connect the USB Hub to CN14 connector with OTG cable.
        	Connect mouse and keyboard to the Hub.

    	For EK-RA4L1 (Full-Speed):
        	Turn OFF S4-4 to use USB Host Controller.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
        	Connect the USB Hub to J11 connector with OTG cable.
        	Connect mouse and keyboard to the Hub.

Note: 
1) EP does not support high speed as USBX HHID does not support it.

2) EP does not support HP brand keyboard for testing (HP PN: 803181-001).

3) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200006dc
   b. Keil:     Not Available 
   c. IAR:      Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).