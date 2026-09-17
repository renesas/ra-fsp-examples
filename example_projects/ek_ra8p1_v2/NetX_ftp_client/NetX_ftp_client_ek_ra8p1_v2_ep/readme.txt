/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of NetX Duo FTP Client with an ethernet driver on Renesas
	RA MCUs based on Renesas FSP using AzureRTOS. The example project configures the FTP Client to establish
	the connection with FTP Server. The FTP Client will perform read and write operations, on to the currently
	opened file on the FTP Server. The status message and other relevant data will be displayed on J-Link
	RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2,
			     MCK-RA8T2, EK-RA8P1 V2.
	1 x Renesas RA board.
	1 x Host PC.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	2 x Ethernet/LAN cables (Ethernet cable CAT5/6).
	1 x Ethernet switch.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming
	and debugging.
	Connect the Ethernet switch to the Ethernet port of the device running the FTP server using a LAN cable.
	Connect the RA board Ethernet port to the Ethernet switch using another LAN cable.
	Note: MCK-RA8T2 uses Ethernet port 0 (CN14) by default.

4. Software Requirements:
 	The user needs to run an FTP Server preferably on a Windows or Linux host PC.

5. Hardware Configurations:
	For EK-RA8D1:
		The user must set the configuration switches (SW1) as below to avoid potential failures:
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |      OFF     |     OFF    |     ON     |     OFF    |     OFF     |     OFF   |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

		CAUTION: Do not enable SW1-4 and SW1-5 together.

	For EK-RA8M1:
		The user must remove jumper J61 to enable Ethernet B.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.

	For EK-RA8P1 V2:
		The user must set the configuration switches (SW6) as below to use the on-board debug
		functionality.
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+

Note:
1) Current IP addresses are provided according to our test environment. The user has to change the IP addresses in IP
   instance according to their network configuration property of g_ip0_Netx Duo IP instance -> IPv4 Address as per their
   network environment.
   FTP Server IP: If it's a home/close network, the user can use 192.168.0.x where x represent any available IP address
   on network.

2) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC
   peripherals. Consequentially, it is necessary to connect the serial programming interface to meet this requirement.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220011cc
   b. Keil:	Not Available
   c. IAR:	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
