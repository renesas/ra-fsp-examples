/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the use of the FreeRTOS + TCP Module with Ethernet. The example project
	demonstrates the usage of FreeRTOS DHCP Client, DNS Client and ICMP code. This example project has 2 modes
	for the user
	1) DHCP Mode, where the IP credentials are obtained from the DHCP Server on the Network.
	2) Static IP Address mode, where in the Static IP address can be configured for the Ethernet based on the
	   availability of free address in the LAN.

	Note: The user is expected to enter the static IP address according to the network settings of the LAN.
	      In case of DHCP, the user just needs to run the application and DHCP client will communicate to the
	      DHCP server and gets the IP address. After obtaining the IP address and DNS lookup for the domain name
	      requested, please wait for pinging to the requested IP address then the response information will be
	      displayed in the RTT Viewer. Following is example of successful response in RTT Viewer:
	      Ping Statistics for 8.8.8.8:
	      Packets: Sent = 100, Received = 99, Lost = 00

2. Software Requirements:
	Refer to the "Tools" section in the FSP release notes to set up all the required software via the link below:
	https://github.com/renesas/fsp/releases

3. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x LAN cable.
	1 x Ethernet switch.

4. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

	For EK-RA6M3, EK-RA6M4, EK-RA6M5, MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2:
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on
		              pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect RA board ethernet port to the ethernet switch/router via a LAN cable.

	For EK-RA8D1:
		Set the configuration switches (SW1) as below to avoid potential failures.
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |      OFF    |      OFF     |     OFF    |     ON     |      OFF   |      OFF    |     OFF   |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		Connect RA board ethernet port to the ethernet switch/router via a LAN cable.
		CAUTION: Do not enable SW1-4 and SW1-5 together.

	For EK-RA8M1:
		Remove jumper J61 to enable Ethernet B.
		Connect RA board ethernet port to the ethernet switch/router via a LAN cable.

	For MCK-RA8T2:
		Connect RA board ethernet port 0 (CN14) to the ethernet switch/router via a LAN cable.

5. Configuration Settings:
	The project supports both Dynamic IP and Static IP modes.
	To switch mode in configuration:
		Setting for FreeRTOS+TCP:
			i. Dynamic IP (Default):
				Use DHCP               : Enable
				DHCP Register Hostname : Enable
				DHCP Uses Unicast      : Enable
				DHCP callback function : Enable

			ii. Static IP:
				Use DHCP               : Disable
				DHCP Register Hostname : Disable
				DHCP Uses Unicast      : Disable
				DHCP callback function : Disable
				Note:
				Please make sure to check and update the values of the ucIPAddress, ucNetMask,
				ucGatewayAddress, and ucDNSServerAddress variables in ...\src\net_thread_entry.c
				file to match the network environment currently used by the project.

Note:
1) For IAR, please initialize device back to factory default using the Renesas Device Partition Manager tool to
   avoid warnings related to incorrect alignment of Stack pointer.

2) For Keil, please note that a licensed version of Keil is required to build this project and ensure that the
   DLL version matches the required J-Link version before debugging the project. Open project, connect the RA board to
   the host PC, and the DLL version can be found at:
	Project:'Project_name' -> Target_1 -> Options for ('Project_name') Target 'Target_1'... -> Debug ->
	Settings (J-LINK/J-TRACE Cortex) -> Debug -> J-Link/J-Trace Adapter -> dll:

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x220011ac
   b. Keil:	0x2200841c
   c. IAR:	0x2201b284

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings,
   iar\Debug\List).
