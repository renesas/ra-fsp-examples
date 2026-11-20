/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionality of NetX Duo DHCPv6 Client with an Ethernet driver on
	Renesas RA MCUs based on Renesas FSP using Azure RTOS. The DHCPv6 Client is created and started. On successful
	receiving of an IPv6 address from the DHCPv6 Server, it pings the DNS Server to verify the received IPv6 address.
	The error and info messages will be printed on the terminal.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	Terminal Console Application: Tera Term or a similar application
	LLVM Embedded Toolchain for ARM: Version 22.1.0

3. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2,
	                     MCK-RA8T2, EK-RA8T2, EK-RA8P1 V2.
	2 x Renesas RA board.
		- 1 x RA board runs the NetX_dhcpv6_server example project.
		- 1 x RA board runs the NetX_dhcpv6_client example project.
	2 x USB cables for programming and debugging (USB cable type varies by board model).
	1 x Ethernet/LAN cable (Ethernet cable CAT5/6).

4. Hardware Connections:
	Connect USB debug ports of the two RA boards to USB ports of the host PC using two appropriate USB cables for
	EP programming and debugging.

	For EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
		Connect the ethernet port of the RA board to the ethernet port of another RA board running
		NetX_dhcpv6_server using a LAN cable.

	For MCK-RA8T1:
		Connect the ethernet port of the RA board to the ethernet port of another RA board running
		NetX_dhcpv6_server using a LAN cable.

	For EK-RA8P1, EK-RA8D2, EK-RA8M2, EK-RA8P1 V2:
		Connect the ethernet port of the RA board to the ethernet port of another RA board running
		NetX_dhcpv6_server using a LAN cable.

	For MCK-RA8T2:
		Connect the ethernet port 0 (CN14) of the RA board to the ethernet port of another RA board running
		NetX_dhcpv6_server using a LAN cable.

	For EK-RA8T2:
		Connect the ethernet port 0 (J15) of the RA board to the ethernet port of another RA board running
		NetX_dhcpv6_server using a LAN cable.

5. Hardware Configuration:
	For EK-RA8D1:
		Set the configuration switches (SW1) as below to avoid potential failures:
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |      OFF     |     OFF    |     ON     |     OFF    |     OFF     |     OFF   |
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

	For EK-RA8P1 V2:
		The user must set the configuration switches (SW6) as below to use the on-board debug
		functionality.
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+

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

3) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC
   peripherals. Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
