/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of Local Interconnect Network (LIN) on Renesas RA MCUs
	based on the Renesas FSP. The LIN modules communicate with transceivers that satisfy the ISO9141 protocol.
	The Master defines four distinct messages, each assigned a unique ID, allowing the user to select and
	transmit a specific message to the Slave. The Master sends a start frame with the selected ID to retrieve
	data, and the Slave responds accordingly. The user can select a baud rate (2400, 4800, 9600, 10400, 14400,
	19200) from the application menu. Additionally, the LIN module's baud rate can be configured to other
	supported values, as specified in the markdown file, by modifying the configuration.xml. For the SAU LIN Slave,
	users can enter Low Power Mode (Software Standby) via the EP menu and wake up when triggered by the Master.

	Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via
	J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to
	the SEGGER J-Link RTT Viewer. If supported, it defaults to the serial terminal (UART). To modify the default
	terminal settings, refer to the instructions in the "Special Topic" section of sci_b_lin_notes.md.

	J-Link OB VCOM support: EK-RA8M1, FPB-RA0E2, EK-RA8D2.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	SEGGER J-Link RTT Viewer: Version 8.92
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: FPB-RA0E2, EK-RA8M1, EK-RA8D2
	2 x Renesas RA boards.
	2 × USB cables:
		- FPB-RA0E2, EK-RA8D2: 2 x Type-C USB cables.
		- EK-RA8M1: 2 x Micro-USB cables.
	2 x LIN transceivers.
		- Link Product: https://www.mikroe.com/lin-click
		- Manufacturer Product Number: MIKROE-3816.
	1 x 12V DC Power Supply.
	Some jumper wires.
	2 × 10kΩ pull-up resistors.

4. Hardware Connections:
	For FPB-RA0E2:
		The user must close E46, E47 to use J-Link OB VCOM for Serial Terminal (UART).

	Board         | Pin Name | Pin Number
	--------------------------------------------
	FPB-RA0E2     | RX_PIN   | P403 (J1:4)
	              | TX_PIN   | P402 (J1:3)
	              | P3V3	 | 3V3  (J3:4)
	              | GND      | GND  (J3:6)
	--------------------------------------------
	EK-RA8M1      | RX_PIN   | P610 (J54:22)
	              | TX_PIN   | P609 (J54:16)
	              | P3V3	 | 3V3  (J54:1)
	              | GND      | GND  (J54:39)
	--------------------------------------------
	EK-RA8D2      | RX_PIN   | P602 (J4:8)
	              | TX_PIN   | P603 (J4:4)
	              | P3V3	 | 3V3  (J4:1)
	              | GND      | GND  (J4:19)

	Refer to the RX_PIN and TX_PIN assignments for the connections below:
	Board1 (Master)                LIN transceiver (Board1)   LIN transceiver (Board2)  Board2 (Slave)
	------------------------------------------------------------------------------------------------
	3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V
	GND        <---------------->  GND                        GND   <---------------->  GND
	RX_PIN     <---------------->  TX                         TX    <---------------->  RX_PIN
	TX_PIN     <---------------->  RX      (See Note 2)       RX    <---------------->  TX_PIN
	3.3V       <---------------->  EN                         EN    <---------------->  3.3V
	                               LIN    <--------------->   LIN 							  
	                               GND: connect to 0V         GND: connect to 0V
	                               VS : connect to 12V        VS:  connect to 12V

	Note:
	1. The LIN transceiver requires a 12V supply. Connect VS to 12V and GND to ground.
	2. For LIN transceiver (TLE7259-3), digital I/O levels compatible with 3.3V and 5V microcontrollers.
	3. The RX pin of LIN transceiver requires an external pull-up resistor connected to the microcontroller
	   supply to define the output voltage level (refer to the hardware connections image in the .md file).
	4. For LIN transceiver Master Mode, the resistor at JP2 (MODE) must be relocated to the right side:
	   Removing the resistor at the left side and soldering it at the right side.

Note:
1) For Serial terminal application:
   1.1 To echo back characters typed in Tera Term, the user needs to enable it through
       [Setup] -> [Terminal...] -> Check [Local echo].
   1.2 The configuration parameters of the serial port on the terminal application are as follows:
		COM port is port provided by the J-Link on-board.
		Baud rate: 115200 bps
		Data length: 8-bits
		Parity: none
		Stop bit: 1-bit
		Flow control: none

2) For SEGGER J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
