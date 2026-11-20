/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionalities of a USBX Device Firmware Update (DFU) driver on
	Renesas RA MCUs based on Renesas FSP using Azure RTOS. The RA board is connected to the host PC, where it
	acts as a peripheral device. Prepare a file (.txt, .srec, .xml, .hex, etc.) to download to the RA MCU which
	supports both CDC and DFU interfaces. In a real-world application, new device firmware can be downloaded.
	In this example project, a set of data is used to demonstrate the download functionality. The DFU example
	application works in four steps:
		a. First, the system is initialized with support for the Interface Association Descriptor. The CDC
		   and DFU classes are then initialized with separate interfaces.
		b. Second, the device operates in Run-Time mode with the CDC and DFU interfaces. An application on
		   the host (Windows PC) sends a DFU request. Upon reception of this command, the system un-initializes
		   the Run-Time configuration and initializes the device in DFU mode.
		c. Third, the device disappears from the bus for a short time (enough for the host and the device to
		   detect a reset sequence). Upon restarting, the device is exclusively in DFU mode and downloads the
		   data sent from the host.
		d. Finally, when the new data has been downloaded, the host application resets the device, and upon
		   re-enumeration, the device reverts to its normal operation with the new firmware. The RA MCU
		   displays the USB enumeration status on the terminal.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application
	Zadig.exe - v2.0.1.160 (https://sourceforge.net/projects/libwdi/files/zadig/)
	dfu-util-0.11-binaries.tar.xz (https://sourceforge.net/projects/dfu-util/files/)

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA6M1, EK-RA6M3, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1, EK-RA4L1, EK-RA8E2,
	                     EK-RA8P1, MCK-RA8T2, EK-RA8D2, EK-RA8M2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x USB cable for USB communication between the RA board and the host PC (USB cable type varies by board model).
	1 x Host PC with at least 2 USB ports (1 for debug, 1 for COM port).

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming
	and debugging.

	For EK-RA4M2, EK-RA6M3, EK-RA6M5, EK-RA8D1, EK-RA8M1 (Full-Speed):
		Jumper J12: Connect pins 2-3.
		Connect jumper J15 pins.
		Connect USB Full-Speed port (J11) of the RA board to USB port of the host PC using the
		appropriate USB cable.

	For EK-RA6M1 (Full-Speed):
		Connect USB Full-Speed port (J9) of the RA board to USB port of the host PC using the
		appropriate USB cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9: Connect pins 2-3.
		Connect jumper JP10 pins.
		Connect USB Full-Speed port (CN14) of the RA board to USB port of the host PC using the
		appropriate USB cable.

	For EK-RA4L1 (Full-Speed):
		The user must turn ON S4-4 to select USB device mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect USB Full-Speed port (J11) of the RA board to USB port of the host PC using the
		appropriate USB cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2 (Full-Speed):
		Note: For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		      and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect USB Full-Speed port (J11) of the RA board to USB port of the host PC using the
		appropriate USB cable.

	For MCK-RA8T2 (Full-Speed):
		Jumper JP6: Connect pins 2-3.
		Connect Jumper JP7 pins.
		Connect USB Full-Speed port (CN18) of the RA board to USB port of the host PC using the
		appropriate USB cable.

Note:
1) This EP supports only USB Full-Speed mode.

2) Prepare a file (e.g., .txt, .hex, .xml, .srec etc.) to download to the MCU. The file size must be smaller than
   30 KB by default. The example test file data.txt (29 KB) is included in the EP folder. To download a larger file,
   update the BUFFER_SIZE macro in usbx_dfu_ep.h according to the available memory and the selected verification scheme.
   For EK-RA4L1, the file size must be smaller than 4 KB due to the limited RAM size of this board. The example
   test file data.txt (4 KB) is included in the EP folder.

3) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
