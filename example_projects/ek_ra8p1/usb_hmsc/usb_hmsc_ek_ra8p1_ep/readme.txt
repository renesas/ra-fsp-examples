/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates basic functionalities of USB Host Mass Storage Class (HMSC) driver with the
	FreeRTOS+FAT file system on Renesas RA MCUs based on Renesas FSP. FreeRTOS+FAT uses the underlying block media
	driver which utilizes USB HMSC driver to perform file operations on the USB storage devices. Information and
	error messages will be printed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA4L1, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1,
	                     EK-RA8M1, EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2, MCK-RA8T1, MCK-RA8T2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x OTG cable for connection between the RA board and the USB flash drive (OTG cable type varies by
	    board model).
	1 x USB flash drive.

3. Configuration Settings:
	To switch USB speed in configuration:
	i. Full-Speed (Default): (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1,
	                              MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8D2, EK-RA8M2)
		Setting for r_usb_basic:
			USB Speed         : Full Speed
			USB Module Number : USB_IP0 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1) : 2 Bytes
			Activation Source (for g_transfer0)                  : USBFS FIFO 1 (DMA/DTC transfer request 1)
			Activation Source (for g_transfer1)                  : USBFS FIFO 0 (DMA/DTC transfer request 0)

	ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8D2, EK-RA8M2)
		Setting for r_usb_basic:
			USB Speed         : Hi Speed
			USB Module Number : USB_IP1 Port

		Setting for r_dmac:
			Transfer Size (for both g_transfer0 and g_transfer1) : 4 Bytes
			Activation Source (for g_transfer0)                  : USBHS FIFO 1 (DMA transfer request 1)
			Activation Source (for g_transfer1)                  : USBHS FIFO 0 (DMA transfer request 0)

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Set jumper J12 to pins 1-2.
		Remove the jumper from J15.
		Connect the USB flash drive to the J11 connector using the appropriate OTG cable.

	For MCK-RA8T1 (Full-Speed):
		Set jumper JP9 to pins 1-2.
		Remove the jumper from JP10.
		Connect the USB flash drive to the CN14 connector using the appropriate OTG cable.

	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB host mode.
		Set J17 jumper to pins 2-3, set J7 jumper to use P407 for USBFS VBUS.
		Connect the USB flash drive to the J11 connector using the appropriate OTG cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2 (Full-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect the USB flash drive to the J11 connector using the appropriate OTG cable.

	For MCK-RA8T2 (Full-Speed):
		Set jumper JP6 to pins 1-2.
		Remove the jumper from JP7.
		Connect the USB flash drive to the CN18 connector using the appropriate OTG cable.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Set jumper J7 to pins 1-2.
		Remove the jumper from J17.
		Connect the USB flash drive to the J6 connector using the appropriate OTG cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
		Set jumper J7 to pins 1-2.
		Remove the jumper from J17.
		Connect the USB flash drive to the J31 connector using the appropriate OTG cable.

	For EK-RA8P1, EK-RA8D2, EK-RA8M2 (High-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect the USB flash drive to the J7 connector using the appropriate OTG cable.

Note:
1) By default, the USB HMSC example project runs in Full-Speed mode.

2) Format the USB flash drive with the FAT32 file system before performing any operation.

3) The written data can also be manually verified in the ra_usb.txt file.

4) The user is expected to execute the "Safely eject" option before removing the USB flash drive;
   otherwise, further file operations may fail and data may be corrupted.

5) If the USB flash drive is removed without using the "Safely eject" option, reinsert the USB flash drive,
   execute the "Safely eject" option, and then execute the "Initialize FreeRTOS+FAT" option. This ensures that
   subsequent file operations work correctly after an unexpected removal.

6) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
	a. To make sure the received data shows properly, the user needs to set:
		[Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR].

	b. To echo back characters typed in Tera Term, the user needs to enable it through:
		[Setup] -> [Terminal...] -> Check [Local echo].

	c. The configuration parameters of the serial port on the serial terminal application:
		Port: The port is provided by the J-Link On-Board.
		Speed: 115200
		Data: 8 bit
		Parity: none
		Stop bits: 1 bit
		Flow control: none
