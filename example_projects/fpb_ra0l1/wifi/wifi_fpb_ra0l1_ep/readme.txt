/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the functionalities of WiFi_OnChip_Silex module running on Renesas RA MCUs
	using Renesas Silex UART Pmod hardware. The project gives the user options to either scan WiFi networks or
	manually enter network credentials in J-Link RTT Viewer and connect. Then it performs ping operation on the
	user entered URL or IP address. If successful, 'Ping Successful' message printed on J-Link RTT Viewer. The
	project also demonstrates TCP socket operations in client mode. It tries connecting to the user entered TCP
	server IP address and port number. It is expected that TCP socket in server mode would be running on the user
	PC and listening on a free port before the EP tries to connect. Once connected to the TCP server, the user
	should send 'on' or 'off' message from the server. Based on the message, on-board user LED would turn ON/OFF
	and the same would be displayed on J-Link RTT Viewer. If an invalid message is received, then previous LED
	state would be maintained. After receiving messages from the server, appropriate acknowledgment messages are
	sent to the server.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Note: TCP server on PC can be created using freely available tool sokit:
	      https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml

3. Hardware Requirements:
	1 x Renesas RA board.
	1 x Renesas Silex UART Pmod.
	1 x WiFi router Access Point(AP) with internet access. Note: WiFi router AP's credentials should be known.
	1 x PC or laptop connected to the same WiFi router AP. On PC/laptop, create TCP server and start listening
	    at port which is free. Port of value greater than 10,000 is recommended.
	1 x Type-C USB cable.

4. Hardware Connections:
	For EK-RA2E1, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA2L2:
		Connect Renesas Silex UART Pmod ----> PMOD1 (J26)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

	For EK-RA2L1, FPB-RA4E1, FPB-RA6E1, EK-RA8M1, EK-RA8D1, EK-RA8P1:
		Connect Renesas Silex UART Pmod ----> PMOD2 (J25)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD2.
	
	For EK-RA2E2:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P400
		2) Renesas Silex UART Pmod (CN1:3) ----> P401
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC
		5) Renesas Silex UART Pmod (CN1:7) ----> P111
		6) Renesas Silex UART Pmod (CN1:8) ----> P015
	
	For MCK-RA6T2:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P605
		2) Renesas Silex UART Pmod (CN1:3) ----> P606
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC
		5) Renesas Silex UART Pmod (CN1:7) ----> P300
		6) Renesas Silex UART Pmod (CN1:8) ----> P302

	For MCK-RA8T1:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P112 (CN2:P1)
		2) Renesas Silex UART Pmod (CN1:3) ----> P113 (CN1:P33)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC
		5) Renesas Silex UART Pmod (CN1:8) ----> P114 (CN1:P31)

	For EK-RA2A2:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P501 (J1:1)
		2) Renesas Silex UART Pmod (CN1:3) ----> P500 (J4:19)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC
		5) Renesas Silex UART Pmod (CN1:8) ----> P203 (J3:9)

	For FPB-RA0E1, FPB-RA8E1:
		Connect Renesas Silex UART Pmod ----> PMOD1
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

	For EK-RA4L1:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P109 (J3:19)
		2) Renesas Silex UART Pmod (CN1:3) ----> P110 (J3:20)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J3:25)
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC  (J3:1)
		5) Renesas Silex UART Pmod (CN1:8) ----> P306 (J4:11)

	For FPB-RA0E2:
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P402 (J1:3)
		2) Renesas Silex UART Pmod (CN1:3) ----> P403 (J1:4)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J1:8)
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC  (J1:11)
		5) Renesas Silex UART Pmod (CN1:8) ----> P208 (J1:20)

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		The user must close E52 to use P305 for RST pin of PMOD2.
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P115 (J2:35)
		2) Renesas Silex UART Pmod (CN1:3) ----> P608 (J2:37)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J4:25)
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC  (J4:1)
		5) Renesas Silex UART Pmod (CN1:8) ----> P305 (J2:38)

	For FPB-RA0L1:
		To use the on-board debug functionality, the user must place jumper J9 on pins 1-2.
		To use P101, P100, P013 on PMOD1, the user must close E34, E36, E35 and open E40.
		Connect Renesas Silex UART Pmod ----> PMOD1 (J19)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

Note:
1) For EK-RA2E2, scanning wifi access points does not work. Please connect to wifi access points by entering its
   credentials manually.

2) For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5, FPB-RA4E1, FPB-RA6E1, MCK-RA6T2, EK-RA8M1, EK-RA8D1, MCK-RA8T1,
   EK-RA2A2, FPB-RA0E1, FPB-RA8E1, EK-RA4L1, FPB-RA0E2, EK-RA2L2, EK-RA8P1, EK-RA4C1, FPB-RA0L1 provide RTT Block
   Address in J-Link RTT Viewer to run the EP.

3) Sokit setup on PC as TCP server:
	a. Download sokit tool from https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml
	b. Once downloaded, extract the contents and start sokit.exe.
	c. Once started, in the 'Server' tab configure the 'Network Setup'.
	d. In the 'TCP Addr' field, enter the PC's IPv4 address. And in 'Port' field, enter the port number.
	   Note: Please provide an unused port number. It is recommended to give port number greater than 10,000.
	e. Then click the 'TCP Listen' button. This would start the TCP server on PC.
	f. The 'Output' window would display the message '15:20:25 MSG start TCP server successfully!'.
	g. Now wait for the EP to connect to server.
	h. Once the EP connects to the server, the 'Connections' window would display the IP address and port number of
           the client.
	i. There are three user input buffers provided for sending messages i.e. 'Buf 1', 'Buf 2' and 'Buf 3'.
	k. In first buffer, enter 'on'. In the second buffer, enter 'off'. And in the third buffer enter any invalid text.
	l. While sending, select the IP address of the client i.e. the MCU from the 'Connections' GroupBox. And then click
	   'Send' button of the appropriate buffer to send message.
	m. Message received from the client would be displayed on the 'Output' window.
	n. The 'Output' window and console displays messages upon each transaction.
	o. To stop the server, click the 'TCP Listen' button again. This would stop the server.
	   Note: Even if the server is stopped in sokit, the MCU may still be connected due to internal network connection.

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20005138
   b. Keil:	Not Available
   c. IAR:	Not Available

5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
