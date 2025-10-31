/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionality of USBX Host CDC ACM module on Renesas RA MCUs using
	2 RA boards. Board 1 acts as Host device running USBX HCDC-ACM example project and Board 2 acts as Peripheral
	device running USBX PCDC-ACM example project and connected with Type-C USB cable via OTG. The peripheral
	initiates the communication and once the host reads the data initiated by the peripheral. The host will then
	send predefined data to the peripheral and the device will echo the data back to the host. The application
	status messages will be displayed on the J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1,
	                     EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8M2
	2 x Renesas RA boards:
		- 1 x RA board runs the USBX HCDC ACM Example Project.
		- 1 x RA board runs the USBX PCDC ACM Example Project.
	1 x OTG cable: Type-C to USB Host (Female).
	3 x USB cables.
		- 2 x USB cables for programming and debugging.
		- 1 x Type-C USB cable used to connect the RA board 1 to the RA board 2 through OTG cable.

3. Configuration Settings:
	To select USB Speed in configuration:
	i. Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1,
	                    EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8M2)
	Setting for r_usb_basic:
		DMA/DTC Source Address	    : FS Address
		DMA/DTC Destination Address : FS Address
		USB Speed                   : Full Speed
		USB Module Number           : USB_IP0 Port

	Setting for r_dmac:
		Transfer Size (for both g_transfer0 and g_transfer1) : 2 Bytes
		Activation Source (for g_transfer0)		     : USBFS FIFO 1 (DMA/DTC transfer request 1)
		Activation Source (for g_transfer1)		     : USBFS FIFO 0 (DMA/DTC transfer request 0)

	i. High-Speed: (For EK-RA6M3, EK-RA8M2)
	Setting for r_usb_basic:
		DMA/DTC Source Address	    : HS Address
		DMA/DTC Destination Address : HS Address
		USB Speed                   : Hi Speed
		USB Module Number           : USB_IP1 Port

	Setting for r_dmac:
		Transfer Size (for both g_transfer0 and g_transfer1) : 4 Bytes
		Activation Source (for g_transfer0)		     : USBHS FIFO 1 (DMA transfer request 1)
		Activation Source (for g_transfer1)		     : USBHS FIFO 0 (DMA transfer request 0)

4. Hardware Connections:
	Connect board 1, running the USBX HCDC ACM example project, through OTG cable to board 2, running the
	USBX PCDC ACM example project.
	Connect USB debug ports of the two RA boards to USB ports of the host PC via two USB cables.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full Speed):
		Jumper J12 placement is pins 1-2.
		Remove Jumper J15 pins.
		Connect USB device to J11 connector via a micro USB Host cable.

	For MCK-RA8T1 (Full Speed):
		Jumper JP9 placement is pins 1-2.
		Remove Jumper J10 pins.
		Connect USB device to CN14 connector via a Type-C USB Host cable.

	For EK-RA4L1 (Full Speed):
		The user must turn OFF S4-4 to select USB Host Controller mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect USB device to J11 connector via a Type-C USB Host cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8M2 (Full Speed):
		Note: For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect USB device to J11 connector via a Type-C USB Host cable.

	For MCK-RA8T2 (Full Speed):
		Jumper JP6 placement is pins 1-2.
		Remove Jumper JP7 pins.
		Connect USB device to CN18 connector via a Type-C USB Host cable.

       	For EK-RA6M3 (High Speed):
		Jumper J7: Connect pins 1-2.
		Remove Jumper J17 pins.
		Connect USB device to J6 connector via a micro USB Host cable.

       	For EK-RA8M2 (High Speed):
		Note: For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect USB device to J7 connector via a Type-C USB Host cable.

5. Verifying Operation:
	1. Import, generate and build the USBX PCDC ACM and USBX HCDC ACM EPs.
	2. Now, flash USBX PCDC ACM code on RA board (e.g., board 2) which will act as peripheral device.
	3. Open J-Link RTT Viewer for board 2.
	4. Next, flash USBX HCDC ACM code on another RA board (e.g., board 1) which will act as host device.
	5. Open J-Link RTT Viewer for board 1 and verify host device operation.

	Link to Errata: https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

Note:
1) Both USBX HCDC ACM and USBX PCDC ACM need to be running at High Speed or Full Speed for the EP functionality
   to work correctly.

2) By default, the USBX HCDC ACM example project runs in Full-Speed mode.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x22000590
   b. Keil:     Not Available
   c. IAR:      Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
