/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates some features of C++ such as class, constructors, destructor, member functions
	using ADC HAL module APIs provided by FSP. The ADC module is wrapped into a class. An ADC object of this class
	is instantiated and is used to call respective member functions. The project initializes the ADC in Single Scan
	or Continuous Scan modes based on user selection in RA Configurator. Once initialized, the user can initiate
	the ADC scan and also stop the scan (in the case of Continuous Scan mode) using the terminal by sending
	commands. Result and ADC status are displayed on the terminal.

	Note: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3 boards operate in One-shot or Sequential modes.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4E2, EK-RA4L1,
	                     EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G,
	                     EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, FPB-RA0E1,
	                     FPB-RA0E2, FPB-RA0E3, FPB-RA0L1, FPB-RA2E3, FPB-RA2T1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1,
	                     MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, MCK-RA8T2, RSSK-RA6T1, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA2A1:
		Connect AVSS0 (J2:36) to VREFL0 (J2:34) using a jumper wire.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4,
		5-6, 7-8 to use the on-board debug functionality.

	For FPB-RA0E3:
		The user must turn OFF S3 to use the on-board debug functionality.

	No hardware connections required for other RA boards.

4. Configuration Settings:
	By default, the example project runs in Continuous Scan mode on the boards.
	In order to switch the scan mode, change the "Mode" field of ADC Driver in the RA Configurator to Single Scan.

	For FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3:
		By default, the example project runs in Sequential mode on the board. In order to switch the scan mode,
		change the "Conversion operation" field of ADC_D Driver in the RA Configurator to One-shot.

	For MCK-RA6T2, EK-RA8P1, MCK-RA8T2, EK-RA8D2, EK-RA8M2:
		By default, the example project runs in Continuous Scan mode on the board. In order to switch the scan
		mode, change the "Scan mode" field of ADC_B Driver in the RA Configurator to Single Scan.

Note:
1) EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4M1, EK-RA2A2, EK-RA4L1, EK-RA2L2, FPB-RA2T1 boards support only
   Single Scan mode.

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
