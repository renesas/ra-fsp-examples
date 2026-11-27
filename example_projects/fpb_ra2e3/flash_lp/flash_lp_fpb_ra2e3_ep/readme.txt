/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates basic functionalities of Flash LP driver. This example project initializes
	the Flash LP and performs code/data flash operations (erase, blank check, write, read) on predefined block
	based on the user input. The information and error messages are displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4M1, EK-RA4W1,
	                     FPB-RA0E1, FPB-RA0E2, FPB-RA2E3, FPB-RA2T1, FPB-RA0L1.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

	For FPB-RA2E3:
		The user must close E25, E28, E46, E47 to use J-Link OB VCOM for serial terminal (UART).
	
Note:
1) If block number less than 10 is given for code flash operations, the board may get locked.
   This happens due to R_FLASH_LP_AccessWindowSet API.

2) The user is expected to enter data not exceeding 15 bytes in size.

3) Operation is not guaranteed for any user input value other than integer (e.g., char, float, special char)
   or incorrect format through the terminal.

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
