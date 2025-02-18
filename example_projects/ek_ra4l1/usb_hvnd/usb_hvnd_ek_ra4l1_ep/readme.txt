/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
   	This project demonstrates the basic functionalities of USB Host Vendor class driver on Renesas RA MCUs based on Renesas FSP.
    	In this example, the application will configure the MCU as a Vendor Host device. This Host device will be connected to the 
    	USB Peripheral Vendor device which is another RA board. After the enumeration is completed, the Vendor Host board will write
    	15 Bytes of data to the Vendor Peripheral board and read back the same data from the Vendor Peripheral board. The Vendor Host
    	and Vendor Peripheral applications are designed to continuously transfer data between both the boards. The user will be able
    	to see all the operation sequence and status on J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1, EK-RA4L1
	2 x Renesas RA boards:
		- 1 x RA board runs the USB HVND Example Project.
		- 1 x RA board runs the USB PVND Example Project.
	1 x USB OTG cable.
        3 x USB cables for programming and debugging.

3. Configuration Settings:
	To select USB Speed in configuration:
    	i.  Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1, EK-RA4L1)
            USB Speed                     :    Full Speed
            USB Module Number             :    USB_IP0 Port

    	ii. High-Speed: (For EK-RA6M3, EK-RA6M3G)
            USB Speed                     :    Hi Speed
            USB Module Number             :    USB_IP1 Port

4. Hardware Connections:
	Connect Board 1, running USB HVND Example Project, through OTG cable to Board 2, running USB PVND Example Project.

	For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1 (Full Speed):
		Jumper J12 placement is pins 1-2.
		Remove jumper J15 pins.
		Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J11) of the board.
		Connect the other end of this cable through OTG Cable to Board 2.

	For EK-RA6M3, EK-RA6M3G (High Speed):
        	Jumper J7 placement is pins 1-2.
		Remove jumper J17 pins.
		Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J6) of the board.
		Connect the other end of this cable through OTG Cable to Board 2.

	For MCK-RA8T1 (Full Speed):
		Jumper JP9 placement is pins 1-2.
		Remove jumper JP10 pins.
		Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed port (CN14) of the board.
		Connect the other end of this cable through OTG Cable to Board 2.

	For EK-RA4L1 (Full Speed):
		The user must turn OFF S4-4 to select USB Host Controller mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed port (J11) of the board.
		Connect the other end of this cable through OTG Cable to Board 2.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008cc
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
