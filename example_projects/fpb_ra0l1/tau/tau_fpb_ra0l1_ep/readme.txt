/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic usage of TAU driver on Renesas RA MCUs based on Renesas FSP.
	It consists of 7 independent operations coordinated together to create 3 sequences. They include setting the
	period for LED ON and OFF times, counting external events, and measuring low-level pulse width. The user will
	input the required parameters for each sequence to configure the corresponding independent channel operation
	function of TAU. The EP information and error messages will be printed to the terminal.

2. Hardware Requirements:
	Supported RA boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	5 x Jumper wires.
	1 x Host PC.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For FPB-RA0E1:
		1. To use J-Link OB VCOM for Serial Terminal (UART):
			The soldering bridge E4 must be closed.
			Populate R10 with a resistor ranging from 0 to 150 ohms.
		2. Pin connection:
			LED1 P008 (J2:30) <--------> TI03 P112 (J2:20)
			TO01 P110 (J2:19) <--------> TI00 P213 (J1:5)
			TO00 P212 (J1:6)  <--------> TI04 P100 (J2:24)
			LED2 P009 (J2:29) <--------> TI05 P103 (J2:21)
			TO06 P102 (J2:22) <--------> TI07 P101 (J2:23)

	For FPB-RA0E2:
		1. To use J-Link OB VCOM for Serial Terminal (UART), the user must close E46, E47.
		2. To use P103 and P102 for LED1 and LED2, the user must close E28, E29.
		3. Pin connection:
			LED1 P103 (J2:13) <--------> TI03 P112 (J2:5)
			TO01 P110 (J2:3)  <--------> TI00 P213 (J1:9)
			TO00 P212 (J1:10) <--------> TI04 P408 (J1:15)
			LED2 P102 (J2:14) <--------> TI05 P201 (J1:26)
			TO06 P301 (J1:31) <--------> TI07 P111 (J2:4)

	For FPB-RA0L1:
		1. To use J-Link OB VCOM for Serial Terminal (UART), the user must close E28, E29.
		2. To use the on-board debug functionality, the user must place jumper J9 on pins 1-2.
		3. To use P002 and P104 for LED1 and LED2, the user must close E20, E18.
		4. Pin connection:
			LED1 P002 (J2:22) <--------> TI03 P500 (J2:13)
			TO01 P110 (J2:3)  <--------> TI00 P213 (J1:7)
			TO00 P212 (J1:8)  <--------> TI04 P408 (J1:11)
			LED2 P104 (J2:8)  <--------> TI05 P201 (J1:20)
			TO06 P301 (J1:23) <--------> TI07 P111 (J2:4)

	For FPB-RA0E3:
		1. To use the on-board debug functionality, the user must turn OFF S3.
		2. To use P010 and P011 for LED1 and LED2, the user must close E48, E49.
		3. Pin connection:
			LED1 P010 (J1:1)  <--------> TI03 P112 (J2:4)
			TO01 P110 (J2:3)  <--------> TI00 P213 (J1:4)
			TO00 P212 (J1:5)  <--------> TI04 P100 (J2:7)
			LED2 P011 (J2:10) <--------> TI05 P201 (J1:8)
			TO06 P102 (J2:5)  <--------> TI07 P101 (J2:6)

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

2) For GCC ARM Embedded Toolchain (except for FPB-RA0E3 board): To enable printing floats to the terminal,
   edit the project settings and ensure that "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous >
   Use float with nano printf.
