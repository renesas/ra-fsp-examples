/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of NetX Duo FTP server with an ethernet driver on Renesas
	RA MCUs based on Renesas FSP using AzureRTOS. FTP server is created and based on user input, user can use
	FTP client machine for communication with the created server.
	On successful completion of each operation, the success status is displayed
	on the RTT Viewer. Error and info messages will be printed on J-link RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	SEGGER J-Link RTT Viewer: Version 9.42
	LLVM Embedded Toolchain for ARM: Version 21.1.1

3. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8D2.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	2 x Ethernet/LAN cables (Ethernet cable CAT5/6).
	1 x Ethernet switch.
	1 x Linux machine, Raspberry Pi, or Windows PC to run as FTP client.

4. Hardware Connections:
	For EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
		Connect USB debug port of the RA board to the host machine via a micro USB cable.
		Connect ethernet port of the RA board to ethernet switch/router via a LAN cable.
		Connect ethernet port of the Linux machine/Raspberry Pi/Windows PC to ethernet switch/router
		via a LAN cable.

	For EK-RA8D2:
		Connect USB debug port of the RA board to the host machine via a Type-C USB cable.
		Connect ethernet port (J15) of the RA board to ethernet switch/router via a LAN cable.
		Connect ethernet port of the Linux machine/Raspberry Pi/Windows PC to ethernet switch/router
		via a LAN cable.

5. Hardware Configurations:
	For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     ON     |     OFF    |     OFF     |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	CAUTION: Do not enable SW1-4 and SW1-5 together.

	For EK-RA8M1: Remove jumper J61 to enable Ethernet B.

	For EK-RA8D2: Set the configuration switches (SW4) as below to avoid potential failures.
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
	| SW4-1 PMOD1 | SW4-2 PMOD1 | SW4-3 Octo-SPI | SW4-4 Arduino/mikroBUS | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS | SW4-8 USBHS |
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
	|     OFF     |     OFF     |      OFF       |          OFF           |    OFF    |    OFF     |     OFF     |     OFF     |
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+

Note:
1) The project is expected that user should use necessary network set up and user need to run FTP client by
   passing FTP server IP address which is set in configurator property of g_ip0_Netx Duo IP instance ->
   IPv4 Address as per their network environment.
   FTP server IP: if it's a home/close network user can use 192.168.0.x where x represent any available IP
		  address on network.
   FTP client   : For FTP client use PC command prompt with default FTP utility(available in both windows and Linux)
                  which will act as FTP client and the PC should be running in the same network environment.
   For establishing connection with FTP server using FTP client, user should wait for message
   "FTP server started and ready to use !!" on RTT Viewer.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000770
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the build configuration folder (e2studio\Debug or e2studio\Release).

4) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC
   peripherals. Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
