/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of the USB PCDC driver on Renesas RA MCUs based on Renesas
	FSP using FreeRTOS. In this example, the user will be able to use the RA board as a CDC to UART converter by
	connecting through a COM port from the host PC. The converter performs the data reception and data transmission
	asynchronously. The user can connect any UART device to receive the data typed in Tera Term, and to send any
	data to the converter to display on Tera Term. The user can also change the baud rate, stop bit and parity bit
	configuration at run time through Tera Term. The status of the application will be displayed on
	J-Link RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	SEGGER J-Link RTT Viewer: Version 8.92
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application
	USB Serial Drivers

3. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA4M2, EK-RA6M3, EK-RA6M4, EK-RA8D1, EK-RA8M1, MCK-RA8T1, EK-RA4L1,
			     EK-RA8E2, MCK-RA8T2, EK-RA8M2, EK-RA8D2.
	1 x Renesas RA board.
	1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/) is used as
	    example external UART device to demonstrate the functionality of the converter.
	2 x Type-C USB cables.
	1 x Micro USB cable for connecting the Pmod USBUART to the host PC.
	1 x PC with at least 3 USB ports (1 for debug, 2 for COM port).
	Some jumper wires.

4. Configuration Settings:
	To select USB Speed in configuration:
		i. Full-Speed: (For EK-RA2A1, EK-RA4M2, EK-RA6M4, MCK-RA8T1, EK-RA8M1, EK-RA8D1, EK-RA4L1, EK-RA8E2, 
				    MCK-RA8T2, EK-RA8M2, EK-RA8D2)
		Setting for r_usb_basic:
			DMA/DTC Source Address		: FS Address
			DMA/DTC Destination Address	: FS Address
			USB Speed                      	: Full Speed
			USB Module Number              	: USB_IP0 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1)	: 2 Bytes
			Activation Source (for g_transfer0)			: USBFS FIFO 1 (DMA/DTC transfer request 1)
			Activation Source (for g_transfer1)			: USBFS FIFO 0 (DMA/DTC transfer request 0)

		ii. High-Speed: (For EK-RA6M3, EK-RA8M1, EK-RA8D1, EK-RA8M2, EK-RA8D2)
		Setting for r_usb_basic:
			DMA/DTC Source Address		: HS Address
			DMA/DTC Destination Address	: HS Address
			USB Speed                      	: Hi Speed
			USB Module Number              	: USB_IP1 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1)	: 4 Bytes
			Activation Source (for g_transfer0)			: USBHS FIFO 1 (DMA transfer request 1)
			Activation Source (for g_transfer1)			: USBHS FIFO 0 (DMA transfer request 0)

