/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of USB Peripheral Vendor class driver on Renesas RA MCUs
	based on Renesas FSP. In this example, the application will configure the MCU as a Vendor Peripheral device.
	This Peripheral device will be connected to the USB Host Vendor device which is another RA board. After the
	enumeration is completed, the Vendor Peripheral board will read 15 Bytes of data from the Vendor Host board
	and write back the same data to the Vendor Host board. The Vendor Host and Vendor Peripheral applications are
	designed to continuously transfer data between both the boards. The user will be able to see all the operation
	sequence and status on J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2,
			     EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1,
			     EK-RA8E2, EK-RA8P1, EK-RA8D2
	2 x Renesas RA boards:
		- 1 x RA board runs the USB PVND example project.
		- 1 x RA board runs the USB HVND example project.
		1 x USB OTG cable.
		3 x Type-C USB cables.
		- 2 x Type-C USB cables for programming and debugging.
		- 1 x Type-C USB cable used to connect the RA board1 to the RA board2 through OTG cable.

3. Configuration Settings:
	To select USB Speed in configuration:
		i.  Full-Speed: (For EK-RA2A1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2,
				     EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1,
				     EK-RA8D2)
			USB Speed                     :    Full Speed
			USB Module Number             :    USB_IP0 Port

		ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA8D2)
			USB Speed                     :    Hi Speed
			USB Module Number             :    USB_IP1 Port

4. Hardware Connections:
	Connect Board1, running USB PVND example project, through OTG cable to Board2, running USB HVND example project.
	Connect USB debug ports of the two RA boards to USB ports of the host PC via two USB cables.

	For EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2 (Full-Speed):
		Connect micro-AB USB Full Speed port (J9) of the Board1 to Board2 via a micro USB cable
		through OTG cable.

	For EK-RA4E2, EK-RA6E2 (Full-Speed):
		Connect micro-AB USB Full Speed port (J11) of the Board1 to Board2 via a micro USB cable
		through OTG cable.

	For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Jumper J12 placement is pins 2-3.
		Connect jumper J15 pins.
		Connect micro-AB USB Full Speed port (J11) of the Board1 to Board2 via a micro USB cable
		through OTG cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9 placement is pins 2-3.
		Connect jumper JP10 pins.
		Connect Type-C USB Full Speed port (CN14) of the Board1 to Board2 via a Type-C USB cable
		through OTG cable.

	For EK-RA4L1 (Full-Speed):
		The user must turn ON S4-4 to select USB device mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect Type-C USB Full Speed port (J11) of the Board1 to Board2 via a Type-C USB cable
		through OTG cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2 (Full-Speed):
		Connect Type-C USB Full Speed port (J11) of the Board1 to Board2 via a Type-C USB cable
		through OTG cable.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Jumper J7 placement is pins 2-3.
		Connect jumper J17 pins.
		Connect micro-AB USB High Speed port (J6) of the Board1 to Board2 via a micro USB cable
		through OTG cable.

	For EK-RA8D2 (High-Speed):
		Connect Type-C USB High Speed port (J7) of the Board1 to Board2 via a Type-C USB cable
		through OTG cable.

Note:
1) By default, the USB PVND example project runs in Full-Speed mode.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220005b0
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
