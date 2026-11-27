/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the functionality of Wi-Fi OnChip Silex on Renesas RA MCUs using Renesas
	Silex UART Pmod. The user can either scan Wi-Fi networks or manually enter network credentials. After
	connecting, the EP performs DNS lookup and ping operations on the user entered URL or IP address. It also
	demonstrates TCP client socket communication by connecting to the user entered TCP server IP address and
	port number. Once connected, the user can send 'on' or 'off' commands to control the on-board LED. The LED
	state is displayed on the terminal, and acknowledgment messages are sent back to the server. Invalid commands
	do not affect the current LED state.

	Note: EK-RA2E2 does not support scanning Wi-Fi Access Points.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application
	Note: TCP server on PC can be created using freely available tool sokit:
	      https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml

3. Hardware Requirements:
	Supported RA boards: EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4L1, EK-RA4M2,
			     EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2,
			     EK-RA8M1, EK-RA8M2, EK-RA8P1, EK-RA8T2, FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA4E1,
			     FPB-RA6E1, FPB-RA8E1, MCK-RA6T2, MCK-RA8T1, FPB-RA6E2.
	1 x Renesas RA board.
	1 x Renesas Silex UART Pmod.
	1 x Wi-Fi router Access Point (AP) with internet access.
		Note: Please ensure the Wi-Fi Access Point credentials are available before running the project.
	1 x PC or laptop connected to the same Wi-Fi router Access Point.
		Note: Create TCP server and start listening on a free port.
		      Port of value greater than 10,000 is recommended.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA2E1, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA2L2:
		Connect Renesas Silex UART Pmod ----> PMOD1 (J26)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

	For EK-RA2L1, FPB-RA4E1, FPB-RA6E1, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8E2, EK-RA8D2, EK-RA8M2, EK-RA8T2:
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on
		              pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		For EK-RA8T2: The user must set the configuration switches (SW6 and SW4) as below to use the on-board
		              debug functionality.
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
		Connect Renesas Silex UART Pmod ----> PMOD2 (J25)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD2.

	For EK-RA2E2:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P400
		2) Renesas Silex UART Pmod (CN1:3) ----> P401
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc
		5) Renesas Silex UART Pmod (CN1:7) ----> P111
		6) Renesas Silex UART Pmod (CN1:8) ----> P015

	For MCK-RA6T2:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P605
		2) Renesas Silex UART Pmod (CN1:3) ----> P606
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc
		5) Renesas Silex UART Pmod (CN1:7) ----> P300
		6) Renesas Silex UART Pmod (CN1:8) ----> P302

	For MCK-RA8T1:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P112 (CN2:P1)
		2) Renesas Silex UART Pmod (CN1:3) ----> P113 (CN1:P33)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc
		5) Renesas Silex UART Pmod (CN1:8) ----> P114 (CN1:P31)

	For EK-RA2A2:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P501 (J1:1)
		2) Renesas Silex UART Pmod (CN1:3) ----> P500 (J4:19)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc
		5) Renesas Silex UART Pmod (CN1:8) ----> P203 (J3:9)

	For FPB-RA0E1, FPB-RA8E1, FPB-RA6E2:
		For FPB-RA6E2: The user must close E6 and open E5 to use PMOD1 for UART.
		Connect Renesas Silex UART Pmod ----> PMOD1
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

	For EK-RA4L1:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P109 (J3:19)
		2) Renesas Silex UART Pmod (CN1:3) ----> P110 (J3:20)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J3:25)
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc  (J3:1)
		5) Renesas Silex UART Pmod (CN1:8) ----> P306 (J4:11)

	For FPB-RA0E2:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P402 (J1:3)
		2) Renesas Silex UART Pmod (CN1:3) ----> P403 (J1:4)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J1:8)
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc  (J1:11)
		5) Renesas Silex UART Pmod (CN1:8) ----> P208 (J1:20)

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		The user must close E52 to use P305 for RST pin of PMOD2.
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P115 (J2:35)
		2) Renesas Silex UART Pmod (CN1:3) ----> P608 (J2:37)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J4:25)
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc  (J4:1)
		5) Renesas Silex UART Pmod (CN1:8) ----> P305 (J2:38)

	For FPB-RA0L1:
		To use the on-board debug functionality, the user must place jumper J9 on pins 1-2.
		To use P101, P100, P013 on PMOD1, the user must close E34, E36, E35 and open E40.
		Connect Renesas Silex UART Pmod ----> PMOD1 (J19)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

Note:
1) Ensure that the Wi-Fi Access Point credentials are available before running the project.

2) Before testing, please confirm that the host PC has enabled the inbound rule 'File and Printer Sharing
   (Echo Request - ICMPv4-In)' to accept ping request from RA MCU to TCP server: Windows Defender Firewall
   -> Advanced settings -> Inbound Rules -> Enable File and Printer Sharing (Echo Request - ICMPv4-In).

3) Sokit setup on PC as TCP server:
	a. Download sokit tool from https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml
	b. Once downloaded, extract the contents and start sokit.exe.
	c. Once started, in the 'Server' tab configure the 'Network Setup'.
	d. In the 'TCP Addr' field, enter the PC's IPv4 address. And in 'Port' field, enter the port number.
		Note: Create TCP server and start listening on a free port. 
		      Port of value greater than 10,000 is recommended.
	e. Then click the 'TCP Listen' button. This starts the TCP server on PC.
	f. The 'Output' window displays the message 'MSG start TCP server successfully!'.
	g. Now wait for the EP to connect to server.
	h. Once the EP connects to the server, the 'Connections' window displays the IP address and port number
	   of the client.
	i. There are three user input buffers provided for sending messages i.e. 'Buf 1', 'Buf 2' and 'Buf 3'.
	k. In the first buffer, enter 'on'. In the second buffer, enter 'off'. In the third buffer, enter any invalid text.
	l. While sending, select the IP address of the client i.e. the MCU from the 'Connections' GroupBox. And then
	   click the 'Send' button of the appropriate buffer to send message.
	m. Message received from the client are displayed on the 'Output' window.
	n. The 'Output' window and console display messages upon each transaction.
	o. To stop the server, click the 'TCP Listen' button again. This stops the server.
	   Note: Even if the server is stopped in sokit, the MCU may still be connected due to internal network connection.

4) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
