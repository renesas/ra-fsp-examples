/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the TML HAL module on Renesas RA MCUs. The user can select different TML
	operating modes through the terminal, including counter operation, capture operation, and counter operation
	in low power mode. In counter operation in normal mode, three TML instances are configured as 8-bit, 16-bit,
	and 32-bit counters. The 16-bit capture mode uses an ELC software event to capture the raw timer count value.
	In counter operation in low power mode, the 32-bit timer is configured as counter mode with input period time,
	and the RA board wakes up from LPM to normal mode when the timer expires. EP information and error messages
	are displayed on the terminal.

2. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.
	
	For FPB-RA0E1:
	---------
		The user must close E4, and populate R10 with a resistor ranging from 0 to 150 ohms
		to use J-Link OB VCOM for Serial Terminal (UART).

	For FPB-RA0E2:
	---------
		The user must close E46, E47 to use J-Link OB VCOM for Serial Terminal (UART).

	For FPB-RA0L1:
	---------
		The user must close E28, E29 to use J-Link OB VCOM for Serial Terminal (UART).
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality. 

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
