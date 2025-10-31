/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the functionalities of NetX Duo WiFi Driver using Azure RTOS running on Renesas
     	RA MCUs using Renesas Silex UART Pmod hardware. The project utilizes some functionalities of WiFi_OnChip_Silex
     	module which are not supported by Netx Duo. The project gives the user options to either scan WiFi networks or manually
     	enter network credentials in J-Link RTT Viewer and connect. Then it performs ping operation on the user entered URL or IP
     	address. If successful, 'Ping Successful' message printed on J-Link RTT Viewer. The project also demonstrates TCP
     	socket operations in client mode. It tries connecting to the user entered TCP server IP address and port number.
     	It is expected that TCP socket in server mode would be running on the user PC and listening on a free port before the
     	EP tries to connect. Once connected to the TCP server, the user should send 'on' or 'off' message from the server.
     	Based on the message, on-board user LED would turn on/off and the same would be displayed on J-Link RTT Viewer. If an
     	invalid message is received, then previous LED state would be maintained. After receiving messages from the server,
     	appropriate acknowledgment messages are sent to the server.

2. Software Requirements:
     	Renesas Flexible Software Package (FSP): Version 6.2.0
     	e2 studio: Version 2025-10
     	SEGGER J-Link RTT Viewer: Version 8.74
     	LLVM Embedded Toolchain for ARM: Version 18.1.3
     	Sokit software is required for creation of TCP server over specified IP and port (detailed steps mentioned below)
     	Download Link: https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml

3. Hardware Requirements:
     	Supported RA boards: EK-RA4M2, EK-RA2L1, EK-RA4M1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4M3,
			     FPB-RA4E1, FPB-RA6E1, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1, 
			     MCK-RA8T1, EK-RA2A2, FPB-RA8E1, EK-RA8E2.
     	1 x Renesas RA board.
     	1 x Renesas Silex UART Pmod.
     	1 x WiFi router Access Point (AP) with internet access. Note: WiFi router AP's credentials should be known.
     	1 x PC or laptop connected to the same WiFi router AP. On PC/laptop, create TCP server and start listening
	    at port which is free.

4. Hardware Connections:
     	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5:
     	-----------
     		Connect
     		Renesas Silex UART Pmod ----> PMOD 1 (J26)	

     	For EK-RA6M1, EK-RA6M2, EK-RA4M1:
     	-----------
     		Connect
     		Renesas Silex UART Pmod ----> PMOD A (J5)

     	For EK-RA2L1, FPB-RA4E1, FPB-RA6E1, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1, FPB-RA8E1:
     	-----------
     		Connect
     		Renesas Silex UART Pmod ----> PMOD 2

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

	For EK-RA8E2: 
	----------
	    	Connect 
		The user must turn ON SW4-4 to use PMOD2 for UART.
		Renesas Silex UART Pmod (J1:1) ----> PMOD2 (J25:1)

5. Sokit setup on PC as TCP server:
   	1)  Download Sokit tool from https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml
   	2)  Once downloaded, extract the contents and start sokit.exe.
   	3)  Once started, in the 'Server' tab configure the 'Network Setup'.
   	4)  In the 'TCP Addr' field, enter the PC's IPv4 address. And in 'Port' field, enter the port number.
      	    Note: Please provide an unused port number. It is recommended to give port number greater than 10,000.
   	5)  Then click the 'TCP Listen' button. This would start the TCP server on PC.
   	6)  The 'Output' window would display the message '15:20:25 MSG start TCP server successfully!'.
   	7)  Now wait for the EP to connect to server.
   	8)  Once the EP connects to the server, the 'Connections' window would display the IP address and port
            number of the client.
   	9)  There are three user input buffers provided for sending messages i.e. 'Buf 1', 'Buf 2' and 'Buf 3'.
   	10) In first buffer, enter 'on'. In the second buffer, enter 'off'. And in the third buffer enter any invalid text.
   	11) While sending, select the IP address of the client i.e. the MCU from the 'Connections' window.
   	12) And then click 'Send' button of the appropriate buffer to send message.
   	13) Message received from the client would be displayed on the 'Output' window.
   	14) The 'Output' window and console displays messages upon each transaction.
   	15) To stop the server, click the 'TCP Listen' button again. This would stop the server.
      	    Note: Even if the server is stopped in Sokit, the MCU may still be connected due to internal network connection.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220404f0
   b. Keil:     Not Available
   c. IAR:      Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
