/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates typical use of GPT HAL module APIs. The user has the provision to input value
	as per displayed menu through the terminal to select different GPT supported modes (Periodic, PWM, One-Shot).
	In Periodic mode, the user can enter the time period within the permitted ranges to change the frequency of
	the user LED. In PWM mode, the user can enter the duty cycle within the specified range to adjust the intensity
	of the user LED. In One-Shot mode, output will be displayed on the terminal. Any failure will also be
	displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4E2,
	                     EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2,
	                     EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2, EK-RA8M1,
	                     EK-RA8M2, EK-RA8P1, FPB-RA2E3, FPB-RA2T1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, MCK-RA4T1,
	                     MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, MCK-RA8T2, RSSK-RA6T1, FPB-RA4E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging.
	1 x Jumper wire (For EK-RA2L1, EK-RA2E1, EK-RA6M5, FPB-RA4E1, EK-RA2A2, EK-RA2L2).

3. Hardware Connections:
	For EK-RA2L1:
		Connect P414 (J1:4) to P505 (J3:6).

	For EK-RA2E1:
		Connect P212 (J2:4) to P913 (J2:12).

	For EK-RA6M5:
		Connect P414 (J1:32) to P008 (J4:26).

	For FPB-RA4E1:
		Connect P104 (J4:21) to P407 (J3:25).

	For EK-RA2A2:
		Connect P105 (J4:16) to P307 (J3:16).

	For EK-RA2L2:
		Connect P104 (J2:35) to P204 (J2:17).

	For FPB-RA2T1:
		The user must close E8, E15 and cut E10 to use P213 for LED1.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.

Note:
1) Timer period range in millisecond as per EP configuration:
   a. EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, RSSK-RA6T1, MCK-RA6T2, EK-RA8M1, -- (0 to 35791) ms
      EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA8E2
   b. EK-RA4M1, EK-RA2L1, EK-RA2E1, FPB-RA2E3                                   -- (0 to 89478) ms
   c. EK-RA2A1, EK-RA2E2, EK-RA2A2, EK-RA2L2                                    -- (0 to 1398)  ms
   d. EK-RA4W1                                                                  -- (0 to 2097)  ms
   e. EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, FPB-RA4E1                         -- (0 to 42949) ms
   f. FPB-RA6E1, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, FPB-RA4E2            -- (0 to 671)   ms
   g. EK-RA4L1                                                                  -- (0 to 838)   ms
   h. EK-RA8P1, MCK-RA8T2, EK-RA8D2, EK-RA8M2 (GPT Src: PCLKD (250 MHz))        -- (0 to 17179) ms
   i. FPB-RA2T1, EK-RA4C1                                                       -- (0 to 1048)  ms
   k. EK-RA8D2, EK-RA8M2 (GPT Src: GPTCLK (300 MHz))                            -- (0 to 14316) ms

2) For EK-RA8D2, EK-RA8M2, the user can select GPT Clock Source as GPTCLK or PCLKD through:
   configuration.xml > Clocks > Clocks Configuration > GPT Src

3) The user is expected to enter data not exceeding 15 bytes in size.

4) Operation is not guaranteed for any user input value other than integer (e.g., float, char, special char)
   through the terminal.

5) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
