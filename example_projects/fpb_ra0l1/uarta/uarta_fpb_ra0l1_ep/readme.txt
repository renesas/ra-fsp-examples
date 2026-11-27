/***********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
***********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionality of the UARTA driver on Renesas RA MCUs
	based on FSP, the Virtual COM is used as example usage of the UARTA. The user can select UARTA
	baud rate from the menu on the terminal (e.g., 9600 bps, 14400 bps, 19200 bps,...). After selecting
	the baud rate, the user needs to reconfigure parameters for the terminal application. Then the user
	can enter and send an ASCII string (up to 256 characters) from the terminal to the RA board, which will
	also be echoed back to the terminal by the RA board.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: FPB-RA0E1, FPB-RA0E2, EK-RA4C1, FPB-RA0L1.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

	For RA boards do not support J-Link On Board VCOM functionality on UARTA: EK-RA4C1.
		1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/)
		module for converting data from the USB interface to the UART interface and vice versa.
		1 x Micro USB cable for connecting the Pmod USBUART to the host PC.
		Some jumper wires.

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For FPB-RA0E1:
		To use J-Link OB VCOM for Serial Terminal (UART):
			- Short the E4 trace.
	 		- Replace R10 by 150ohm resistor.

	For FPB-RA0E2:
		To use J-Link OB VCOM for Serial Terminal (UART), the user must close E46, E47.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		The user must close E52 to use P305.
		Connect RxDA0 P305 (J2:38) to TXD Pin of Pmod USBUART (J2:3).
		Connect TxDA0 P306 (J2:36) to RXD Pin of Pmod USBUART (J2:2).
		Connect GND (J2:2) to GND Pin of Pmod USBUART (J2:5).
		Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.

	For FPB-RA0L1:
		To use J-Link OB VCOM for Serial Terminal (UART), the user must close E28, E29.
		To use the on-board debug functionality, the user must place jumper J9 on pins 1-2.

5. Verifying Operation:
	a. Import, generate, and build the example project.
	b. Refer to the Hardware Connections section to set up hardware properly.
	c. Flash or debug the example project.
	d. Open a serial terminal application (e.g., Tera Term) to view output log, configure the settings as shown below:
		- To make sure the received data shows properly, the user needs to set:
			+ [Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR].

		- To echo back characters typed in Tera Term, the user needs to enable it through:
			+ [Setup] -> [Terminal...] -> Check [Local echo].

		- The configuration parameters of the serial port on the serial terminal application:
			+ Port: The port is provided by the J-Link On-Board.
			+ Speed: 115200
			+ Data: 8 bit
			+ Parity: none
			+ Stop bits: 1 bit
			+ Flow control: none
	e. After the main menu is displayed on the terminal, the user can select the baud rate option as desired.
		- Type '1' to select baud rate = 9600 bps.
		- Type '2' to select baud rate = 14400 bps.
		- Type '3' to select baud rate = 19200 bps.
		- Type '4' to select baud rate = 38400 bps.
		- Type '5' to select baud rate = 57600 bps.
		- Type '6' to select baud rate = 115200 bps.
	f. Configure baud rate of the terminal application to match the selected baud rate displayed on the terminal.
	g. Type any ASCII key into the terminal, then press the Enter key to start UARTA operation.
	h. Type any ASCII string (up to 256 characters) into the terminal, then press the Enter key to send.
	i. View the output log on the terminal.

Note:
1) The user can refer to the Hardware User's Manual and the FSP User's Manual on Baud rate section
   to check the baud rate calculation.
