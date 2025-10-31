/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of NetX Duo DHCPv6 server with an ethernet driver on Renesas
	RA MCUs based on Renesas FSP using AzureRTOS. DHCPv6 server is created and based on user input, the user can
	start, and print DHCPv6 client info. On successful completion of each operation, the success status is
	displayed on the RTT Viewer. Error and info messages will be printed on J-Link RTT Viewer.

2. Hardware Requirement:
 	Supported Board EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1, EK-RA8P1:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Ethernet/LAN cable (Ethernet cable CAT5/6).

3. Hardware Connections:
	For EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
		Connect the RA board USB device port to the host machine via a micro USB cable.
		Connect the Ethernet port on RA board to the Ethernet port of PC/another RA board on which DHCPv6 Client
		is residing via a LAN cable.

	For MCK-RA8T1, EK-RA8P1:
		Connect the RA board USB device port to the host machine via a Type-C USB cable.
		Connect the Ethernet port on RA board to the Ethernet port of PC/another RA board on which DHCPv6 Client
		is residing via a LAN cable.

4. Hardware Configuration:
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

Note:
1) The user should wait for "Ethernet link is up." message on RTT Viewer for the ethernet/network link up status case.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x220005b0
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the build configuration folder (Debug/Release).
