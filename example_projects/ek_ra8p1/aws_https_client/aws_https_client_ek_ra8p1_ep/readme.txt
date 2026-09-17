/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of AWS https client library with ethernet driver on Renesas
	RA MCUs based on Renesas FSP using FreeRTOS. AWS client is used to connect to https adafruit server which is
	cloud platform. On successful connection, menu is displayed enabling user to send GET, PUT, POST requests to
	adafruit server. On POST/PUT request, MCU Die temperature is read via (using) ADC and uploaded to server.
	On GET request, the last MCU die temperature data is read from the adafruit server. J-Link RTT Viewer is
	used to display the status and responses of the requests made to server.

	Note: After the initial successful connection, the user should run the POST request first and then the GET
	      request prior to running the PUT request.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x LAN cable.
	1 x Ethernet switch.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

	Connect RA board ethernet port to the ethernet switch/router via a LAN cable.
	Note: The Switch should have WAN connection to communicate the server over internet and connect to the router
	      which is connected to the Internet.

4. Hardware Configurations:
	- For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |      OFF    |      OFF     |     OFF    |     ON     |      OFF   |      OFF    |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	CAUTION: Do not enable SW1-4 and SW1-5 together.

	- For EK-RA8M1: Remove jumper J61 to enable Ethernet B

	- For EK-RA8P1: Set the configuration switches (SW4) as below to avoid potential failures.
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	| SW4-1 PMOD1_SEL0 | SW4-2 PMOD1_SEL1 |    SW4-3 OSPI   |   SW4-4 ARDUINO  |    SW4-5 I3C   |    SW4-6 MIPI  | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	|       OFF        |       OFF        |       OFF       |       OFF        |      OFF       |      OFF       |       OFF        |       OFF        |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+

Note:
1) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC
   peripherals. Consequentially, it is necessary to connect the serial programming interface to meet this requirement.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200346c
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
