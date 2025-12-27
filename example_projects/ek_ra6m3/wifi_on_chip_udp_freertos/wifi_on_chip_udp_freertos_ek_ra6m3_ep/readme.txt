/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	The example project demonstrates the functionalities of WiFi_OnChip_Silex module running on Renesas RA MCUs using Renesas 
	Silex UART Pmod hardware. Options are provided to the user to either connect to a known Wi-Fi network manually or by scanning 
	available networks on the J-Link RTT Viewer. A ping operation on a user-specified URL or IP address is performed to check the 
	internet connectivity. On successful validation, the user is allowed to connect to the UDP server by providing the UDP 
	server's IP address and port number as inputs. A ping operation is performed to validate the connectivity to the server. 
	On successful validation, EP creates a UDP socket, establishes a connection, and sends a "hello" message to the UDP server. 
	The EP continuously waits for any incoming messages from the UDP server. The incoming messages are displayed on the J-Link RTT Viewer.

2. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below 
instructions to observe the operation of the system.

	Supported Boards: EK-RA6M3
   
    	1) Download the PacketSender Software and install it on the Host PC
		https://packetsender.com/download#show
	
	2) Navigate to File > Settings on PacksetSender application and perform the following
		a) Disable all the servers except UDP
		b) Enter the desired server port number(preferably >10000)
		c) Click OK to complete the server setup
   
	3) Connect the Silex Module as described in section 4.
	
	4) Connect the USB Debug port and view the status/error messages on J-Link RTT Viewer.
			
	5) On the J-Link RTT Viewer, Either connect to the Wi-Fi Access point manually or connect via Wi-Fi scan
	
	6) On successful connection, the EP displays the MCU IP address
	
	7) Ping to any valid desired IP address to verifiy the network connectivity from the board over Wi-Fi connection
	
	8) On successful ping which verifies the connection, now connect to the UDP server by providing the IP address and its port number (Eg, UDP server on PC with IP address 192.168.0.177 and Port number 5000)
	
	9) On successful connection, "UDP socket connected" is displayed on the J-Link RTT viewer.
	
	10) The EP then sends a "hello" message to the server and continously listens for incoming messages
	
	11) Message from the MCU can be viewed on PacketSender application. The application depicts the IP address and the Port number from where the data is recieved.

	12) Data can be sent from the Server to client with the IP address and the port number mentioned in Step 11
	
3. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	SEGGER J-Link RTT Viewer: Version 8.92
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	PacketSender (https://packetsender.com/download#show)
   
4. Hardware Requirements:
	1. 1 x RA board 
	2. 1 x Renesas Silex UART Pmod module
	3. 1 x Wi-Fi router Access Point(AP) with internet access. Note: WiFi router AP's credentials should be known.
	4. PC or laptop connected to the same WiFi router AP. On PC/laptop, create UDP server and start listening
	   at port which is free. Port of value greater than 10000 is recommended.
	5. 1 x Micro USB cable

5. Hardware settings for the project:
	EK-RA4M3/EK-RA6M3/EK-RA6M3G/EK-RA6M4/EK-RA6M5:
	-----------
	Connect
	Renesas Silex UART Pmod ----> PMOD 1 (J26)	

	FPB-RA4E1/FPB-RA6E1
	----------
	connect
	Renesas Silex UART Pmod ----> PMOD 2 (J25)
	
Note : For EK-RA4M3/EK-RA6M4/EK-RA6M5/FPB-RA4E1/FPB-RA6E1 Provide RTT Block Address in RTT viewer to run the EP.

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x1ffe0294
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
