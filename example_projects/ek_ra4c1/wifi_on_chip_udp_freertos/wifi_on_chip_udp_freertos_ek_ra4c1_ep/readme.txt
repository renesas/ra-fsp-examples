/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the functionalities of WiFi_OnChip_Silex module running on Renesas RA MCUs
	using Renesas Silex UART Pmod hardware. Options are provided to the user to either connect to a known Wi-Fi
	network manually or by scanning available networks on the J-Link RTT Viewer. A ping operation on a
	user-specified URL or IP address is performed to check the internet connectivity. On successful validation,
	the user is allowed to connect to the UDP server by providing the UDP server's IP address and port number as
	inputs. A ping operation is performed to validate the connectivity to the server. On successful validation,
	EP creates a UDP socket, establishes a connection, and sends a "Hello" message to the UDP server. The EP
	continuously waits for any incoming messages from the UDP server. The incoming messages are displayed on the
	J-Link RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	PacketSender (https://packetsender.com/download#show)
	
3. Hardware Requirements:
	Supported RA Boards: EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, FPB-RA4E1, FPB-RA6E1, EK-RA8M1,
			     EK-RA8D1, MCK-RA8T1, EK-RA2A2, FPB-RA0E1, FPB-RA8E1, EK-RA4L1, FPB-RA0E2, EK-RA2L2
			     EK-RA8P1, EK-RA8E2, EK-RA4C1
  	1 x Renesas RA board.
  	1 x Renesas Silex UART Pmod.
	1 x Wi-Fi router Access Point(AP) with internet access. Note: WiFi router AP's credentials should be known.
	1 x PC or laptop connected to the same WiFi router AP. On PC/laptop, create UDP server and start listening
	    at a port which is free. Port of value greater than 10000 is recommended.
	1 x Type-C USB cable.

4. Hardware Connections:
	For EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA2L2:
	-----------
		Connect Renesas Silex UART Pmod ----> PMOD1 (J26)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

	For FPB-RA4E1, FPB-RA6E1, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8E2:
	----------
		For EK-RA8E2: The user needs to turn ON SW4-4 (PMOD2_MD_SEL) to use PMOD 2 for UART function.
		Connect Renesas Silex UART Pmod ----> PMOD2 (J25)
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD2.

	For MCK-RA8T1:
	----------
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P112 (CN2:P1)
		2) Renesas Silex UART Pmod (CN1:3) ----> P113 (CN1:P33)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc
		5) Renesas Silex UART Pmod (CN1:8) ----> P114 (CN1:P31)

	For EK-RA2A2:
	----------
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P501 (J1:1)
		2) Renesas Silex UART Pmod (CN1:3) ----> P500 (J4:19)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc
		5) Renesas Silex UART Pmod (CN1:8) ----> P203 (J3:9)
	
	For FPB-RA0E1, FPB-RA8E1:
	----------
		Connect Renesas Silex UART Pmod ----> PMOD1
		Please carefully align pin 1 on the Renesas Silex UART Pmod with pin 1 on the PMOD1.

	For EK-RA4L1:
	----------
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P109 (J3:19)
		2) Renesas Silex UART Pmod (CN1:3) ----> P110 (J3:20)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J3:25)
		4) Renesas Silex UART Pmod (CN1:6) ----> Vcc  (J3:1)
		5) Renesas Silex UART Pmod (CN1:8) ----> P306 (J4:11)

	For FPB-RA0E2:
	----------
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P402 (J1:3)
		2) Renesas Silex UART Pmod (CN1:3) ----> P403 (J1:4)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J1:8)
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC  (J1:11)
		5) Renesas Silex UART Pmod (CN1:8) ----> P208 (J1:20)

	For EK-RA4C1:
	----------
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		The user must close E52 to use P305 for RST pin of PMOD2.
		Connect
		1) Renesas Silex UART Pmod (CN1:2) ----> P115 (J2:35)
		2) Renesas Silex UART Pmod (CN1:3) ----> P608 (J2:37)
		3) Renesas Silex UART Pmod (CN1:5) ----> GND  (J2:1)
		4) Renesas Silex UART Pmod (CN1:6) ----> VCC  (J1:1)
		5) Renesas Silex UART Pmod (CN1:8) ----> P305 (J2:38)

5. Verifying Operation:
	To run this example project, the user needs to compile and download the example project to the evaluation kit
   	and then follow below instructions to observe the operation of the system.

	1) Download the Packet Sender Software and install it on the Host PC.
		Download link: https://packetsender.com/download#show
	2) Navigate to File > Settings on Packet Sender application and perform the following
		a) Disable all the servers except UDP
		b) Enter the desired server port number (preferably > 10000)
		c) Click OK to complete the server setup
	3) Connect the Silex Module as described in section 4.
	4) Connect the USB Debug port and view the status/error messages on J-Link RTT Viewer.
	5) On the J-Link RTT Viewer, Either connect to the Wi-Fi Access point manually or connect via Wi-Fi scan.
	6) On successful connection, the EP displays the MCU IP address.
	7) Ping to any valid desired IP address to verify the network connectivity from the board over Wi-Fi connection.
	8) On successful ping which verifies the connection, now connect to the UDP server by providing the IP address
	   and its port number. (e.g., UDP server on PC with IP address 192.168.0.177 and Port number 50000)
	9) On successful connection, "UDP socket connected" is displayed on the J-Link RTT Viewer.
	10) The EP then sends a "Hello" message to the server and continuously listens for incoming messages.
	11) Message from the MCU can be viewed on Packet Sender application. The application depicts the IP address
	    and the Port number from where the data is received.
	12) Data can be sent from the Server to client with the IP address and the port number mentioned in Step 11.
	
Note:
1) For EK-RA4M3, EK-RA6M4, EK-RA6M5, FPB-RA4E1, FPB-RA6E1, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA2A2, FPB-RA0E1, FPB-RA8E1,
   EK-RA4L1, FPB-RA0E2, EK-RA2L2, EK-RA8P1, EK-RA8E2, EK-RA4C1 provide RTT Block Address in J-Link RTT Viewer to run the EP.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004634
   b. Keil:     Not Available
   c. IAR:      Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
