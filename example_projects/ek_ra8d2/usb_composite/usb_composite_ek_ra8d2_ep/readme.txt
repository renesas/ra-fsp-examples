/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USB Composite driver on Renesas RA MCUs based on Renesas
 	FSP using FreeRTOS. In this example, the application will configure the MCU as a CDC device as well as a Mass
	storage device. Tera Term is used to communicate with the CDC interface of the MCU. Data written by the
	Tera Term will be echoed back on the terminal by RA board. The user at the same time can read/write the data
	from/to the Mass storage device using host machine. When the host sets or changes the control line state, the
	on-board LED will be toggled.

2. Hardware Requirements:
	1 x Renesas RA board.
	2 x Type-C USB cables.
	1 x Host PC with at least 2 USB ports (1 for debug and 1 for COM port).

3. Hardware Connections:
	By default, the EP runs in Full-Speed mode, except for EK-RA6M3, EK-RA6M3G and EK-RA8M1 boards.
	To switch the USB speed, please refer to the "Configuration Settings" section for detailed instructions.

	For EK-RA6M1, EK-RA6M2 (Full-Speed):
		+ Connect micro-AB USB Full-Speed port (J9) of the RA board to USB port of the host PC
		  via a micro USB cable.

	For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		+ Jumper J12: Connect pins 2-3.
		+ Connect the jumper J15 pins.
		+ Connect the micro-AB USB Full-Speed port (J11) of the RA board to USB port of the host PC
		  via a micro USB cable.

	For MCK-RA8T1 (Full-Speed):
		+ Jumper JP9: Connect pins 2-3.
		+ Connect the jumper JP10 pins.
		+ Connect the Type-C USB Full-Speed port (CN14) of the RA board to USB port of the host PC
		  via a Type-C USB cable.

	For EK-RA4L1 (Full-Speed):
		+ Turn ON S4-4 to select USB device mode.
		+ Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		+ Connect the Type-C USB Full-Speed port (J11) of the RA board to USB port of the host PC
		  via a Type-C USB cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2 (Full-Speed):
		+ Connect the Type-C USB Full-Speed port (J11) of the RA board to USB port of the host PC
		  via a Type-C USB cable.

	For MCK-RA8T2 (Full-Speed):
		+ Jumper JP6: Connect pins 2-3.
		+ Connect the jumper JP7 pins.
		+ Connect the Type-C USB Full-Speed port (CN18) of the RA board to USB port of the host PC
		  via a Type-C USB cable.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
		+ Jumper J7: Connect pins 2-3.
		+ Connect the jumper J17 pins.
		+ Connect the micro-AB USB High-Speed port (J6) of the RA board to USB port of the host PC
		  via a micro USB cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		+ For EK-RA8D1, the user needs to turn OFF SW1-6 to use USB HS.
		+ Jumper J7: Connect pins 2-3.
		+ Connect the jumper J17 pins.
		+ Connect the micro-AB USB High-Speed port (J31) of the RA board to USB port of the host PC
		  via a micro USB cable.

	For EK-RA8P1, EK-RA8D2 (High-Speed):
		+ Connect the Type-C USB High-Speed port (J7) of the RA board to USB port of the host PC
		  via a Type-C USB cable.

4. Configuration Settings:
	To switch USB Speed in configuration:
	i. Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1,
			    EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8D2)
		Setting for r_usb_basic:
			DMA/DTC Source Address      : FS Address
			DMA/DTC Destination Address : FS Address
			USB Speed                   : Full Speed
			USB Module Number           : USB_IP0 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1) : 2 Bytes
			Activation Source (for g_transfer0)		     : USBFS FIFO 1 (DMA/DTC transfer request 1)
			Activation Source (for g_transfer1)		     : USBFS FIFO 0 (DMA/DTC transfer request 0)

	ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8D2)
		Setting for r_usb_basic:
			DMA/DTC Source Address	    : HS Address
			DMA/DTC Destination Address : HS Address
			USB Speed                   : Hi Speed
			USB Module Number           : USB_IP1 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1) : 4 Bytes
			Activation Source (for g_transfer0)		     : USBHS FIFO 1 (DMA transfer request 1)
			Activation Source (for g_transfer1)		     : USBHS FIFO 0 (DMA transfer request 0)

5. USB Disconnect Behaviour:
	The application also handles removing & connecting the peripheral USB cable use case.
	Ex: If the user removes the peripheral USB cable from the board, the Composite USB example project will be
	running and it waits for the cable to be connected. Once the cable is connected back, the example project works
	the same.

	Note:
	- If the user runs the example project on a PC with full permissions, they need to close Tera Term and eject the
	  USB disk before removing the peripheral USB cable.
	- PMSC Disk content is retained only when the Peripheral USB cable is removed & connected back.
	  If the user removes the power cable of board, the disk content will be lost as the disk is implemented on RAM.

6. Verifying Operation:
	- Import and build the EP.
	- Set up hardware connection properly.
	- Debug/Download the EP into the RA board.
	- Open the J-Link RTT Viewer to see the output.
	- Check device enumeration in "Device Manager".
	- Open the Tera Term:
		+ Select the correct COM port.
		+ To echo back what was typed in Tera Term, the user needs to enable it through:
			[Setup] -> [Terminal...] -> Check [Local echo].
		+ Set parameters of the serial port: Speed, Data, Parity, Stop bits through:
			[Setup] -> [Serial port...].
		+ Type input to execute write and read operations.

Note:
1) On some PCs, the USB Mass Storage Device may not be recognized due to restricted access permissions for
   external devices.

2) The usb_composite example project does not function correctly on PCs with CrowdStrike security
   software installed.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220007d0
   b. Keil:	Not Available
   c. IAR:	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
