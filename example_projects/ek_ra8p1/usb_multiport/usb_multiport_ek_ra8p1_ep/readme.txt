/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionality of USB Multiport on Renesas RA MCUs based on Renesas
	FSP using FreeRTOS. USB PCDC is configured for Full-Speed with a lower priority than USB HMSC, which is
	configured for High-Speed. The terminal is used for the input and status display. Tera Term is used to display
	the output and also to capture the data from it based on user input on the terminal. The project checks for
	the predefined file and reads the content and sends it to USB PCDC for display on Tera Term. Data captured
	from Tera Term (max 512 bytes) will be written to the file. The EP information and status will be displayed
	on the terminal.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8D2, EK-RA8M2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x USB cable for USB communication between the RA board and the host PC (USB cable type varies by
	    board model).
	1 x OTG cable for connection between the RA board and the USB flash drive (OTG cable type varies by
	    board model).
	1 x USB flash drive.
	1 x Host PC with at least 2 USB ports (1 for debug and 1 for COM port).

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	The user needs to connect both USBFS and USBHS to run EP.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Jumper J7: Connect pins 1-2.
		Remove the jumper J17 pins.
		Connect USB flash drive to USB High-Speed port (J6) of the RA board using the appropriate USB OTG cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		Note: For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
		Jumper J7: Connect pins 1-2.
		Remove the jumper J17 pins.
		Connect USB flash drive to USB High-Speed port (J31) of the RA board using the appropriate USB OTG cable.

	For EK-RA8P1, EK-RA8D2, EK-RA8M2 (High-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect USB flash drive to USB High-Speed port (J7) of the RA board using the appropriate USB OTG cable.

	For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Jumper J12: Connect pins 2-3.
		Connect the jumper J15 pins.
		Connect USB Full-Speed port (J11) of the RA board to USB port of the host PC
		using the appropriate USB cable.

	For EK-RA8P1, EK-RA8D2, EK-RA8M2 (Full-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect USB Full-Speed port (J11) of the RA board to USB port of the host PC
		using the appropriate USB cable.

Note:
1) The USB mass storage device must be formatted with the FAT file system.

2) The written data can also be manually verified in the file.

3) If the respective board is not powered through the USB debug port from the host PC, the functionality
   cannot be demonstrated.

4) Do not remove the USB flash drive while the example project is running. Restart the example project if
   the USB flash drive is disconnected.

5) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
