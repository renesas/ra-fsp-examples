/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the functionalities of NetX Duo WiFi Driver using Azure RTOS running on Renesas RA MCUs using Renesas 
	Silex UART Pmod hardware.The project utilises some functionalities of WiFi_OnChip_Silex module which are not supported by Netx Duo.
	The project gives user options to either scan WiFi networks or manually enter network credentials in JlinkRTTViewer and connect. 
	Then it performs ping operation on user entered URL or IP address. If successful'Ping Successful' message printed on JlinkRTTViewer. 
	The project also demonstrates TCP socket operations in client mode. It tries connecting to the user entered TCP server IP address and 
	port number. It is expected that TCP socket in server mode would be running on user PC and listening on a free port before the EP tries 
	to connect. Once connected to the TCP server, the user should send 'on' or 'off' message from the server. Based on the message, on-board 
	user LED would turn on/off and the same would be displayed on JlinkRTTViewer. If an invalid message is received, then previous LED state
 	would be maintained. After receiving messages from the server, appropriate acknowledgment  messages are sent to the server.

2. Project setup:

	External hardware requirements:
	*******************************
	1. Renesas Silex UART Pmod
	2. WiFi router Access Point(AP) with internet access. Note: WiFi router AP's credentials should be known.
	3. PC or laptop connected to the same WiFi router AP. On PC/laptop, create TCP server and start listening
	at port which is free.
	
	Note: TCP server on PC can be created using freely available tool sokit: 
	https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml

	Hardware connection:
	********************
	EK-RA4M2/EK-RA4M3/EK-RA6M3/EK-RA6M4/EK-RA6M5/FPB-RA4E1/FPB-RA6E1:
	-----------
	Connect
	Renesas Silex UART Pmod ----> PMOD 1 (J26)	

        EK-RA6M1/EK-RA6M2/EK-RA4M1:
	-----------
	Connect
	Renesas Silex UART Pmod ----> PMOD A (J5)

        EK-RA2L1/EK-RA4E2/EK-RA6E2/MCK-RA4T1/MCK-RA6T3/EK-RA8M1/EK-RA8D1:
        -----------
        Connect
        Renesas Silex UART Pmod ----> PMOD 2

	MCK-RA8T1
	----------
	1) Renesas Silex UART Pmod (CN1:2) ----> P112
	2) Renesas Silex UART Pmod (CN1:3) ----> P113
	3) Renesas Silex UART Pmod (CN1:5) ----> GND
	4) Renesas Silex UART Pmod (CN1:6) ----> Vcc
	5) Renesas Silex UART Pmod (CN1:8) ----> P114

	EK-RA2A2
	----------
	1) Renesas Silex UART Pmod (CN1:2) ----> P501 (J1:1)
	2) Renesas Silex UART Pmod (CN1:3) ----> P500 (J4:19)
	3) Renesas Silex UART Pmod (CN1:5) ----> GND
	4) Renesas Silex UART Pmod (CN1:6) ----> Vcc
	5) Renesas Silex UART Pmod (CN1:8) ----> P203 (J3:9)
       
Note: 
1) For EK-RA4M2/EK-RA6M4/EK-RA6M5/EK-RA4M3/FPB-RA4E1/FPB-RA6E1/EK-RA4E2/EK-RA6E2/MCK-RA4T1/MCK-RA6T3/EK-RA8M1/EK-RA8D1/MCK-RA8T1/EK-RA2A2, 
   Provide RTT Block Address in RTT viewer to run the EP.

2) Sokit setup on PC as TCP server:
	a. Download sokit tool form https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml
	b. Once downloaded, extract the contents and start sokit.exe.
	c. Once started, in the 'Server' tab configure the 'Network Setup'.
	d. In the 'TCP Addr' field, enter the PC's IPv4 address. And in 'Port' field, enter the port number.
	   Note: Please provide an unused port number. It is recommended to give port number greater than 10,000.
	e. Then click the 'TCP Listen' button. This would start the TCP server on PC.
	f. The 'Output' window would display the message '15:20:25 MSG start TCP server successfully!'.
	g. Now wait for the EP to connect to server.
	h. Once the EP connects to the server, the 'Connections' window would display the IP address and port number of the client.
	i. There are three user input buffers provided for sending messages i.e. 'Buf 1', 'Buf 2' and 'Buf 3'.
	k. In one buffer, enter 'on'. In the second buffer, enter 'off'. And in the third buffer enter any invalid text.
	l. While sending, select the IP address of the client i.e. the MCU from the 'Connections' window. 
	m. And then click 'Send' button of the appropriate buffer to send message.
	n. Message received from the client would be displayed on the 'Output' window.
	o. The 'Output' window and console displays messages upon each transaction.
	p. To stop the server, click the 'TCP Listen' button again. This would stop the server.
	Note: Even if the server is stopped in sokit, the MCU may still be connected due to internal network connection.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001254
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
