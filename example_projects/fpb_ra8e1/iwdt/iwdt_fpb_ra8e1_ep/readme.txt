/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the typical use of the IWDT HAL module. In the terminal, the user can
	press '1' to initialize the IWDT and start the GPT timer. The IWDT counter is refreshed periodically every
	1 second when the GPT timer expires. Refresh status will be printed every 2 seconds. Once the user
	presses '2', the IWDT counter stops refreshing and resets the MCU.

	For FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3: In the terminal, the user can enter '1' to initialize the IWDT
	and start the TAU timer. The IWDT counter is refreshed periodically every 1 second when the TAU timer expires.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4E2, EK-RA4L1,
	                     EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G,
	                     EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, FPB-RA0E1,
	                     FPB-RA0E2, FPB-RA0E3, FPB-RA0L1, FPB-RA2E3, FPB-RA2T1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1,
	                     MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, MCK-RA8T2, RSSK-RA6T1.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For FPB-RA2T1:
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.

	For FPB-RA0E3:
		The user must turn OFF S3 to use the on-board debug functionality.

	For FPB-RA8E1:
		The user must open jumpers J6 and J9 to use the on-board debug functionality.

Note:
1) To calculate IWDT underflow time, please refer to the formula in the link:
   https://renesas.github.io/fsp/group___i_w_d_t.html

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