5. Hardware Connections:
	For EK-RA6M3:
		USB PCDC (High Speed):
			1. Jumper J7: Connect pins 2-3.
			2. Connect Jumper J17 pins.
			3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
			   port (J6) of the board. Connect the other end of this cable to USB port of the host PC.

		USB PCDC (Full Speed):
			1. Jumper J12: Connect pins 2-3.
			2. Connect Jumper J15 pins.
			3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
			   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

		UART:
			1. TXD0 P411 to RX pin of external UART device.
			2. RXD0 P410 to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RST0 P413 should be connected to either RTS (P103) or GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
			   For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For EK-RA4M2:
		USB PCDC (Full Speed):
			1. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
			   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.
			2. Jumper J12 placement is pins 2-3.
			3. Connect jumper J15 pins.

		UART:
			1. TXD9 P602 to RX pin of external UART device.
			2. RXD9 P601 to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RTS9 P603 should be connected to GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
			   For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For EK-RA6M4:
		USB PCDC (Full Speed):
			1. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
			   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.
			2. Jumper J12 placement is pins 2-3.
			3. Connect jumper J15 pins.

		UART:
			1. TXD0 P101 to RX pin of external UART device.
			2. RXD0 P100 to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS0 P414 should be connected to P413.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
			   For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For EK-RA2A1:
		USB PCDC (Full Speed):
			1. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
			   port (J9) of the board. Connect the other end of this cable to USB port of the host PC.

		UART:
			1. TXD0 P302 to RX pin of external UART device.
			2. RXD0 P301 to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RTS0 P303 should be connected to GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
			   For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For EK-RA8M1, EK-RA8D1:
		Note: For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
		USB PCDC (Full Speed):
			1. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
			   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.
			2. Jumper J12 placement is pins 2-3.
			3. Connect jumper J15 pins.

		USB PCDC (High Speed):
			1. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
			   port (J31) of the board. Connect the other end of this cable to USB port of the host PC.
			2. Jumper J7 placement is pins 2-3.
			3. Connect jumper J17 pins.

		UART:
			1. TXD2 PA03 to RX pin of external UART device.
			2. RXD2 PA02 to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RTS2 PA05 should be connected to GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
			   For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For MCK-RA8T1:
		USB PCDC (Full Speed):
			1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
			   port (CN14) of the board. Connect the other end of this cable to USB port of the host PC.
			2. Jumper JP9 placement is pins 2-3.
			3. Connect jumper JP10 pins.

		UART:
			1. TXD0 P603 (CN3:31) to RX pin of external UART device.
			2. RXD0 P602 (CN3:33) to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RTS0 P604 should be connected to GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
			   For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For EK-RA4L1:
		USB PCDC (Full Speed):
			1. The user must turn ON S4-4 to select USB device mode.
			2. Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
			3. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
			   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

		UART:
			1. TXD1 P115 (J2:35) to RX pin of external UART device.
			2. RXD1 P608 (J2:37) to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RTS1 P402 (J2:48) should be connected to GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
			   For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For EK-RA8E2:
		USB PCDC (Full Speed):
			1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
			   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

		UART:
			1. TXD1 P400 (J2:2) to RX pin of external UART device.
			2. RXD1 P401 (J2:3) to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RTS1 P403 (J2:5) should be connected to GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
			   For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For MCK-RA8T2:
		USB PCDC (Full Speed):
			1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
			   port (CN18) of the board. Connect the other end of this cable to USB port of the host PC.
			2. Jumper JP6 placement is pins 2-3.
			3. Connect jumper JP7 pins.

		UART:
			1. TXD0 P603 (CN1:33) to RX pin of external UART device.
			2. RXD0 P602 (CN1:31) to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RTS0 P604 (CN1:27) should be connected to GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
			   For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.

		USB PCDC (High Speed): Default USB speed mode
			1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
			   port (J7) of the board. Connect the other end of this cable to USB port of the host PC.

		USB PCDC (Full Speed):
			1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB High Speed 
			   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

		UART:
			1. TXD0 P603 (J37:3) to RX pin of external UART device.
			2. RXD0 P602 (J3:40) to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RTS0 P604 (J37:5) should be connected to GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
		           For instructions on how to enable and disable flow control, refer to the FSP User's manual.

	For EK-RA8D2:
		USB PCDC (High Speed):
			1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed
			   port (J7) of the board. Connect the other end of this cable to USB port of the host PC.

		USB PCDC (Full Speed):
			1. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB High Speed 
			   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

		UART:
			1. TXD0 P603 (J4:4) to RX pin of external UART device.
			2. RXD0 P602 (J4:8) to TX pin of external UART device.
			3. When using flow control from Tera Term, the CTS_RTS0 P604 (J4:6) should be connected to GND.
			4. Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
		           For instructions on how to enable and disable flow control, refer to the FSP User's manual.

Note:
1) The user can connect any other UART device to communicate with the USB PCDC to UART converter.

2) To enable testing of the converter, the user can also loop back the TX and RX pins on the RA board USB to UART converter.

3) By default, High-Speed USB is enabled in the project configuration for MCUs that support High-Speed operation.
   For MCUs that only support Full-Speed USB, the default configuration is set accordingly. To switch from High-Speed
   to Full-Speed USB, refer to the instructions in the usb_pcdc_with_freertos_notes.md

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220006f0
   b. Keil:	Not Available
   c. IAR:	Not Available

5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
