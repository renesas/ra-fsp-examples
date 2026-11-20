/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of DMAC driver on Renesas RA MCUs based on Renesas FSP.
	The project initializes 3 DMAC transfer instances. Upon successful initialization, a menu option is displayed.
	When the user inputs 1, AGT generates 100ms interrupt to trigger a transfer from the source buffer to the port
	control register for 60 times indicated by LED toggling for 60 times. When the user inputs 2, 32-bit data is
	transferred from the GPT counter register to the destination array which is printed on the terminal. When the
	user inputs 3, 32-bit data is transferred from predefined source array to the port register and the state of
	LED changes accordingly.

2. Hardware Requirements:
	Supported RA boards: EK-RA4C1, EK-RA4E2, EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2,
			     EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2,
			     EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, MCK-RA4T1,
			     MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, MCK-RA8T2, RSSK-RA6T1, FPB-RA4E2, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4,
		5-6, 7-8 to use the on-board debug functionality.

	For FPB-RA8E1:
		The user must open jumpers J6 and J9 to use the on-board debug functionality.

Note:
1) On starting or resetting the board, at times the DMAC transfer operation takes more time. Due to this the LED
   blinking frequency decreases/pauses. Please reset the board in that case, and it will run as expected.

2) For EK-RA6M3 and EK-RA6M3G boards: LED blinking pauses in-between and then continues.

3) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
