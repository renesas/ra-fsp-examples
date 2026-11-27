/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the direct access, configuration, and operation of DOC SFRs. The SFRs and interrupts
	are configured directly to perform a comparison match. The ISR for the DOC is configured through the Interrupts
	tab of the FSP Configurator.

	For more information on configuring interrupts, refer to:
	https://renesas.github.io/fsp/ > RA Flexible Software Package Documentation > Starting Development > e2 studio
	User Guide > Configuring a Project > Configuring Interrupts from the Stacks Tab.
	The SFRs and interrupts are configured and enabled to perform a comparison match, using both direct access and
	BSP-provided APIs.

	LED behavior:
	a. For boards that have 3 on-board LEDs:
		LED1 is turned ON while program control waits for the user defined ISR to execute.
		LED2 is turned ON to indicate the successful conclusion of application execution.
		LED3 is turned ON if there is an expected error encountered during operation.

	b. For boards that have 2 on-board LEDs:
		LED1 is turned ON while program control waits for the user defined ISR to execute.
		LED2 is turned ON to indicate the successful conclusion of application execution.
		LED1 and LED2 are turned ON if there is an expected error encountered during operation.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A2, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4E2, EK-RA4L1, EK-RA6E2, EK-RA8D1,
			     EK-RA8D2, EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, FPB-RA2E3, FPB-RA2T1, FPB-RA8E1,
			     MCK-RA4T1, MCK-RA6T3, MCK-RA8T1, MCK-RA8T2, FPB-RA4E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For FPB-RA2T1:
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.
		The user must Close E7 to use P914 for LED2.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.

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
