/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the CRC HAL module APIs. It demonstrates CRC operation for
	data transmission in normal mode and reception in snoop mode through the SCI interface. On pressing any key
	through the terminal, CRC value in normal mode is calculated for 4 bytes of data. The calculated CRC value and
	input data are transmitted and received using the SCI_UART loopback connection. Once the transfer is complete,
	if the CRC value in snoop mode is zero and the transmit and receive buffers are equal, the operation is
	considered successful. The LED blinks only if the CRC operation is successful and stays ON if a data mismatch
	occurs. Failure and status messages are displayed on the terminal.

	Note:
		EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA4L1, EK-RA4C1, FPB-RA4E2, FPB-RA6E2 boards do not support snoop
		mode and operate in normal mode through SCI interface.
		FPB-RA0E1, FPB-RA0E2, FPB-RA0E3, FPB-RA0L1 boards do not support snoop mode and operate in normal mode
		through SAU interface.

2. Hardware Requirements:
	Supported RA boards: RSSK-RA6T1, EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1,
			     EK-RA4E2, EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1,
			     EK-RA6M2,EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2,
			     FPB-RA0E1, FPB-RA0E2, FPB-RA0E3, FPB-RA0L1, EK-RA8M1 , EK-RA8M2, EK-RA8P1, FPB-RA2E3,
			     FPB-RA2T1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1,
			     MCK-RA8T2, FPB-RA4E2, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Jumper wire.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	Establish loopback connection for SCI UART within the board with TXD and RXD pins shorted as mentioned below:
	For EK-RA6M2:
		Channel 3 has been used by SCI UART loopback operation.
		SCI3 TXD_MOSI P409 <----> SCI3 RXD_MISO P408.

	For FPB-RA4E1, EK-RA8M1, EK-RA8D1:
		Channel 3 has been used by SCI UART loopback operation.
		SCI3 TXD3 P409 <----> SCI3 RXD3 P408.

	For EK-RA2A1:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD P410 <----> SCI1 RXD P411.

	For EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P411 <----> SCI0 RXD0 P410.

	For EK-RA4M1, EK-RA6M3, EK-RA6M3G:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD_MOSI P411 <----> SCI0 RXD_MISO P410.

	For EK-RA6M1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD P101 <----> SCI0 RXD P100.

	For EK-RA4M3, EK-RA4M2, EK-RA6M5:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P101 <----> SCI0 RXD0 P100.

	For FPB-RA2E3:
		The user must close E25, E28, E46, E47 to use J-Link OB VCOM for serial terminal (UART).
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P101 (J4:11) <----> SCI0 RXD0 P100 (J4:12).

	For EK-RA4W1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD P101 <----> SCI0 RXD P104.

	For EK-RA6M4:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD1 P709 <----> SCI1 RXD1 P708.

	For EK-RA2L1:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD1 P213 <----> SCI1 RXD1 P212.

	For RSSK-RA6T1:
		Channel 4 has been used by SCI UART loopback operation.
		SCI4 TXD4 P205 <----> SCI4 RXD4 P206.

	For EK-RA2E1:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD1 P501 <----> SCI1 RXD1 P502.

	For FPB-RA6E1:
		Channel 2 has been used by SCI UART loopback operation.
		SCI2 TXD2 P302 <----> SCI2 RXD2 P301.

	For EK-RA2E2:
		Channel 9 has been used by SCI UART loopback operation.
		SCI9 TXD9 P400 <----> SCI9 RXD9 P401.

	For MCK-RA6T2:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD1 PB09 <----> SCI1 RXD1 PB08.

	For MCK-RA8T1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P603 (CN3:33) <----> SCI0 RXD0 P602 (CN3:31).

	For EK-RA2A2:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P100 <----> SCI0 RXD0 P101.

	For FPB-RA8E1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P609 (J3:13) <----> SCI0 RXD0 P610 (J3:14).

	For EK-RA4L1:
		Channel 5 has been used by SCI UART loopback operation.
		SCI5 TXD5 P506 (J1:20) <----> SCI5 RXD5 P507 (J1:19).

	For EK-RA2L2:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P101 (J2:38) <----> SCI0 RXD0 P100 (J2:39).

	For EK-RA8E2:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P112 (J14:24) <----> SCI0 RXD0 P113 (J14:22).

	For EK-RA8P1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P603 (J4:4) <----> SCI0 RXD0 P602 (J4:8).

	For FPB-RA2T1:
		Channel 2 has been used by SCI UART loopback operation.
		SCI2 TXD0 P101 (J4:11) <----> SCI2 RXD0 P100 (J4:12).
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		Channel 1 has been used by SCI UART loopback operation.
		SCI0 TXD0 P400 (J1:3) <----> SCI0 RXD0 P401 (J1:4).

	For MCK-RA8T2:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P603 (CN1:33) <----> SCI0 RXD0 P602 (CN1:31).

	For EK-RA8D2:
		Channel 3 has been used by SCI UART loopback operation.
		SCI3 TXD3 P310 (J3:40) <----> SCI3 RXD3 P408 (J17:18).

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P603 (J37:3) <----> SCI0 RXD0 P602 (J3:40).

	For FPB-RA4E2 and FPB-RA6E2:
		Channel 9 has been used by SCI UART loopback operation.
		SCI9 TXD9 P109 (J4:2) <----> SCI9 RXD9 P110 (J4:3).

	Establish loopback connection for SAU UART within the board with TXD and RXD pins shorted as mentioned below:
	For FPB-RA0E1:
		Channel 0 has been used by SAU UART loopback operation.
		SAU_UART0 TXD0 P101 <----> SAU_UART0 RXD0 P100.

	For FPB-RA0E2:
		To use P213 and P212 for SAU UART channel 1: E36, E39 must be closed and E37, E38 must be opened.
		Channel 1 has been used by SAU UART loopback operation.
		SAU_UART1 TXD1 P213 (J1:9) <----> SAU_UART1 RXD1 P212 (J1:10).

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
		Channel 0 has been used by SAU UART loopback operation.
		SAU_UART0 TXD0 P101 (J2:11) <----> SAU_UART0 RXD0 P100 (J2:12).
		
	For FPB-RA0E3:
		The user must turn OFF S3 to use the on-board debug functionality.
		Channel 1 has been used by SAU UART loopback operation.
		SAU_UART1 TXD1 P213 (J1:4) <----> SAU_UART1 RXD1 P212 (J1:5).

Note:
1) The user can change the polynomial to CRC_16 and CRC_CCITT and bit order from MSB to LSB from CRC configurator and
   observe the results (for FPB-RA0E1, FPB-RA0E2, FPB-RA0E3 and FPB-RA0L1 only support CRC_CCITT and LSB).

2) The application does not work for CRC_32 bit polynomial.

3) Please refer the latest FSP User Manual for comparison with popular online CRC calculator and the associated
   limitations are captured in it.

4) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
