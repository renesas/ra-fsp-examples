/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionality of USB Host Communication Class (HCDC) driver on
	Renesas RA MCUs using 2 RA boards. Board 1 (with USB HCDC example project running on it) communicates with
	Board 2 (with USB PCDC example project running). Board 1 initiates the communication by sending commands to
	Board 2 and Board 2 responds by sending the data. Board 1 prints the received data on the RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1,
	                     MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1.
	2 x Renesas RA boards.
		- 1 x RA board runs the USB PCDC example project.
		- 1 x RA board runs the USB HCDC example project.
	2 x Type-C USB cables.
	1 x Type-C OTG cable.

3. Hardware Connections:
	Connect USB debug ports of the two RA boards to USB ports of the host PC via two USB cables.
	Connect Board 1, running USB HCDC example project, through OTG cable to Board 2, running USB PCDC example project.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Jumper J12: Connect pins 1-2.
		Remove jumper J15 pins.
		Connect micro-AB USB Full Speed port (J11) of the board to the Board 2 through OTG cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9: Connect pins 1-2.
		Remove jumper JP10 pins.
		Connect Type-C USB Full Speed port (CN14) of the board to the Board 2 through OTG cable.

	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB host mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect Type-C USB Full Speed port (J11) of the board to the Board 2 through OTG cable.

	For EK-RA8E2, EK-RA8P1 (Full-Speed):
		Connect Type-C USB Full Speed port (J11) of the board to the Board 2 through OTG cable.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Jumper J7: Connect pins 1-2.
		Remove jumper J17 pins.
		Connect micro-AB USB High Speed port (J6) of the board to the Board 2 through OTG cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
		Jumper J7: Connect pins 1-2.
		Remove jumper J17 pins.
		Connect micro-AB USB High Speed port (J31) of the board to the Board 2 through OTG cable.

	For EK-RA8P1 (High-Speed):
		Connect Type-C USB High Speed port (J7) of the board to the Board 2 through OTG cable.

4. Configuration Settings: 
	To select USB Speed in configuration:
	i. Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1,
 		 	    EK-RA4L1, EK-RA8E2, EK-RA8P1)
		USB Speed                      :    Full Speed
		USB Module Number              :    USB_IP0 Port

	ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1)
		USB Speed                      :    Hi Speed
		USB Module Number              :    USB_IP1 Port

Note:
1) By default, the USB HCDC example project (i.e., Board 1) runs in Full-Speed mode.
	
2) Both USB HCDC and USB PCDC need to be running at Full-Speed or High-Speed for the EP functionality to work correctly.

3) SEGGER_RTT_PRINTF_BUFFER_SIZE in file SEGGER_RTT_Conf.h should be configured to 512.

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000590
   b. Keil:  	Not Available
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
