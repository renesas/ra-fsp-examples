/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of an SNMP agent running on Azure RTOS with Renesas RA MCUs
	using the Renesas Flexible Software Package (FSP). The SNMP agent is configured to respond to SNMP manager
	commands and to send event-driven traps. When any key is pressed in the J-Link RTT Viewer, the agent sends an
	SNMP trapv2 event to the manager. Application status is displayed in the J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8M2.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	2 x Ethernet/LAN cable (Ethernet cable CAT5/6).
	1 x Ethernet switch.
	1 x Linux machine, Raspberry Pi, or Windows PC running Ubuntu and WSL system to run the SNMP Manager.

3. Hardware Connections:
	For EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
		Connect USB debug port of the RA board to the host machine via a micro USB cable.
		Connect ethernet port of the RA board to ethernet switch/router via a LAN cable.
		Connect ethernet port of the Linux machine/Raspberry Pi/Windows PC to ethernet switch/router
		via a LAN cable.

	For MCK-RA8T1, EK-RA8M2:
		Connect USB debug port of the RA board to the host machine via a Type-C USB cable.
		Connect ethernet port of the RA board to ethernet switch/router via a LAN cable.
		Connect ethernet port of the Linux machine/Raspberry Pi/Windows PC to ethernet switch/router
		via a LAN cable.

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

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200068c
   b. Keil:	Not Available
   c. IAR:	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

3) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC
   peripherals. Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
