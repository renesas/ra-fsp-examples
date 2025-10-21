/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of the USBX Host Printer driver on Renesas RA MCUs based on
	Renesas FSP using Azure RTOS. An RA MCU board will be connected to the PC where the RA board will act as a host
	device and the printer will act as a peripheral device. A print command for a test page will be sent from the
	RA board, and printing data will be transferred from the host device to the printer. Printer information and
	error messages will be displayed on J-Link RTT Viewer.

	Following is the sequence used by the application:
	1. Connect two USB cables.
		a. One from the RA board (USB Debug port) to the PC for Debugging.
		b. One from the RA board (USB HS/FS port) to the printer.
	2. The printer information will be displayed on the J-Link RTT Viewer.
	3. On the host device, execute print command.
	4. MCU will send the printing data to the printer.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M5, EK-RA6M3, EK-RA6M4, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1,
	                     MCK-RA8T2, EK-RA8D2.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x OTG cable (For connecting printer).
	1 x Printer device (With USB interface).
	1 x Printer's USB cable used to connect the printer to the RA board.

3. Hardware Connections:
	The user needs to confirm the default USB speed selection used in the example project prior to set up the
	hardware connection.

	For EK-RA6M3 (High Speed):
		1. Jumper J7: Connect pins 1-2.
		2. Remove jumper J17 pins.
		3. Connect debug port of the RA MCU board to the Host PC using a micro USB cable.
		4. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed
		   port (J6) of the board. Connect the other end of this cable to USB port of the printer device.

	For EK-RA6M5 (High Speed):
		1. Jumper J7: Connect pins 1-2.
		2. Remove jumper J17 pins.
		3. Connect debug port of the RA MCU board to the Host PC using a micro USB cable.
		4. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed
		   port (J31) of the board. Connect the other end of this cable to USB port of the printer device.

	For EK-RA6M4, EK-RA8M1, EK-RA8D1 (Full Speed):
		1. Jumper J12: Connect pins 1-2.
		2. Remove jumper J15 pins.
		3. Connect debug port of the RA MCU board to the Host PC using a micro USB cable.
		4. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
		   port (J11) of the board. Connect the other end of this cable to USB port of the printer device.

	For MCK-RA8T1 (Full Speed):
		1. Jumper JP9: Connect pins 1-2.
		2. Remove jumper JP10 pins.
		3. Connect debug port of the RA MCU board to the Host PC using a Type-C USB cable.
		4. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
		   port (CN14) of the board. Connect the other end of this cable to USB port of the printer device.

	For EK-RA4L1 (Full Speed):
		1. Turn OFF S4-4 to use USB Host Controller.
		2. Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		3. Connect debug port of the RA MCU board to the Host PC using a Type-C USB cable.
		4. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
		   port (J11) of the board. Connect the other end of this cable to USB port of the printer device.
	
	For EK-RA8E2, EK-RA8D2 (Full Speed):
		1. Connect debug port of the RA MCU board to the Host PC using a Type-C USB cable.
		2. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
		   port (J11) of the board. Connect the other end of this cable to USB port of the printer device.

	For MCK-RA8T2 (Full Speed):
		1. Jumper JP6: Connect pins 1-2.
		2. Remove jumper JP7 pins.
		3. Connect debug port of the RA MCU board to the Host PC using a Type-C USB cable.
		4. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
		   port (CN18) of the board. Connect the other end of this cable to USB port of the printer device.

	For EK-RA8P1, EK-RA8D2 (High Speed):
		1. Connect debug port of the RA MCU board to the Host PC using a Type-C USB cable.
		2. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB High Speed
		   port (J7) of the board. Connect the other end of this cable to USB port of the printer device.

4. Configuration Settings:
	To switch USB Speed in configuration:
	i. Full-Speed: (For EK-RA6M4, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2, MCK-RA8T2, EK-RA8D2)
		Setting for r_usb_basic:
			DMA/DTC Source Address      : FS Address
			DMA/DTC Destination Address : FS Address
			USB Speed                   : Full Speed
			USB Module Number           : USB_IP0 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1) : 2 Bytes
			Activation Source (for g_transfer0)                  : USBFS FIFO 1 (DMA/DTC transfer request 1)
			Activation Source (for g_transfer1)-                 : USBFS FIFO 0 (DMA/DTC transfer request 0)

	ii. High-Speed: (For EK-RA6M3, EK-RA6M5, EK-RA8P1, EK-RA8D2)
		Setting for r_usb_basic:
			DMA/DTC Source Address      : HS Address
			DMA/DTC Destination Address : HS Address
			USB Speed                   : Hi Speed
			USB Module Number           : USB_IP1 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1) : 4 Bytes
			Activation Source (for g_transfer0)                  : USBHS FIFO 1 (DMA transfer request 1)
			Activation Source (for g_transfer1)                  : USBHS FIFO 0 (DMA transfer request 0)

Note:
1) For EK-RA8D2: By default, this example project runs USB in High-Speed mode.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22012210
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
