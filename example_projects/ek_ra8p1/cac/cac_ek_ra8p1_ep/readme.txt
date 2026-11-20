/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of CAC driver. On any key press from the terminal,
	CAC module starts measuring the accuracy of selected target clock against the selected reference clock.
	Result of the measurement is displayed on the terminal.

2. Hardware Requirements:
	Supported RA Boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4E2, EK-RA4L1,
	                     EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G,
	                     EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, FPB-RA2E3,
	                     FPB-RA2T1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1,
	                     MCK-RA8T2, RSSK-RA6T1, FPB-RA4E2, FPB-RA6E2.

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

	For FPB-RA2E3:
		The user must close E25, E28, E46, E47 to use J-Link OB VCOM for serial terminal (UART).

Note:
1) The user may select any reference clock and clock divider. However, ensure that the reference clock is appropriately
   chosen relative to the CPU clock. A high reference clock may cause infinite interrupts upon measurement completion.
   To avoid this, use a lower reference clock frequency or adjust the clock divider accordingly.

2) The user may select any measurement clock and clock divider.

3) The user needs to ensure that the Main clock/Sub-clock oscillator is available on the board before selecting it.

4) The user must set upper limit and lower limit in configurator based on the selected reference and measurement clocks.

5) Formula to calculate threshold (upper/lower) limits:
	a. Get Measurement clock frequency with clock divider i.e. say 'M'.
	   for ex. select HOCO(24MHZ on EK-RA4M1) and clock divider as '1', then M = 24MHZ.
	b. Get Reference clock frequency with clock divider i.e. say 'R'.
	   for ex. select Main OSC(12MHZ on EK-RA4M1) and clock divider as '32', then R = (12MHZ/32).
	c. Calculate count value, say 'C'.
	   i.e. C = (M/R), so C = 64.
	d. Take a tolerance of 10%, say 'T'.
	   i.e. T = (C * (10/100)) = C/10 = 6.4.
	e. Upper limit Threshold = (count value + tolerance) = (C + T) = 70.4.
	f. Lower limit Threshold = (count value - tolerance) = (C - T) = 57.6.

6) For clock frequencies, refer to the "Clocks" tab in the configurator or the board-specific user manual.

7) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
