/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of Local Interconnect Network (LIN) on Renesas RA MCUs based on 
	the Renesas FSP. The LIN modules communicate with transceivers that satisfy the ISO9141 protocol. The Master defines 
	four distinct messages, each assigned a unique ID, allowing the user to select and transmit a specific message to 
	the Slave. The Master sends a start frame with the selected ID to retrieve data, and the Slave responds accordingly. 
	The user can select a baud rate (2400, 4800, 9600, 10400, 14400, 19200) from the application menu. Additionally, 
	the LIN module's baud rate can be configured to other supported values, as specified in the markdown file, by modifying 
	the configuration.xml. For the SAU LIN Slave, users can enter Low Power Mode (Software Standby) via the EP menu and 
	wake up when triggered by the Master.

	Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, 
	depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer. 
	If supported, it defaults to the serial terminal (UART).
	To modify the default terminal settings, refer to the instructions in the "Special Topic" section of sau_lin_notes.md.

	J-Link OB VCOM support: FPB-RA0E2, EK-RA8M1.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	SEGGER J-Link RTT Viewer: Version 9.42
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: FPB-RA0E2, EK-RA8M1, FPB-RA0E3.
	2 x Renesas RA boards.
	2 x USB cables:
		- FPB-RA0E2, FPB-RA0E3: 2 x USB Type-C cables
		- EK-RA8M1: 2 x Micro-USB cables
	2 LIN transceivers.
		- Link Product: https://www.mikroe.com/lin-click
		- Manufacturer Product Number: MIKROE-3816
	1 x 12V DC Power Supply.
	Some jumper wires.
	2 x 10kΩ pull-up resistors.

4. Hardware Configuration:
	For LIN transceiver settings:
	Label | Name       | Default | Description
	----------------------------------------------------------------------------------------------------------------------
	JP1   | VCC SEL    | Left    | Power supply voltage selection: Left position (3.3V), Right position (5V).
	JP2   | MODE       | Left    | Communication mode selection: Left position (Slave mode), Right position (Master mode).

5. Hardware Connections:
	For FPB-RA0E2:
		 The user must close E46, E47 to use J-Link OB VCOM for Serial Terminal (UART).

	For FPB-RA0E3:
		The user must turn OFF S3 to use the on-board debug functionality.

	Board         | Pin Name | Pin Number
	--------------------------------------------
	FPB-RA0E2     | RX_PIN   | P403 (J1:4)
	              | TX_PIN   | P402 (J1:3)
	              | P3V3     | 3V3  (J3:4)
	              | GND      | GND  (J3:6)
	--------------------------------------------
	EK-RA8M1      | RX_PIN   | P610 (J54:22)
	              | TX_PIN   | P609 (J54:16)
	              | P3V3     | 3V3  (J54:1)
	              | GND      | GND  (J54:39)
	-------------------------------------------- 
	FPB-RA0E3     | RX_PIN   | P100 (J2:7)
	              | TX_PIN   | P101 (J2:6)
	              | P3V3     | 3V3  (J21:4)
	              | GND      | GND  (J21:6)

	Refer to the RX_PIN and TX_PIN assignments for the connections below:
	Board1(Master)                 LIN transceiver(Board1)    LIN transceiver(Board2)   Board2(Slave)
	------------------------------------------------------------------------------------------------
	3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
	GND        <---------------->  GND                        GND   <---------------->  GND      
	RX_PIN     <---------------->  TX                         TX    <---------------->  RX_PIN
	TX_PIN     <---------------->  RX      (See Note 3)       RX    <---------------->  TX_PIN
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
1) FPB-RA0E3 limitations: Due to the limited RAM size (2 KB), Serial Terminal is not supported on FPB-RA0E3.

2) For Serial terminal application:
   1.1 To echo back characters typed in Tera Term, the user needs to enable it through:
       [Setup] -> [Terminal...] -> Check [Local echo].
   1.2 The configuration parameters of the serial port on the terminal application are as follows:
		COM port is port provided by the J-Link on-board.
		Baud rate: 115200 bps
		Data length: 8-bits
		Parity: none
		Stop bit: 1-bit
		Flow control: none

3) For SEGGER J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
