/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about the Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates Azure NetX Duo's networking capabilities using the Ethernet physical layer
	of the RA MCU. It leverages the NetX Duo network stack to execute various network operations, such as sending
	and receiving data over a network. The RA board will automatically obtain an IP address from the network using
	a DHCP (Dynamic Host Configuration Protocol) client service.

	Once the RA board successfully acquires the IP address, the user can interact with the system by selecting
	options from a menu, which is accessible via the RTT Viewer or a terminal application running on the host PC.
	After the RA board retrieves its IP address from the router, it will display the network configuration details,
	along with the available menu options, in the terminal application. This enables the user to explore different
	features and operations supported by the network stack.

	The available options include:
	- TCP Client Service: Connect to a server, send a request, wait for a response from the server, and disconnect
	  from the server.
	- UDP Client Service: Send a request and wait for a response from the server.
	- DNS Client Service: Retrieve the A record and PTR record.
	- Web HTTP Server: Handle HTTP requests from the browser, including requesting the homepage, retrieving board
	  network configuration, controlling onboard LEDs, and requesting the status of onboard LEDs.

	Note:
	- To display information, the user can select between the SEGGER J-Link RTT Viewer and the serial terminal
	  (UART) with J-Link OB VCOM.
	- By default, the EP information is printed to the host PC via the serial terminal.
	- RA boards supported for J-Link OB VCOM: EK-RA8D1, EK-RA8M1, EK-RA8P1.
	- To use the SEGGER J-Link RTT Viewer instead of the serial terminal, please follow the instructions in the
	  Verifying Operation section.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application
	SEGGER J-Link RTT Viewer: Version 9.42
	Socket Application (e.g., sokit version 1.3)
	Browser Application (e.g., Microsoft Edge, Google Chrome)

3. Hardware Requirements:
	Supported RA boards: EK-RA8D1, EK-RA8M1, EK-RA8P1.
	1 x Renesas RA board.
	1 x Ethernet router with an internet connection.
	2 x Ethernet cables to connect the RA board and the host PC to the router.
	1 x Type-C USB cable for programming and debugging.

4. Hardware Connections:
	- Power on the Router: Ensure the router is powered on and properly connected to the internet. Verify that the
	  router’s LAN ports are active and available for connections.
	- Connect the RA board to the Router: Take an Ethernet cable and connect one end to the Ethernet port on the
	  RA board, and the other end to one of the router's available LAN ports. This connection allows the RA board
	  to communicate with the network and obtain an IP address via DHCP.
	- Connect the host PC to the Router: Using a second Ethernet cable, connect the host PC to another LAN port on
	  the same router. This step ensures that both the RA board and the host PC are on the same network, enabling
	  communication between them for debugging and menu operations.
	- Connect the RA board to the host PC: Using a USB cable, connect the RA board to the host PC. This
	  connection is necessary for programming the RA board, enabling debugging, and displaying runtime information
	  in the terminal or RTT Viewer on the host PC.
	- By following these steps, you'll establish proper hardware connections between the RA board, the router,
	  and the host PC, ensuring smooth network communication and debugging capabilities.

	For EK-RA8D1: Set the configuration switches (SW1) as below.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     ON     |     OFF    |     OFF     |    OFF    |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

	For EK-RA8M1: Remove jumper J61 to enable Ethernet B.

	For EK-RA8P1: Set the configuration switches (SW4) as below to avoid potential failures.
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	| SW4-1 PMOD1_SEL0 | SW4-2 PMOD1_SEL1 |    SW4-3 OSPI   |   SW4-4 ARDUINO  |    SW4-5 I3C   |    SW4-6 MIPI  | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	|       OFF        |       OFF        |       OFF       |       OFF        |      OFF       |      OFF       |       OFF        |       OFF        |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+

