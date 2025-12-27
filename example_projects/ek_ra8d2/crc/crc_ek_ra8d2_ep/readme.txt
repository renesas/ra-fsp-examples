/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the CRC HAL module APIs. It demonstrates CRC operation for
	data transmission in normal mode and reception in snoop mode through SCI interface. If the board does not
	support snoop mode (EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA4L1, EK-RA4C1), reception is performed in
	normal mode through SCI interface. If the board does not support snoop mode (FPB-RA0E1, FPB-RA0E2, FPB-RA0L1),
	reception is performed in normal mode through SAU interface. On pressing any key through RTT Viewer, CRC value
	in normal mode is calculated for 4 bytes of data. The calculated CRC value and input data are transmitted and
	received through loopback on SCI_UART (except for FPB-RA0E1, FPB-RA0E2, FPB-RA0L1 which use SAU_UART). Once the
	transfer is complete, if CRC value for snoop mode is zero and the transmit and receive buffers are equal, the
	on-board LED blinks as a sign of successful CRC operation. On data mismatch, LED stays ON. Failure and status
	messages are displayed on RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Jumper wire.

3. Hardware Connections:
	Establish loopback connection for SCI UART within the board with TXD and RXD pins shorted as mentioned below:
	For EK-RA6M2:
		Channel 3 has been used by SCI UART loopback operation.
		SCI3 TXD_MOSI P409 <----> SCI3 RXD_MISO P408

	For FPB-RA4E1, EK-RA8M1, EK-RA8D1:
		Channel 3 has been used by SCI UART loopback operation.
		SCI3 TXD3 P409 <----> SCI3 RXD3 P408

	For EK-RA2A1:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD P410 <----> SCI1 RXD P411  

	For EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P411 <----> SCI0 RXD0 P410

	For EK-RA4M1, EK-RA6M3, EK-RA6M3G:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD_MOSI P411 <----> SCI0 RXD_MISO P410

	For EK-RA6M1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD P101 <----> SCI0 RXD P100

	For EK-RA4M3, EK-RA4M2, EK-RA6M5, FPB-RA2E3:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P101 <----> SCI0 RXD0 P100

	For EK-RA4W1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD P101 <----> SCI0 RXD P104

	For EK-RA6M4:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD1 P709 <----> SCI1 RXD1 P708

	For EK-RA2L1:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD1 P213 <----> SCI1 RXD1 P212

	For RSSK-RA6T1:
		Channel 4 has been used by SCI UART loopback operation.
		SCI4 TXD4 P205 <----> SCI4 RXD4 P206

	For EK-RA2E1:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD1 P501 <----> SCI1 RXD1 P502

	For FPB-RA6E1:
		Channel 2 has been used by SCI UART loopback operation.
		SCI2 TXD2 P302 <----> SCI2 RXD2 P301

	For EK-RA2E2:
		Channel 9 has been used by SCI UART loopback operation.
		SCI9 TXD9 P400 <----> SCI9 RXD9 P401	  

	For MCK-RA6T2:
		Channel 1 has been used by SCI UART loopback operation.
		SCI1 TXD1 PB09 <----> SCI1 RXD1 PB08  

	For MCK-RA8T1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P603 (CN3:33) <----> SCI0 RXD0 P602 (CN3:31)

	For EK-RA2A2:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P100 <----> SCI0 RXD0 P101 

	For FPB-RA8E1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P609 (J3:13) <----> SCI0 RXD0 P610 (J3:14)

	For EK-RA4L1:
		Channel 5 has been used by SCI UART loopback operation.
		SCI5 TXD5 P506 (J1:20) <----> SCI5 RXD5 P507 (J1:19)

	For EK-RA2L2:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P101 (J2:38) <----> SCI0 RXD0 P100 (J2:39)

	For EK-RA8E2:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P112 (J14:24) <----> SCI0 RXD0 P113 (J14:22)

	For EK-RA8P1:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P603 (J4:4) <----> SCI0 RXD0 P602 (J4:8)

	For FPB-RA2T1:
		Channel 2 has been used by SCI UART Loopback operation.
		SCI2 TXD0 P101 (J4:11) <----> SCI2 RXD0 P100 (J4:12)
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		Channel 1 has been used by SCI UART loopback operation.
		SCI0 TXD0 P400 (J1:3) <----> SCI0 RXD0 P401 (J1:4)

	For MCK-RA8T2:
		Channel 0 has been used by SCI UART loopback operation.
		SCI0 TXD0 P603 (CN1:33) <----> SCI0 RXD0 P602 (CN1:31)

	For EK-RA8D2:
		Channel 3 has been used by SCI UART loopback operation.
		SCI3 TXD3 P310 (J3:40) <----> SCI3 RXD3 P408 (J17:18)

	Establish loopback connection for SAU UART within the board with TXD and RXD pins shorted as mentioned below:
	For FPB-RA0E1:
		Channel 0 has been used by SAU UART loopback operation.
		SAU_UART0 TXD0 P101 <----> SAU_UART0 RXD0 P100

	For FPB-RA0E2:
		To use P213 and P212 for SAU UART channel 1: E36, E39 must be closed and E37, E38 must be opened.
		Channel 1 has been used by SAU UART loopback operation.
		SAU_UART1 TXD1 P213 (J1:9) <----> SAU_UART1 RXD1 P212 (J1:10)

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
		Channel 0 has been used by SAU UART loopback operation.
		SAU_UART0 TXD0 P101 (J2:11) <----> SAU_UART0 RXD0 P100 (J2:12)

Note:
1) For any event or API failure, appropriate messages are displayed on the RTT Viewer.

2) The user can change the polynomial to CRC_16 and CRC_CCITT and bit order from MSB to LSB from CRC configurator and
   observe the results (For FPB-RA0E1, FPB-RA0E2, FPB-RA0L1 only supports CRC_CCITT and LSB).

3) The application does not work for CRC_32 bit polynomial.

4) Please refer the latest FSP User Manual for comparison with popular online CRC calculator and the associated
   limitations are captured in it.

5) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200044c
   b. Keil:     Not Available
   c. IAR:      Not Available

6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
