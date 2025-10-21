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
	   Availability of free address in the LAN.

	Note: The user is expected to enter the static IP address according to the network settings of the LAN.
	      In case of DHCP, the user just needs to run the application and DHCP client will communicate to the
	      DHCP server and gets the IP address. After obtaining the IP address and DNS lookup for the domain name
	      requested, please wait for pinging to the requested IP address then the response information will be
	      displayed in the RTT Viewer. Following is example of successful response in RTT Viewer:
	      Ping Statistics for 8.8.8.8:
	      Packets: Sent = 100, Received = 99, Lost = 00

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x LAN cable.
	1 x Ethernet switch.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

	For EK-RA6M3, EK-RA6M4, EK-RA6M5, MCK-RA8T1, EK-RA8P1, EK-RA8M2:
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
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
		Connect LAN cable on RA board at ethernet port 0 (CN14) and other end connect to ethernet switch/router.

4. Configuration Settings:
	- While running the application using DHCP or Static Address mode, the following settings need to be done
	  in the configurator.
	- The same project can be used for both modes.
	- By default, this example project runs in DHCP mode.

	|----------------------------------------------------------------------------|
	| FreeRTOS+TCP                                   Static IP       Dynamic IP  |
	|============================================================================|
	| Use DHCP                                       Disable         Enable      |
	|----------------------------------------------------------------------------|
	| DHCP Register Hostname                         Disable         Enable      |
	|----------------------------------------------------------------------------|
	| DHCP Uses Unicast                              Disable         Enable      |
	|----------------------------------------------------------------------------|
	| DHCP callback Function                         Disable         Enable      |
	|----------------------------------------------------------------------------|

Note:
1) For IAR, please initialize device back to factory default using the Renesas Device Partition Manager tool to
   avoid warnings related to incorrect alignment of Stack pointer.

2) For Keil, please note that a licensed version of Keil is required to build this project.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220011b0
   b. Keil:     0x2200841c
   c. IAR:	0x2201b074

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