5. Verifying Operation:
	1. Import, generate, build, and flash the example project to the RA board using e2 studio on the host PC.
	   - By default, the EP supports serial terminal for RA boards that support J-Link OB VCOM.
	     Define USE_VIRTUAL_COM=1 macro in
	     Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes

	   - To use SEGGER J-Link RTT Viewer, please follow the instructions as below:
	     Define USE_VIRTUAL_COM=0 macro in
	     Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes
	2. Open the terminal application on the host PC and connect to the RA board.
	3. Wait for the RA board to obtain the IP address from the router.
	4. The RA board's network configuration details and menu options will be displayed in the terminal application.
	5. Verify TCP Client Operation:
		1. Launch a socket application on the host PC.
		2. Set up a TCP server by specifying the server's IP address and port number.
		3. Ensure that the server is configured to listen for incoming connections.
		4. In the terminal, type the command: '1 <x.x.x.x><port>'.
		   For example, if the server's IP is '192.168.1.12' and the port is '12345', type: '1 192.168.1.12:12345'.
		5. The RA board will send a TCP request to the server.
		6. The socket application will display the TCP request.
		7. In the socket application, the user should type any data and click Send promptly to respond to the RA board.
		   Note: If the user does not complete this step, a timeout error will occur (expected behavior).
		   The operation will then end and return to the menu so the user can retry.
		8. The RA board will display the received data in the terminal application.
	6. Verify UDP Client Operation:
		1. Launch a socket application on the host PC.
		2. Set up a UDP server by specifying the server's IP address and port number.
		3. Ensure that the server is configured to listen for incoming connections.
		4. In the terminal, type the command: '2 <x.x.x.x><port>'.
		   For example, if the server's IP is '192.168.1.12' and the port is '12345', type: '2 192.168.1.12:12345'.
		5. The RA board will send a UDP request to the server.
		6. The socket application will display the UDP request.
		7. In the socket application, the user should type any data and click Send promptly to respond to the RA board.
		   Note: If the user does not complete this step, a timeout error will occur (expected behavior).
		   The operation will then end and return to the menu so the user can retry.
		8. The RA board will display the received data in the terminal application.
	7. Verify DNS Client Operation:
		a. Request a PTR record for an IP address:
			1. In the terminal, type: '3 <x.x.x.x>'.
			   For example, if the IP address is '192.168.1.12', type: '3 192.168.1.12'.
			2. The RA board will receive and display the PTR record data in the terminal application.
		b. Request an A record for a domain:
			1. In the terminal, type: '3 <domain_name>'.
			   For example, if the domain name is 'renesas.com', type: '3 renesas.com'.
			2. The RA board will receive and display the A record data in the terminal application.
	8. Verify Web Server Operation:
		1. In the terminal, type '4' to start the web server.
		2. Open a web browser on the host PC.
		3. In the URL bar, type: '<x.x.x.x>/index.html' and press 'Enter', where '<x.x.x.x>' is the IP address
		   of the RA board.
		4. The browser will display the network configuration and LED control panel.
		5. In the LED control panel on the webpage, click on the SW1 or SW2 buttons to toggle LED1 or LED2 on
		   the RA board.
		6. Click on SW1 or SW2 on the RA board to toggle LED1 or LED2.
		7. The RA board will update the LED states dynamically on the webpage.
		8. In the terminal, type any character to stop the web server.

Note:
1) x.x.x.x is a placeholder for an IP address, which may vary depending on the environment,
   and the use of the same notation does not imply that the values are identical across different steps.

2) By default, the web.bin file containing the website content is flashed to OSPI during debugging. Ensure that it is
   also flashed when running the project using a hex file to avoid empty content in Option 4.

3) For using the serial terminal application:
	1. To echo back characters typed in Tera Term, the user needs to enable it through:
		[Setup] -> [Terminal...] -> Check [Local echo].
	2. The configuration parameters of the serial port are:
		- Port: Provided by the J-Link OB VCOM.
		- Speed: 115200
		- Data: 8 bit
		- Parity: none
		- Stop bits: 1 bit
		- Flow control: none

4) For using the J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
