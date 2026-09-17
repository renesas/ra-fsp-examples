/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the configuration of the DAC, AGT, and DTC to generate a continuous
	sinusoidal signal. The user will define the samples per period and frequency in Hz of the desired output wave
	via J-Link RTT Viewer. The samples per period are used to compute and store 1 period of the sinusoidal in a
	lookup table (LUT), and the frequency is used to set the AGT period to the appropriate sampling rate. The AGT
	generates an interrupt which triggers the DTC to transfer the next value of the LUT to the DADR0 register. The
	output sinusoidal wave will oscillate between the minimum and maximum DAC values at the user-specified samples
	per period and frequency.

	For MCK-RA8T1: The AGT generates an interrupt which triggers the DTC to transfer the next value of the LUT to
	the DADR1 register.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2L1, EK-RA4M2, EK-RA6M4, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3,
			     EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA8P1, EK-RA8D2,
			     EK-RA8M2, MCK-RA8T2, EK-RA8P1 V2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	Optional hardware to view waveform: Oscilloscope and probes.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and 
	debugging.

	Optional: To capture the output signal from the DAC, set up the connection as shown below:
		Connect oscilloscope probes to the DAC output and GND pin.

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

		For EK-RA8P1 V2:
			The user must set the configuration switches (SW6) as below to use the on-board
			debug functionality.
			+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
			| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
			+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
			|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
			+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
			P014 (J3:22) is DA0 output.

Note:
1) J-Link RTT Viewer only supports integer values for user input.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
