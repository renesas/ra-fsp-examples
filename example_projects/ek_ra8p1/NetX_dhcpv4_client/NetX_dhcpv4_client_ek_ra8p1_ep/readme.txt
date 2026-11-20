/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionality of the NetX Duo DHCPv4 client with an Ethernet driver
	on Renesas RA MCUs based on Renesas FSP using Azure RTOS. The DHCP client obtains an IP address from a DHCPv4
	server and pings the server to verify network connectivity. It then restarts the DHCP client, obtains another
	IP address, pings the available DNS servers and gateways, and releases the IP address. Status information and
	error messages are displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: CK-RA6M5, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1, EK-RA8D2,
	                     EK-RA8M2, MCK-RA8T2, EK-RA8T2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Ethernet/LAN cable (Ethernet cable CAT5/6).
	1 x Ethernet switch/router.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For CK-RA6M5, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2:
		Connect the Ethernet port on the RA board to an Ethernet switch/router using an Ethernet/LAN cable.

	For MCK-RA8T2:
		Connect the Ethernet port 0 (CN14) on the RA board to an Ethernet switch/router using an
		Ethernet/LAN cable.

	For EK-RA8T2:
		Connect the Ethernet port 0 (J15) on the RA board to an Ethernet switch/router using an
		Ethernet/LAN cable.

4. Hardware Configuration:
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
 
		+-------+-------+-------+-------+-------+-------+-------+-------+
		| SW4-1 | SW4-2 | SW4-3 | SW4-4 | SW4-5 | SW4-6 | SW4-7 | SW4-8 |
		+-------+-------+-------+-------+-------+-------+-------+-------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |
		+-------+-------+-------+-------+-------+-------+-------+-------+

Note:
1) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC
   peripherals. Consequentially, it is necessary to connect the serial programming interface to meet this requirement.

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
