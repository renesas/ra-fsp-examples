/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of USB Host Mass Storage Class (HMSC) driver with
	FreeRTOS+FAT file system using baremetal on Renesas RA MCUs based on Renesas FSP. FreeRTOS+FAT uses the
	underlying Block media driver which utilizes USB HMSC driver to perform file operations on the USB storage
	devices. Error and EP status messages will be printed on J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1,
	                     MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8M2
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Type-C USB OTG cable.
	1 x USB flash drive.

3. Configuration Settings:
	To select USB Speed in configuration:
	i.  Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1,
	                     EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8M2)
		Setting for r_usb_basic:
			USB Speed         : Full Speed
			USB Module Number : USB_IP0 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1) : 2 Bytes
			Activation Source (for g_transfer0)		     : USBFS FIFO 1 (DMA/DTC transfer request 1)
			Activation Source (for g_transfer1)		     : USBFS FIFO 0 (DMA/DTC transfer request 0)

	ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8M2)
		Setting for r_usb_basic:
			USB Speed         : Hi Speed
			USB Module Number : USB_IP1 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1) : 4 Bytes
			Activation Source (for g_transfer0)		     : USBHS FIFO 1 (DMA transfer request 1)
			Activation Source (for g_transfer1)		     : USBHS FIFO 0 (DMA transfer request 0)

4. Hardware Connections:
	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Jumper J7: Connect pins 1-2.
		Remove Jumper J17 pins.
		Connect the USB flash drive to the J6 connector using a micro USB OTG cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		Jumper J7: Connect pins 1-2.
		Remove Jumper J17 pins.
		Connect the USB flash drive to the J31 connector using a micro USB OTG cable.

	For EK-RA8P1, EK-RA8M2 (High-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect the USB flash drive to the J7 connector using a Type-C USB OTG cable.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1 (Full-Speed):
		Jumper J12: Connect pins 1-2.
		Remove Jumper J15 pins.
		Connect the USB flash drive to the J11 connector using a micro USB OTG cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9: Connect pins 1-2.
		Remove Jumper JP10 pins.
		Connect the USB flash drive to the CN14 connector using a Type-C USB OTG cable.

	For EK-RA4L1 (Full-Speed):
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		The user must turn OFF S4-4 to select USB Host Controller mode.
		Connect the USB flash drive to the J11 connector using a Type-C USB OTG cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8M2 (Full-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect the USB flash drive to the J11 connector using a Type-C USB OTG cable.

	For MCK-RA8T2 (Full-Speed):
		Jumper JP6: Connect pins 1-2.
		Remove Jumper JP7 pins.
		Connect the USB flash drive to the CN18 connector using a Type-C USB OTG cable.

Note:
1) By default, the USB HMSC example project runs in High-Speed mode on boards that support High-Speed and
   in Full-Speed mode for the other boards.

2) Format the USB flash drive (with FAT32 file system) before performing any operations.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22001910
   b. Keil:	Not Available
   c. IAR:	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
