/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the typical use of the I3C Driver on Renesas RA MCUs based on Renesas FSP.
	The I3C master on RA board demonstrates operations associated with an I3C slave running on another RA board.
	Once initialization is successful, I3C Master device assigns slave address to I3C slave device through DAA
	(dynamic address assignment) using I3C common command code (CCC). Once Dynamic Address Assignment is completed,
	the Master EP performs write/read operations, and displays slave device information based on user input. If the
	on-board switch is pressed on the slave side, it will initiate an IBI transfer request. Error and info messages
	will be printed on J-Link RTT Viewer.

	Note: EK-RA8D1, EK-RA8P1, EK-RA8M2 boards support I3C HDR-DDR mode. The Master EP can perform write/read
	      operations both in the SDR mode and HDR-DDR mode.

2. Hardware Requirements:
	Supported RA boards: EK-RA2E2, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1, EK-RA4L1,
	                     EK-RA2L2, EK-RA8P1, EK-RA8M2
	2 x Renesas RA boards.
 	2 x Type-C USB cables for programming and debugging.
	5 x Jumper wires.
	1 x Resistor (1 kΩ, 3.9 kΩ, or 4.7 kΩ) for the SDA pull-up.
		- EK-RA8P1, EK-RA8M2: No external 3.3 V pull-up resistor is required.
		- EK-RA8M1, EK-RA8D1, EK-RA4L1, EK-RA2L2: Use a 1 kΩ resistor.

3. Hardware Connections:
	Connect USB debug ports of the two RA boards to USB ports of the host PC via two USB cables.

	The jumper wires are required to establish loopback connection along I3C lines across the board with pins
	as mentioned below:
		For EK-RA2E2:
			Master Board                    Slave Board
			-------------                   ------------
			SDA0 P401            ---->      SDA0 P401
			SCL0 P400            ---->      SCL0 P400
			Common GND

		For EK-RA4E2, EK-RA6E2:
			Master Board                    Slave Board
			-------------                   ------------
			I3C_SDA P101         ---->      I3C_SDA P101
			I3C_SCL P100         ---->     	I3C_SCL P100
			Common GND

		For MCK-RA4T1, MCK-RA6T3:
			Master Board                    Slave Board
			-------------                   ------------
			I3C_SDA P101 (TP-16) ---->   	I3C_SDA P101 (TP-16)
			I3C_SCL P100 (TP-17) ---->   	I3C_SCL P100 (TP-17)
			Common GND

		For EK-RA8M1:
			Master Board                    Slave Board
			-------------                   ------------
			I3C_SDA0 (J27:2)     ---->    	I3C_SDA0 (J27:2)
			I3C_SCL0 (J27:1)     ---->      I3C_SCL0 (J27:1)
			Common GND

		For EK-RA8D1:
			Master Board                    Slave Board
			-------------                   ------------
			I3C_SDA0 P401        ---->      I3C_SDA0 P401
			I3C_SCL0 P400        ---->      I3C_SCL0 P400
			Common GND

		For EK-RA4L1:
			Master Board                    Slave Board
			-------------                   ------------
			I3C_SDA0 P401 (J4:5) ---->   	I3C_SDA0 P401 (J4:5)
			I3C_SCL0 P400 (J4:6) ---->      I3C_SCL0 P400 (J4:6)
			Common GND

		For EK-RA2L2:
			Master Board                    Slave Board
			-------------                   ------------
			I3C_SDA0 P401 (J1:9) ---->   	I3C_SDA0 P401 (J1:9)
			I3C_SCL0 P400 (J1:8) ---->      I3C_SCL0 P400 (J1:8)
			Common GND

		For EK-RA8P1, EK-RA8M2:
			Master Board                    Slave Board
			-------------                   ------------
			I3C_SDA0 P401 (J24:9)  ---->   	I3C_SDA0 P401 (J24:9)
			I3C_SCL0 P400 (J24:10) ---->	I3C_SCL0 P400 (J24:10)
			Common GND

4. Hardware Configurations:
	For EK-RA8M1: Install jumper J61 to enable I3C.

	For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     OFF    |     OFF     |     OFF   |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

	For EK-RA4L1: The user needs to turn OFF S4-5 to enable I3C function.

	For EK-RA8P1: The user needs to turn ON SW4-5 to use P401, P400 on J24 for I3C function.

	For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29
	              on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.

Note:
1) On slave side the user can perform IBI transfer request using onboard Push button (SW2).
   For MCK-RA4T1, MCK-RA6T3: The user needs to use Toggle switch (S1) instead.

2) Readme file is common for both i3c_master and i3c_slave example projects.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200044c
   b. Keil:	Not Available
   c. IAR:	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
