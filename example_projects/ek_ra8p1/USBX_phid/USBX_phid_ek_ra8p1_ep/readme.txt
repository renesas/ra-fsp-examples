/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionality of the USBX Peripheral Human Interface Device (HID)
	module running on Renesas RA MCUs based on Renesas FSP using Azure RTOS. The RA board acts as an HID device
	and is configured as a keyboard. When any key is pressed in the terminal, the RA board sends characters
	'a' to 'z' followed by '1' to '9' and '0' to the host PC. USB status and error messages are displayed on
	the terminal.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4E2, EK-RA6E2,
	                     EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8D2,
	                     EK-RA8M2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x USB cable for USB communication between the RA board and the host PC (USB cable type varies by board model).
	1 x Host PC with at least 2 available USB ports (1 for debugging and 1 for the COM port).
	
4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming
	and debugging.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		1. Jumper J12: Connect pins 2-3.
		2. Connect jumper J15 pins.
		3. Connect the USB Full-Speed port (J11) of the RA board to the host PC using the appropriate USB cable.

	For EK-RA6M1, EK-RA6M2, EK-RA4E2, EK-RA6E2 (Full-Speed):
		1. Connect the USB Full-Speed port (J11) of the RA board to the host PC using the appropriate USB cable.

	For MCK-RA8T1 (Full-Speed):
		1. Jumper JP9: Connect pins 2-3.
		2. Connect jumper JP10 pins.
		3. Connect the USB Full-Speed port (CN14) of the RA board to the host PC using the appropriate USB cable.

	For EK-RA4L1 (Full-Speed):
		1. Turn ON S4-4 to select USB device mode.
		2. Set J17 jumper to pins 2-3 and set J7 jumper to use P407 for USBFS VBUS.
		3. Connect the USB Full-Speed port (J11) of the RA board to the host PC using the appropriate USB cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2 (Full-Speed):
		Note: For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		1. Connect the USB Full-Speed port (J11) of the RA board to the host PC using the appropriate USB cable.

	For MCK-RA8T2 (Full-Speed):
		1. Jumper JP6: Connect pins 2-3.
		2. Connect jumper JP7 pins.
		3. Connect the USB Full-Speed port (CN18) of the RA board to the host PC using the appropriate USB cable.

Note:
1) Known limitation: The USBX PHID USB Full-Speed example project does not work on PCs with CrowdStrike
   software installed.

2) Open "Device Manager" and check "Keyboards" to verify that the device is detected.

3) Characters 'a' to 'z', followed by '1' to '9' and '0', are sent from the RA board to the host PC.
   Open Notepad within 3 seconds after pressing a key in the terminal to observe the received keyboard input.

4) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
