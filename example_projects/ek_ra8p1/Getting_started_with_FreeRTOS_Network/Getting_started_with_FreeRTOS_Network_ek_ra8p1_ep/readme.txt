/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about the Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates FreeRTOS-Plus-TCP's networking capabilities using the Ethernet interface
	of the RA MCU. It utilizes the FreeRTOS-Plus-TCP stack to perform various network operations, including
	sending and receiving data over TCP and UDP protocols. The RA board will automatically obtain an IP address
	from the network using a DHCP (Dynamic Host Configuration Protocol) client service.

	Once the RA board successfully acquires the IP address, users can interact with the system by selecting options
	from a menu, which is accessible via the terminal. After the RA board retrieves its IP address from the router,
	it will display the network configuration details, along with the available menu options, in the terminal. This
	enables the user to explore different features and operations supported by the network stack.

	Key functionalities in this example include:
	- TCP Client Service: Establish a connection with the server, send a request, wait for the server's response,
	  and disconnect upon completion.
	- UDP Client Service: Send a message to a server and listen for the response.
	- DNS Client Service: Perform a DNS query based on a domain name specified by the user.
	- Web HTTP Server: Process HTTP requests from a browser, allowing users to access the homepage, view board
	  network configurations, control onboard LEDs, and check their status.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application
	Socket Application (e.g., sokit version 1.3)
	Browser Application (e.g., Microsoft Edge, Google Chrome)

3. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, EK-RA8P1, EK-RA8D2,
			     EK-RA8M2, EK-RA8T2, EK-RA8P1 V2.
	1 x Renesas RA board.
	1 x Ethernet router with an internet connection.
	2 x Ethernet cables to connect the RA board and the host PC to the router.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

4. Hardware Connections:
	- Power on the Router: Ensure the router is powered on and properly connected to the internet. Verify that
	  the router’s LAN ports are active and available for connections.
	- Connect the RA Board to the Router: Take an Ethernet cable and connect one end to the Ethernet port on
	  the RA board, and the other end to one of the router's available LAN ports. This connection allows the RA
	  board to communicate with the network and obtain an IP address via DHCP.
	- Connect the host PC to the Router: Using a second Ethernet cable, connect the host PC to another LAN port on
	  the same router. This step ensures that both the RA board and the host PC are on the same network, enabling
	  communication between them for debugging and menu operations.
	- Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming
	  and debugging. This connection is necessary for programming the RA board, enabling debugging, and displaying
	  runtime information in the terminal.
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

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29
		on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.

	For EK-RA8T2:
		The user must set the configuration switches (SW6 and SW4) as below to use the on-board debug
		functionality and enable Ethernet 0.
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+

		+-------+-------+-------+-------+-------+-------+-------+-------+
		| SW4-1 | SW4-2 | SW4-3 | SW4-4 | SW4-5 | SW4-6 | SW4-7 | SW4-8 |
		+-------+-------+-------+-------+-------+-------+-------+-------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |
		+-------+-------+-------+-------+-------+-------+-------+-------+

	For EK-RA8P1 V2:
		The user must set the configuration switches (SW6 and SW4) as below to use the on-board
		debug functionality and enable OSPI.
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
		+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+

		+-------+-------+-------+-------+-------+-------+-------+-------+
		| SW4-1 | SW4-2 | SW4-3 | SW4-4 | SW4-5 | SW4-6 | SW4-7 | SW4-8 |
		+-------+-------+-------+-------+-------+-------+-------+-------+
		|  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |
		+-------+-------+-------+-------+-------+-------+-------+-------+

Note:
1) Keep the project path short to prevent errors during the build process.

2) For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of
   Getting_started_with_FreeRTOS_Network_notes.md.
 
3) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
	a. To make sure the received data shows properly, the user needs to set:
		[Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR].

	b. To echo back characters typed in Tera Term, the user needs to enable it through:
		[Setup] -> [Terminal...] -> Check [Local echo].

	c. The configuration parameters of the serial port on the serial terminal application:
		Port: The port is provided by the J-Link On-Board.
		Speed: 115200
		Data: 8 bit
		Parity: none
		Stop bits: 1 bit
		Flow control: none
