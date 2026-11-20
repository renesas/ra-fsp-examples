/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the functionality of GPT input capture module. GPT0/GPT5 is used to generate
	periodic pulses of 500 milliseconds duration and provided as input to GPT input capture (GPT1/GPT6). GPT1 counts
	the event pulse received at its input. Based on the period and capture event, the time period of pulse is
	calculated and displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4E2,
	                     EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2,
	                     EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2, EK-RA8M1,
	                     EK-RA8M2, EK-RA8P1, FPB-RA2E3, FPB-RA2T1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, MCK-RA4T1,
	                     MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, MCK-RA8T2, RSSK-RA6T1, FPB-RA4E2, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Jumper wire.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA2A1:
		Connect P400 to P301.

	For EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M5:
		Connect P405 to P415.

	For EK-RA4W1:
		Connect P105 to P213.

	For FPB-RA6E1:
		Connect P405 to P103.
	
	For EK-RA6M4:
		Connect P105 to P512.

	For RSSK-RA6T1:
		Connect P103 to P112.

	For EK-RA2E1:
		Connect P500 to P105.

	For EK-RA2E2:
		Connect P112 to P101.

	For FPB-RA4E1, FPB-RA8E1:
		Connect P105 to P113.

	For MCK-RA6T2:
		Connect PE14 to PE12.

	For EK-RA6E2, EK-RA4E2:
		Connect P207 to P105.

	For MCK-RA4T1, MCK-RA6T3:
		Connect P409 to P113.

	For EK-RA8M1, EK-RA8D1:
		Connect P405 to P713.

	For FPB-RA2E3:
		The user must close E25, E28, E46, E47 to use J-Link OB VCOM for serial terminal (UART).
		Connect P103 (J4:9) to P111 (J4:4).


	For EK-RA2A2:
		Connect P104 to P502.

	For MCK-RA8T1:
		Connect P113 to P300.
		
	For EK-RA4L1:
		Connect P405 (J2:42) to P304 (J2:7).

	For EK-RA8E2:
		Connect P405 (J2:7) to P403 (J2:5).

	For EK-RA2L2: 
		Connect P103 (J2:36) to P112 (J2:30).

	For EK-RA8P1, EK-RA8D2:
		Connect P403 (J17:7) to P700 (J3:9).

	For FPB-RA2T1:
		Connect P101 (J4:11) to P103 (J4:9).

	For EK-RA4C1:
		Connect P405 (J2:42) to P304 (J2:7).
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For MCK-RA8T2:
		Connect P203 (CN3:29) to P300 (CN2:14).

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect P115 (J1:17) to P300 (J1:22).

	For FPB-RA4E2:
		Connect P815 (J3:16) to P105 (J4:11).

	For FPB-RA6E2:
		Connect P815 (J3:16) to P105 (J4:11).

Note:
1) Generated periodic pulse for RA2A1 is 1000 milliseconds.

2) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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

3) For GCC ARM Embedded Toolchain: To enable printing floats to the terminal, edit the project settings and
   ensure that "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous >
   Use float with nano printf
