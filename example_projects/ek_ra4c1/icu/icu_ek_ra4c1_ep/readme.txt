/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the ICU HAL module APIs. The project initializes the ICU
	module. On pressing the user push button, an external IRQ is triggered, which toggles on-board LED.

	For MCK-RA4T1, MCK-RA6T3: On flipping the toggle switch (OFF->ON), an external IRQ is triggered,
	which toggles on-board LED.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4E2,
			     EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2,
			     EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2, EK-RA8M1,
			     EK-RA8M2, EK-RA8P1, FPB-RA0E1, FPB-RA0E2, FPB-RA0E3, FPB-RA0L1, FPB-RA2E3,
			     FPB-RA2T1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3,
			     MCK-RA8T1, MCK-RA8T2, RSSK-RA6T1, FPB-RA4E2, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Pmod BTN (For MCK-RA8T1, MCK-RA8T2: e.g., https://digilent.com/reference/pmod/pmodbtn/start).
	1 x Jumper wire (For FPB-RA2E3, FPB-RA2T1).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For MCK-RA4T1, MCK-RA6T3:
		Connect to inverter board.
		Use Switch S1 (toggle switch).

	For MCK-RA8T1:
		Connect Pmod BTN (J1:1) to PMOD (CN6:1).
		Use Switch BTN1 (push button on Pmod BTN).

	For EK-RA2A1, EK-RA6M1, EK-RA6M2, EK-RA6M5, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA4M1, EK-RA4M3, EK-RA4M2,
	    EK-RA2E1, EK-RA2L1, FPB-RA4E1, FPB-RA6E1, EK-RA4E2, EK-RA6E2, EK-RA8M1, EK-RA8D1, FPB-RA0E1, FPB-RA8E1,
	    EK-RA4L1, FPB-RA0E2, EK-RA2L2, FPB-RA4E2, FPB-RA6E2:
		Use Switch S1 (push button).

	For RSSK-RA6T1, MCK-RA6T2, EK-RA2E2, EK-RA2A2:
		Use Switch S2 (push button).

	For FPB-RA2E3:
		The user must close E25, E28, E46, E47 to use J-Link OB VCOM for serial terminal (UART).
		Connect wire to short NMI P200 (J3:21) to P206 (J3:18).
		Use Switch S1 (push button).

	For EK-RA4W1, EK-RA8E2, EK-RA8P1, EK-RA4C1, FPB-RA0L1, EK-RA8D2, EK-RA8M2:
		Use Switch SW1 (push button).
		For EK-RA4C1:
			The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and turn OFF SW4-4
			to use the on-board debug functionality.
		For FPB-RA0L1:
			The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
			The user must close E20 to use P002 for LED1.
		For EK-RA8M2:
			The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
			3-4, 5-6, 7-8 to use the on-board debug functionality.

	For FPB-RA2T1:
		The user must close E8, E15, and cut E10 to use P213 for LED1.
		Connect wire to short NMI P200 (J3:21) to IRQ0 P206 (J3:18).
		Use Switch S1 (push button).

	For FPB-RA0E3:
		The user must turn OFF S3 to use the on-board debug functionality.
		Use Switch S1 (push button).

	For MCK-RA8T2:
		Connect Pmod BTN (J1:1) to Pmod 2A (CN5:1).
		Please carefully align pin 1 on the Pmod BTN with pin 1 on the Pmod 2A.
		Use Switch BTN1 (push button on Pmod BTN).

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
