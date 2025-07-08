/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the basic functionality of USB Host Communication Class (HCDC) driver on Renesas 
	RA MCUs using 2 RA boards. Board 1 (with USB HCDC Example Project running on it) communicates with Board 2 (with USB
	PCDC Example Project running). Board 1 initiates the communication by sending commands to Board 2 and Board 2 responds
	by sending the data. Board 1 prints the received data on the RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1 and
	                     EK-RA4L1.
	2 x Renesas RA boards.
		- 1 x RA board runs the USB PCDC Example Project.
		- 1 x RA board runs the USB HCDC Example Project.
	2 x Type-C USB cables.
	1 x OTG cable: Type-C USB cable.

3. Hardware Connections:
	Connect Board 1, running USB HCDC Example Project, through OTG cable to Board 2, running USB PCDC Example Project.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Jumper J12: Connect pins 1-2.
		Remove jumper J15 pins.
		Connect the USB device to the J11 connector using a micro USB Host cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9: Connect pins 1-2.
		Remove jumper JP10 pins.
		Connect the USB device to the CN14 connector using a Type-C USB Host cable.

	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB host mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect the USB device to the J11 connector using a Type-C USB host cable.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Jumper J7: Connect pins 1-2.
		Remove jumper J17 pins.
		Connect the USB device to the J6 connector using a micro USB Host cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
		Jumper J7: Connect pins 1-2.
		Remove jumper J17 pins.
		Connect the USB device to the J31 connector using a micro USB Host cable.

4. Configuration Settings: 
	To select USB Speed in configuration:
	i. Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1,
 		 	    EK-RA4L1 boards)
		USB Speed                      :    Full Speed
		USB Module Number              :    USB_IP0 Port

	ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1 boards)
		USB Speed                      :    Hi Speed
		USB Module Number              :    USB_IP1 Port

Note: 	
1) By default USB HCDC Example Project (i.e., Board 1) runs in Full-speed mode.

2) RTT Print Buffer size should be configured to 512.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000d64
   b. Keil:  	Not Available 
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
