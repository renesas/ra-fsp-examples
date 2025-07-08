/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	 
	This example project showcases CAN FD communication using two Renesas RA boards. Users can configure the nominal
	baud rate, data baud rate, and sample point at runtime, and initiate data transmission with the selected settings.
	To stop the transmission and return to the baud rate configuration menu, users simply press the Enter key without
	entering any input.
	During transmission, data is sent from Board1 to Board2. Upon receiving the data, Board2 displays it on the terminal
	and sends an updated message back to Board1. When Board1 receives this data, it converts the CAN frame to a CAN FD
	frame and transmits it back to Board2. Board2, upon receiving this frame, further updates the data and sends it to
	Board1 using the CAN FD format.
	Finally, once the transmission cycle completes, Board1 prints the received data on the terminal.

	Note: In this example project, the same code runs on both boards. The board from which the user initiates the
	transmission becomes board1, and the board which receives the data becomes board2.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M5
	2 x RA boards
	2 x Micro USB cables  
	For using on board CAN Transceivers: 3 x Jumper wires with both ends female
	For using external CAN-FD Transceiver Board (eg.OM13099): 
	    + 1 x OM13099 (CAN-FD Transceiver Board): 
	    + Link product: [OM13099 (CAN-FD Transceiver Board)](https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099).
	    + Manufacturer Product Number: OM13099
	    + 9 x Jumper wires with both ends female

2. Hardware Connections:

	Note: This project requires two RA boards, which may be the same MCUs (e.g., EK-RA8M1 <-> EK-RA8M1) or different MCUs
	(e.g., EK-RA8M1 <-> EK-RA6T3).

	Identical boards with onboard CAN transceivers populated: simply connect CANH, CANL, and GND between the two boards.

	Identical boards without populated transceivers (e.g., EK-RA8D1 <-> EK-RA8D1) or different board types lacking onboard
	transceivers (e.g., EK-RA8D1 <-> EK-RA6T3): use an external OM13099 board and wire it as detailed in the optional sections
	for each board combination.

	Supported RA Boards: EK-RA6M5.
 
	EK-RA6M5:
	    - For using on board CAN Transceivers:
		+ Short E33, E34 & E35 
		+ J33:1 of board 1 to J33:1 of board 2
		+ J33:2 of board 1 to J33:2 of board 2
		+ J33:3 of board 1 to J33:3 of board 2
       
	    - (Optional) For using external CAN Transceiver Board:
		Note: To use the external CAN transceiver, the user must either cut E33, E34, and E35 to prevent conflicts with
		the onboard CAN transceiver or configure alternative CAN CTX and CRX pins instead of cutting E33, E34, and E35.
		
		|     Board 1      | OM13099 (CAN-FD Transceiver Board) |     Board 2      |
		|------------------+------------------------------------+------------------|
		| CTX P609 (J3:12) |           CAN0 TX (J3:4)           |        ---       |
		| CRX P610 (J3:13) |           CAN0 RX (J3:2)           |        ---       |
		|   VCC (J18:4)    |            3V3 (J3:12)             |        ---       |
		|   5V0 (J18:5)    |            5V0 (J3:14)             |        ---       |
		|   GND (J18:6)    |            GND (J3:16)             |        ---       |
		|        ---       |           CAN1 TX (J1:4)           | CTX P609 (J3:12) |
		|        ---       |           CAN1 RX (J1:2)           | CRX P610 (J3:13) |
		|        ---       |           P3:2 <-> P3:13           |        ---       |
		|        ---       |           P3:7 <-> P3:18           |        ---       |

	Note:
	1. Ensure the configured bitrate of 2 Boards are same.
	2. In this example project, the same code runs on both boards. The board from which the user initiates the transmission
	becomes board1, and the board which receives the data becomes board2.
	3. User is expected to enter data of size not exceeding above 15 bytes.
	4. For OM13099 (CAN Transceiver Board):
	    a. Connect P3 loopback connection on CAN Transceiver Board with jumper cables instead of a db9 serial cable.
	    b. Connect jumpers J3, J4, J5 and J6 on OM13099 (CAN Transceiver Board) to establish connection to CAN termination resistors.

Note:
1) Ensure the configured bitrate of 2 Boards are same.
2) In this example project, the same code runs on both boards. The board from which the user initiates the transmission
   becomes board1, and the board which receives the data becomes board2.
3) The user is expected to enter data not exceeding 15 bytes in size.
4) For OM13099 (CAN Transceiver Board):
   a. Connect P3 loopback connection on CAN Transceiver Board with jumper cables instead of a db9 serial cable.
   b. Connect jumpers J3, J4, J5 and J6 on OM13099 (CAN Transceiver Board) to establish connection to CAN termination resistors.
5) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000d48
   b. Keil: Not Available 
   c. IAR: Not Available
 
6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
