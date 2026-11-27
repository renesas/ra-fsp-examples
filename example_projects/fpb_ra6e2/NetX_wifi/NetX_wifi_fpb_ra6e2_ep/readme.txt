/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the functionalities of NetX Duo Wi-Fi Driver using Azure RTOS on Renesas
	RA MCUs using Renesas Silex UART Pmod. The project utilizes some functionalities of Wi-Fi Onchip Silex
	module which are not supported by NetX Duo. The user can either scan Wi-Fi networks or manually enter network
	credentials. After connecting, the EP performs DNS lookup and ping operations on the user entered URL or IP
	address. It also demonstrates TCP client socket communication by connecting to the user entered TCP server
	IP address and port number. Once connected, the user can send 'on' or 'off' commands to control the on-board
	LED. The LED state is displayed on the terminal, and acknowledgment messages are sent back to the server.
	Invalid commands do not affect the current LED state.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	Terminal Console Application: Tera Term or a similar application
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Sokit software is required for creation of TCP server over specified IP and port (detailed steps
        mentioned below)
	Download Link: https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA2L1, EK-RA4M1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5,
			     EK-RA4M3, FPB-RA4E1, FPB-RA6E1, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1,
			     EK-RA8D1, MCK-RA8T1, EK-RA2A2, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA8P1, EK-RA4C1,
			     MCK-RA8T2, EK-RA8D2, EK-RA8M2, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Renesas Silex UART Pmod (P/N: RTK00WFMX0B01000BE).
	1 x Wi-Fi router Access Point (AP) with internet access.
		Note: Please ensure the Wi-Fi Access Point credentials are available before running the project.
	1 x PC or laptop connected to the same Wi-Fi router AP.
		Note:
		Create TCP server and start listening on a free port.
		Port of value greater than 10,000 is recommended.
	For MCK-RA8T1, EK-RA2A2, EK-RA4L1, EK-RA4C1:
		Some jumper wires used to connect the Renesas Silex UART Pmod to the RA board.

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming
	and debugging.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5:
	-----------
		Connect Renesas Silex UART Pmod ----> PMOD1 (J26)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

	For EK-RA6M1, EK-RA6M2, EK-RA4M1:
	-----------
		Connect Renesas Silex UART Pmod ----> PMODA (J5)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMODA.

	For EK-RA2L1, FPB-RA4E1, FPB-RA6E1, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1, FPB-RA8E1,
	    EK-RA8P1, EK-RA8D2, EK-RA8M2:
	-----------
		For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on
		pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect Renesas Silex UART Pmod ----> PMOD2 (J25)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD2.

	For MCK-RA8T1:
	----------
		Connect
		1)Renesas Silex UART Pmod (CN1:2) ----> P112
		2)Renesas Silex UART Pmod (CN1:3) ----> P113
		3)Renesas Silex UART Pmod (CN1:5) ----> GND
		4)Renesas Silex UART Pmod (CN1:6) ----> VCC
		5)Renesas Silex UART Pmod (CN1:8) ----> P114

	For EK-RA2A2:
	----------
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P501 (J1:1)
		2) Renesas Silex UART Pmod (CN1:3) ----> P500 (J4:19)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC
		5) Renesas Silex UART Pmod (CN1:8) ----> P203 (J3:9)

	For EK-RA4L1:
	----------
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P109 (J3:19)
		2) Renesas Silex UART Pmod (CN1:3) ----> P110 (J3:20)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J3:25)
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC  (J3:1)
		5) Renesas Silex UART Pmod (CN1:8) ----> P306 (J4:11)

	For EK-RA8E2: 
	----------
		The user must turn ON SW4-4 to use PMOD2 for UART.
		Connect Renesas Silex UART Pmod (CN1:1) ----> PMOD2 (J25:1)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD2.

	For EK-RA4C1:
	----------
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and turn OFF SW4-4 to use
		the on-board debug functionality.
		The user must close E52 to use P305 for RST pin of PMOD2.
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P115 (J2:35)
		2) Renesas Silex UART Pmod (CN1:3) ----> P608 (J2:37)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J2:1)
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC  (J1:1)
		5) Renesas Silex UART Pmod (CN1:8) ----> P305 (J2:38)

	For MCK-RA8T2:
	----------
		Connect Renesas Silex UART Pmod (CN1:1) ----> Pmod 3A/6A (CN6:1)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the Pmod 3A/6A.

	For FPB-RA6E2:
	-----------
		The user must close E6 and open E5 to use PMOD1 for UART.
		Connect Renesas Silex UART Pmod ----> PMOD1
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

Note:
1) Sokit setup on PC as TCP server:
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

2) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
