/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of USB Host Mass Storage Class (HMSC) driver
	with FreeRTOS+FAT file system on Renesas RA MCUs based on Renesas FSP. FreeRTOS+FAT uses the underlying
	Block media driver. The Block media driver utilizes USB HMSC driver to perform file operations on the
	USB storage devices. Error and info messages will be printed on J-Link RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Type-C USB OTG cable.
	1 x USB flash drive.

3. Hardware Connections:
	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Set jumper J12 to pins 1-2.
		Remove the jumper from J15.
		Connect the USB flash drive to the J11 connector using a micro USB OTG cable.

	For MCK-RA8T1 (Full-Speed):
		Set jumper JP9 to pins 1-2.
		Remove the jumper from JP10.
		Connect the USB flash drive to the CN14 connector using a Type-C USB OTG cable.

	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB host mode.
		Set J17 jumper to pins 2-3, set J7 jumper to use P407 for USBFS VBUS.
		Connect the USB flash drive to the J11 connector using a Type-C USB OTG cable.

	For EK-RA8E2, EK-RA8P1 (Full-Speed):
		Connect the USB flash drive to the J11 connector using a Type-C USB OTG cable.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Set jumper J7 to pins 1-2.
		Remove the jumper from J17.
		Connect the USB flash drive to the J6 connector using a micro USB OTG cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
		Set jumper J7 to pins 1-2.
		Remove the jumper from J17.
		Connect the USB flash drive to the J31 connector using a micro USB OTG cable.

	For EK-RA8P1 (High-Speed):
		Connect the USB flash drive to the J7 connector using a Type-C USB OTG cable.

4. Configuration Settings:
	To switch USB Speed in configuration:
	i. Full-Speed (Default): (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1,
				      MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1)
		Setting for r_usb_basic:
			DMA/DTC Source Address		: FS Address
			DMA/DTC Destination Address	: FS Address
			USB Speed                      	: Full Speed
			USB Module Number              	: USB_IP0 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1)	: 2 Bytes
			Activation Source (for g_transfer0)			: USBFS FIFO 1 (DMA transfer request 1)
			Activation Source (for g_transfer1)			: USBFS FIFO 0 (DMA transfer request 0)

	ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1)
		Setting for r_usb_basic:
			DMA/DTC Source Address		: HS Address
			DMA/DTC Destination Address	: HS Address
			USB Speed                      	: Hi Speed
			USB Module Number              	: USB_IP1 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1)	: 4 Bytes
			Activation Source (for g_transfer0)			: USBHS FIFO 1 (DMA transfer request 1)
			Activation Source (for g_transfer1)			: USBHS FIFO 0 (DMA transfer request 0)

Note:
1) By default USB HMSC example project runs in Full-Speed mode.

2) Format the USB drive (with FAT32 file system) before performing any operation.

3) Data written can also be manually verified in file ra_usb.txt.

4) The user is expected to execute "safely_eject" option before removing USB else further file operations may fail and
   USB data may get corrupted.

5) If USB is removed without using "safely_eject" option, the user is recommended to re-insert USB, perform "safely_eject"
   and "initialise USB" commands in sequence. This will make sure no USB operations fail after unexpected removal of USB.

6) The user is expected to enter data not exceeding 20 bytes in size.

7) Operation is not guaranteed for any user input value other than int, char (e.g., float, special character) through
   J-Link RTT Viewer input.
﻿
8) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000570
   b. Keil:     Not Available
   c. IAR:      Not Available
 
9) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
