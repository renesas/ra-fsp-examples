/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about the Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates FreeRTOS-Plus-TCP's networking capabilities using the Ethernet interface of
	the RA MCU. It utilizes the FreeRTOS-Plus-TCP stack to perform various network operations, including sending
	and receiving data over TCP and UDP protocols. The RA board will automatically obtain an IP address from the 
	network using a DHCP (Dynamic Host Configuration Protocol) client service.

	Once the RA board successfully acquires the IP address, users can interact with the system by selecting options
	from a menu, which is accessible via the RTT Viewer or a terminal application running on the host PC. After
	the RA board retrieves its IP address from the router, it will display the network configuration details,
	along with the available menu options, in the terminal application. This enables the user to explore different
	features and operations supported by the network stack.

	Key functionalities in this example include:
	- TCP Client Service: Establish a connection with the server, send a request, wait for the server's response,
	  and disconnect upon completion.
	- UDP Client Service: Send a message to a server and listen for the response.
	- DNS Client Service: Perform a DNS query based on a domain name specified by the user.
	- Web HTTP Server: Process HTTP requests from a browser, allowing users to access the homepage, view board
	  network configurations, control onboard LEDs, and check their status.

	Note:
	- Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via
	  J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to
	  the SEGGER J-Link RTT Viewer; if supported, it defaults to the serial terminal (UART).
	- To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please refer to the instructions provided
	  in the 'Special Topic' section of Getting_started_with_FreeRTOS_Network_notes.md.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.1.0
	e2 studio: Version 2025-07
	SEGGER J-Link RTT Viewer: Version 8.58
	LLVM Embedded Toolchain for ARM: Version 18.1.3
	Terminal Console Application: Tera Term or a similar application
	Socket Application: sokit version 1.3, etc.
	Browser Application: Microsoft Edge, Google Chrome, etc.

3. Hardware Requirements:
	- Supported RA boards: EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, EK-RA8P1
	- 1 x Ethernet router with an internet connection.
	- 2 x Ethernet cables to connect the RA board and the host PC to the router.
	- 1 x Type-C USB cable for programming, debugging, and status display on the terminal (Tera Term or
	      SEGGER RTT J-Link Viewer).

4. Hardware Connections:
	- Power on the Router: Ensure the router is powered on and properly connected to the internet. Verify that
	  the router’s LAN ports are active and available for connections.
	- Connect the RA Board to the Router: Take an Ethernet cable and connect one end to the Ethernet port on
	  the RA board, and the other end to one of the router's available LAN ports. This connection allows the RA
	  board to communicate with the network and obtain an IP address via DHCP.
	- Connect the Host PC to the Router: Using a second Ethernet cable, connect the host PC to another LAN port on
	  the same router. This step ensures that both the RA board and the host PC are on the same network, enabling
	  communication between them for debugging and menu operations.
	- Connect the RA Board to the Host PC: Using a Type-C USB cable, connect the RA board to the host PC.
	  This connection is necessary for programming the RA board, enabling debugging, and displaying runtime
	  information in the terminal or RTT Viewer on the host PC.
	- By following these steps, you'll establish proper hardware connections between the RA board, the router, and
	  the host PC, ensuring smooth network communication and debugging capabilities.

	For EK-RA8D1:
		Set the configuration switches (SW1) as below.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     ON     |     OFF    |     OFF     |    OFF    |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

	For EK-RA8M1:
		Remove jumper J61 to enable Ethernet B.

	For EK-RA8P1:
		To guarantee reliable Ethernet connectivity, a switch or router compliant with
		IEEE 802.3ab (1000BASE-T, 1000 Mbps) is required.

Note:
1) For the Serial Terminal application: The macro USE_VIRTUAL_COM is set to 1.
   1.1 Enable echo in Tera Term: Setup → Terminal… → check Local echo.
   1.2 Serial port settings:
       - COM port: J-Link on-board
       - Baud rate: 115200 bps
       - Data length: 8 bits
       - Parity: None
       - Stop bit: 1

2) For using the J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

3) Keep the project path short to prevent errors during the build process.

4) For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of
   Getting_started_with_FreeRTOS_Network_notes.md.
