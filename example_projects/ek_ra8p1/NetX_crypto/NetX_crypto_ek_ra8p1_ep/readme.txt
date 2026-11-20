/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionality of NetX Secure Crypto on Renesas RA MCUs based on
	Renesas FSP supporting SCE/RSIP hardware acceleration using AzureRTOS. The example project executes AES, ECC,
	and RSA algorithms (including different chaining modes) in a multi-threaded environment using mutex to enable
	access to SCE/RSIP. The user can enter any key and press Enter to start continuous concurrent execution of the
	algorithms. The algorithms will continue running and display execution status on the terminal until the
	board is reset.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards:
		SCE7      - EK-RA6M2.
		SCE9      - EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5.
		RSIP-E51A - EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA8E2.
		RSIP-E11A - EK-RA4L1.
		RSIP-E50D - EK-RA8P1, MCK-RA8T2, EK-RA8M2, EK-RA8D2.
		RSIP-E31A - EK-RA4C1.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Host PC.

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29
		on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.

Note:
1) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
