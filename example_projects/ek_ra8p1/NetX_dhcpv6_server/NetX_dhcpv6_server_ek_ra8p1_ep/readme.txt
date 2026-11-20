/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of NetX Duo DHCPv6 Server with an Ethernet driver on Renesas
	RA MCUs based on Renesas FSP using Azure RTOS. The DHCPv6 Server is created and, based on the user input, the
	user can start the DHCPv6 Server and print the DHCPv6 clients info. On successful completion of each operation, the
	success status is displayed on the terminal. Error and info messages are also printed there.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	Terminal Console Application: Tera Term or a similar application
	LLVM Embedded Toolchain for ARM: Version 22.1.0

3. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2,
	                     MCK-RA8T2, EK-RA8T2
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 × PC supported ethernet or another RA board running DHCPv6 Client.
	Without ethernet switch:
		1 x Ethernet/LAN cable (Ethernet cable CAT5/6).
	With ethernet switch: (Optional)
		1 x Ethernet switch.
		2 x Ethernet/LAN cables (Ethernet cable CAT5/6).
		Note: The ethernet switch should not have Internet access; it only serves as a connection hub.

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and debugging.

	Without ethernet switch:
		Connect the ethernet port of the RA board to the ethernet port of the PC/another RA board
		running DHCPv6 Client using a LAN cable.
	With ethernet switch: (Optional)
		Connect the ethernet port of the RA board and the ethernet port of the PC/another RA board
		running DHCPv6 Client to an ethernet switch using two LAN cables.

	For MCK-RA8T2, EK-RA8T2:
		Use the ethernet port 0 of the RA board (CN14 for MCK-RA8T2, J15 for EK-RA8T2) for the connections above.

5. Hardware Configuration:
	For EK-RA8D1:
		Set the configuration switches (SW1) as below to avoid potential failures:
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |      OFF    |      OFF     |     OFF    |     ON     |      OFF   |      OFF    |     OFF   |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		CAUTION: Do not enable SW1-4 and SW1-5 together.

	For EK-RA8M1:
		Remove jumper J61 to enable Ethernet B.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.

	For EK-RA8T2:
		The user must set the configuration switches (SW6 and SW4) as below to use the on-board debug
		functionality and enable Ethernet 0.
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+

		+-------------+-------------+------------+------------+------------+------------+-------------+-------------+
		| SW4-1 PMOD1 | SW4-2 PMOD1 | SW4-3 CAN0 | SW4-3 CAN1 | SW4-5 PMIC | SW4-6 ETH0 | SW4-7 USBFS | SW4-8 RS485 |
		+-------------+-------------+------------+------------+------------+------------+-------------+-------------+
		|     OFF     |     OFF     |     OFF    |     OFF    |     OFF    |     OFF    |     OFF     |      OFF    |
		+-------------+-------------+------------+------------+------------+------------+-------------+-------------+

Note:
1) The user should wait for the "Ethernet link is up." message on the terminal to confirm the Ethernet link status.

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
