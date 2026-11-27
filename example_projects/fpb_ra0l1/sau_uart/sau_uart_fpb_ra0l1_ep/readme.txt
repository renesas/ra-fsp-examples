/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the SAU UART HAL module APIs. The project initializes the
	SAU UART module with 115200 bps baud rate, 8-bits data length, no parity check, and one-bit stop. After opening
	and configuring the serial terminal application, the user can enter any ASCII character string with a length
	less than 256 characters into the terminal, then press enter-key to send it to the RA board. The RA Board will
	echo any data received back to the terminal on the host PC.

	The Pmod USBUART facilitates the conversion of UART signals to USB signals and vice versa, enabling
	communication between the RA board and the serial terminal application.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x USB to UART Converter (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
	1 x Micro USB cable used to connect the Pmod USBUART to the host PC.
	For FPB-RA0E2, FPB-RA0E3: Some jumper wires used to connect the Pmod USBUART to the RA board.

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	Connect Pmod USBUART to the RA board and the host PC as shown below:
	For FPB-RA0E1:
		- Connect the Pmod USBUART (J2) to the PMOD1 connector on the RA board. Please carefully align pin 1
		  on the Pmod USBUART with pin 1 on the PMOD1.
		- Connect the Pmod USBUART (J1) to the host PC using a micro USB cable.

	For FPB-RA0E2:
		- Connect TXD2 P402 (J1:3) to RXD (J2:2) of Pmod USBUART.
		- Connect RXD2 P403 (J1:4) to TXD (J2:3) of Pmod USBUART.
		- Connect GND (J1:8) to GND (J2:5) of Pmod USBUART.
		- Connect the Pmod USBUART (J1) to the host PC using a micro USB cable.

	For FPB-RA0L1:
		- Connect the Pmod USBUART (J2) to the PMOD1 (J19) connector on the RA board. Please carefully align
		  pin 1 on the Pmod USBUART with pin 1 on the PMOD1.
		- Connect the Pmod USBUART (J1) to the host PC using a micro USB cable.
		- The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

	For FPB-RA0E3:
		- The user must turn OFF S3 to use the on-board debug functionality.
		- Connect TXD1 P213 (J1:4) to RXD (J2:2) of Pmod USBUART.
		- Connect RXD1 P212 (J1:5) to TXD (J2:3) of Pmod USBUART.
		- Connect GND (J1:3) to GND (J2:5) of Pmod USBUART.
		- Connect the Pmod USBUART (J1) to the host PC using a micro USB cable.

Note:
1) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
	a. To make sure the received data shows properly, the user needs to set:
		[Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR].

	b. To echo back characters typed in Tera Term, the user needs to enable it through:
		[Setup] -> [Terminal...] -> Check [Local echo].

	c. The configuration parameters of the serial port on the serial terminal application:
		Port: Virtual COM Port (Pmod USBUART).
		Speed: 115200
		Data: 8 bit
		Parity: none
		Stop bits: 1 bit
		Flow control: none
