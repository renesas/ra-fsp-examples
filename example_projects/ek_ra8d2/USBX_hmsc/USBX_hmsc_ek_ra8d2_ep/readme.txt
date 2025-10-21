/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionalities of USBX Host Mass Storage Class (HMSC) driver
	with Azure RTOS + FileX file system on Renesas RA MCUs based on Renesas FSP. FileX interface on USBX uses the
	underlying USB HMSC driver to perform various file operations on the USB storage devices based on user input
	from the menu. Status of application will be displayed on J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M4, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1
	                     EK-RA8E2, EK-RA8P1, EK-RA8D2
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x USB Type-C OTG cable.
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

	For EK-RA8E2, EK-RA8P1, EK-RA8D2 (Full-Speed):
		Connect the USB flash drive to the J11 connector using a Type-C USB OTG cable.

	For EK-RA6M3 (High-Speed):
		Set jumper J7 to pins 1-2.
		Remove the jumper from J17.
		Connect the USB flash drive to the J6 connector using a micro USB OTG cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
		Set jumper J7 to pins 1-2.
		Remove the jumper from J17.
		Connect the USB flash drive to the J31 connector using a micro USB OTG cable.

	For EK-RA8P1, EK-RA8D2 (High-Speed):
		Connect the USB flash drive to the J7 connector using a Type-C USB OTG cable.

4. Configuration Settings:
	To switch USB Speed in configuration:
	i. Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1,
	                    MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, EK-RA8D2)
		Setting for r_usb_basic:
			DMA/DTC Source Address      : FS Address
			DMA/DTC Destination Address : FS Address
			USB Speed                   : Full Speed
			USB Module Number           : USB_IP0 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1) : 2 Bytes
			Activation Source (for g_transfer0)                  : USBFS FIFO 1 (DMA/DTC transfer request 1)
			Activation Source (for g_transfer1)                  : USBFS FIFO 0 (DMA/DTC transfer request 0)

	ii. High-Speed: (For EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8D2)
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
1) By default, USBX HMSC example project runs in High-Speed mode (If the RA board supports High-Speed mode).

2) The user is expected to execute the "Eject" option before removing USB, or else further file operations may fail
   and USB data may get corrupted.

3) Operation is not guaranteed for any user input value other than int or char types (e.g., float, special characters)
   through J-Link RTT Viewer input.

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000610
   b. Keil:	Not Available
   c. IAR:	Not Available

5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
