/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of Netx Duo DHCPv6 client with an ethernet driver on Renesas
	RA MCUs based on Renesas FSP using AzureRTOS. DHCP client is created and started. On successful receiving of
	IP address from DHCPv6 server, it pings the server to verify the received IP address. On successful completion,
	the success status and IP address are displayed on the RTT Viewer. The error and info messages will be printed
	on RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	SEGGER J-Link RTT Viewer: Version 9.42
	LLVM Embedded Toolchain for ARM: Version 21.1.1

3. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2,
	                     MCK-RA8T2, EK-RA8T2
	2 x Renesas RA boards.
		- 1 x RA board runs the NetX_dhcpv6_server example project.
		- 1 x RA board runs the NetX_dhcpv6_client example project.
	2 x Type-C USB cables for programming and debugging.
	1 x Ethernet/LAN cable (Ethernet cable CAT5/6).

4. Hardware Connections:
	For EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
		Connect the RA board USB debug port (J10) to the host machine via a micro USB cable.
		Connect the ethernet port of the RA board to the ethernet port of another RA board running 
		NetX_dhcpv6_server using a LAN cable.

	For MCK-RA8T1:
		Connect the RA board USB debug port (CN11) to the host machine via a Type-C USB cable.
		Connect the ethernet port of the RA board to the ethernet port of another RA board running 
		NetX_dhcpv6_server using a LAN cable.

	For EK-RA8P1, EK-RA8D2, EK-RA8M2:
		Connect the RA board USB debug port (J10) to the host machine via a Type-C USB cable.
		Connect the ethernet port of the RA board to the ethernet port of another RA board running 
		NetX_dhcpv6_server using a LAN cable.

	For MCK-RA8T2:
		Connect the RA board USB debug port (CN13) to the host machine via a Type-C USB cable.
		Connect the ethernet port 0 (CN14) of the RA board to the ethernet port of another RA board running 
		NetX_dhcpv6_server using a LAN cable.

	For EK-RA8T2:
		Connect the RA board USB debug port (J10) to the host machine via a Type-C USB cable.
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

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x22001190
   b. Keil:	Not Available
   c. IAR:	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

3) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC
   peripherals. Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
