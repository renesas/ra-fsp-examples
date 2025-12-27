/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of NetX Duo DHCPv4 server with an ethernet driver on Renesas 
	RA MCUs based on Renesas FSP using AzureRTOS. DHCPv4 Server is created and based on user input, the user can
	start, stop and print DHCPv4 Client info. On successful completion of each operation, the success status is
	displayed on the RTT Viewer. Error and info messages will be printed on J-Link RTT Viewer.
  	
2. Hardware Requirement:
	Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8D2, EK-RA8M2
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 × PC supported ethernet or another RA board running DHCPv4 Client.
	Without ethernet switch:
		1 x Ethernet/LAN cable (Ethernet cable CAT5/6).
	With ethernet switch: (Optional)
		1 x Ethernet switch.
		2 x Ethernet/LAN cables (Ethernet cable CAT5/6).
		Note: The ethernet switch should not have Internet access; it only serves as a connection hub.

3. Hardware Connections:
	For EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
		Connect the RA board USB device port to the host machine via a micro USB cable.
		Without ethernet switch:
			Connect the ethernet port of the RA board to the ethernet port of the PC/another RA board
			running DHCPv4 Client using a LAN cable.
		With ethernet switch: (Optional)
			Connect the ethernet port of the RA board and the ethernet port of the PC/another RA board
			running DHCPv4 Client to an ethernet switch using two LAN cables.

	For MCK-RA8T1, EK-RA8D2, EK-RA8M2:
		Connect the RA board USB device port to the host machine via a Type-C USB cable.
		Without ethernet switch:
			Connect the ethernet port of the RA board to the ethernet port of the PC/another RA board
			running DHCPv4 Client using a LAN cable.
		With ethernet switch: (Optional)
			Connect the ethernet port of the RA board and the ethernet port of the PC/another RA board
			running DHCPv4 Client to an ethernet switch using two LAN cables.

4. Hardware Configuration:
	For EK-RA8D1:
		Set the configuration switches (SW1) as below to avoid potential failures.
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

Note:
1) The user should wait for message "Ethernet link is up." on RTT viewer for ethernet/network link up status case.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200058c
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
