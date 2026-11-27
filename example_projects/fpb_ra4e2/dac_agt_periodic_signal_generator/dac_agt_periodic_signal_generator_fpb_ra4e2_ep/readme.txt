/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the configuration of the DAC, AGT, and DTC to generate a continuous
	sinusoidal signal. The user will define the samples per period and frequency in Hz of the desired output wave
	via the terminal. The samples per period are used to compute and store one period of the sinusoidal in a
	lookup table (LUT), and the frequency is used to set the AGT period to the appropriate sampling rate. The AGT
	generates an interrupt that triggers the DTC to transfer the next value of the LUT to the DADR0 register. The
	output sinusoidal wave will oscillate between the minimum and maximum DAC values at the user-specified samples
	per period and frequency.

	For MCK-RA8T1: The AGT generates an interrupt which triggers the DTC to transfer the next value of the LUT to
	the DADR1 register.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2L1, EK-RA4M2, EK-RA6M4, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3,
	                     EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA8P1, EK-RA8D2,
	                     EK-RA8M2, MCK-RA8T2, FPB-RA4E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	Optional hardware to view waveform: Oscilloscope and probes.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and 
	debugging.

	The following pin connections are required to capture the output signal from the DAC:
		For EK-RA2L1, EK-RA4M2, EK-RA6M4, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1:
			P014 is DA0 output.

		For EK-RA2A1:
			P500 is DA12_0 output.

		For MCK-RA8T1:
			P015 (CN1:13) is DA12_1 output.

		For FPB-RA8E1:
			P014 (J1:34) is DA0 output.

		For EK-RA4L1:
			P004 (J1:7) is DA0 output.

		For EK-RA8E2:
			P014 (J17:2) is DA0 output.

		For EK-RA8P1, EK-RA8D2:
			P014 (J2:4) is DA0 output.

		For EK-RA8M2:
			The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on
			pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
			P014 (J3:20) is DA0 output.

		For MCK-RA8T2:
			P014 (CN2:28) is DA0 output.

		For FPB-RA4E2:
			P014 (J4:21) is DA0 output.

Note:
1) Terminal only supports integer values for user input.

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
