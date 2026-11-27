/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the functionality of ULPT in Periodic mode and One-Shot mode. The user
	selects the period for both One-Shot and Periodic timers as 1 second, 2 seconds, or 3 seconds. After selecting
	the period, the ULPT1 starts in Periodic mode and toggles the on-board LED when the ULPT1 underflow interrupt
	occurs. The ULPT1 will stop after underflow three times, and the ULPT0 will start in One-shot mode after that.
	The MCU will be put into Deep SW Standby mode and wait for the ULPT0 underflow interrupt to reset the MCU
	internally to normal mode and restart the EP.

2. Hardware Requirements:
	Supported RA boards: EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2,
			     MCK-RA8T2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For FPB-RA8E1:
		The user must open jumpers J6 and J9 to use the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4,
		5-6, 7-8 to use the on-board debug functionality.

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

2) The user should terminate the debugging session and run the example project in standalone mode.
