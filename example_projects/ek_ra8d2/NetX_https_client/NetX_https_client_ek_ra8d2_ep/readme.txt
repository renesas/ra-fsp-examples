/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of NetX Duo HTTPS Client with ethernet driver on Renesas
	RA MCUs based on Renesas FSP using AzureRTOS. The EP uses the DHCP Client to get the IP address from the
	DHCP Server and HTTPS Client to connect to HTTPS Adafruit IO Server which is a cloud platform. On successful
	connection, the menu is displayed enabling the user to send POST, PUT, GET requests to Adafruit IO server.
	On POST/PUT request, MCU die temperature is read using ADC and uploaded to server. On GET request, the last
	MCU die temperature is read from the Adafruit IO server. Error and info messages will be printed on
	J-Link RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	SEGGER J-Link RTT Viewer: Version 9.42
	LLVM Embedded Toolchain for ARM: Version 21.1.1

3. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8D2.
	1 x Renesas RA board.
	1 x Host PC.
	1 x USB cable for programming and debugging.
	1 x Ethernet/LAN cable (Ethernet cable CAT5/6).
	1 x Ethernet switch.

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC via a USB cable.
	Connect the RA board ethernet port to the ethernet switch via a LAN cable. Ensure that the ethernet switch
	is connected to a network with Internet access.

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

Note:
1) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC
   peripherals. Consequentially, it is necessary to connect the serial programming interface to meet this requirement.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22001a8c
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
