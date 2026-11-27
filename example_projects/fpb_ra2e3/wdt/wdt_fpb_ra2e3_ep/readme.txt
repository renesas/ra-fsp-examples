/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the typical use of the WDT HAL module APIs. The user can enter '1' through
	terminal to initialize the WDT and start the GPT timer. The WDT counter is refreshed periodically
	every 1 second when the GPT timer expires. Refresh status will be printed every 3 seconds. The user can press
	the push button to stop the GPT timer which in turn stops refreshing the WDT timer. After approximately 2
	seconds, the WDT resets the MCU and turns the LED ON until the user provides the input once again.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4E2,
			     EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2,
			     EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2, EK-RA8M1,
			     EK-RA8M2, EK-RA8P1, FPB-RA2E3, FPB-RA2T1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, MCK-RA4T1,
			     MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, MCK-RA8T2, RSSK-RA6T1.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging.
	1 x Pmod BTN (For MCK-RA8T1, MCK-RA8T2. e.g., https://digilent.com/reference/pmod/pmodbtn/start).
	1 x Jumper wire (For FPB-RA2E3, FPB-RA2T1).

3. Hardware Connections:
	For EK-RA2A1, EK-RA2E1, EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, EK-RA6M5,
	    EK-RA4E2, EK-RA6E2, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T3, FPB-RA8E1, EK-RA4L1, EK-RA2L2:
		Use Switch S1 to stop the WDT from refreshing.

	For EK-RA4W1, EK-RA8E2, EK-RA8P1, EK-RA8D2:
		Use Switch SW1 to stop the WDT from refreshing.

	For EK-RA2E2, RSSK-RA6T1, EK-RA6M3, EK-RA6M3G, MCK-RA6T2, EK-RA2A2, EK-RA8M1, EK-RA8D1:
		Use Switch S2 to stop the WDT from refreshing.

	For FPB-RA2E3:
		Connect wire to short P200/NMI (J3:21) to P206 (J3:18).
		Use Switch S1 to stop the WDT from refreshing.
		The user must close E25, E28, E46, E47 to use J-Link OB VCOM for serial terminal (UART).

	For MCK-RA8T1:
		Connect Pmod BTN (J1:1) to Pmod (CN6:1).
		Use Switch BTN1 (push button on Pmod BTN) to stop the WDT from refreshing.

	For FPB-RA2T1:
		Connect wire to short P200/NMI (J3:21) to P400 (J3:1).
		Use Switch S1 to stop WDT from refreshing.
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		Use Switch SW1 to stop the WDT from refreshing.

	For MCK-RA8T2:
		Connect Pmod BTN (J1:1) to Pmod 2A (CN5:1).
		Please carefully align pin 1 on the Pmod BTN with pin 1 on the Pmod 2A.
		Use Switch BTN1 (push button on Pmod BTN) to stop the WDT from refreshing.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.
		Use Switch SW1 to stop the WDT from refreshing.

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
