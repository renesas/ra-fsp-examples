/***********************************************************************************************************************
* 
* Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
*
* This software is supplied by Renesas Electronics America Inc. and may only be used with products of Renesas Electronics Corp.
* and its affiliates (“Renesas”).  No other uses are authorized.  This software is protected under all applicable laws, 
* including copyright laws.
* Renesas reserves the right to change or discontinue this software.
* THE SOFTWARE IS DELIVERED TO YOU “AS IS,” AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT 
* PERMISSIBLE UNDER APPLICABLE LAW,DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE.  TO THE MAXIMUM 
* EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE (OR ANY PERSON 
* OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING, WITHOUT LIMITATION, 
* ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES;
* ANY LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF 
* THE POSSIBILITY OF SUCH LOSS,DAMAGES, CLAIMS OR COSTS.
* 
**********************************************************************************************************************/

Project Overview:
	The example project demonstrates the functionalities of WiFi_OnChip_Silex module running on Renesas RA MCUs using Renesas 
	Silex UART Pmod hardware. The project gives user options to either scan WiFi networks or manually enter network credentials 
	in JlinkRTTViewer and connect. Then it performs ping operation on user entered URL or IP address. If successful,  
	'Ping Successful' message printed on JlinkRTTViewer. The project also demonstrates TCP socket operations in client mode. It tries
	connecting to the user entered TCP server IP address and port number. It is expected that TCP socket in server mode would be 
	running on user PC and listening on a free port before the EP tries to connect. Once connected to the TCP server, the user 
	should send 'on' or 'off' message from the server. Based on the message, on-board user LED would turn on/off and the same 
	would be displayed on JlinkRTTViewer. If an invalid message is received, then previous LED state would be maintained.
	After receiving messages from the server, appropriate acknowledgment  messages are sent to the server.

Project setup:

	External hardware requirements:
	*******************************
	1. Renesas Silex UART Pmod
	2. WiFi router Access Point(AP) with internet access. Note: WiFi router AP's credentials should be known.
	3. PC or laptop connected to the same WiFi router AP. On PC/laptop, create TCP server and start listening
	at port which is free. Port of value greater than 10,000 is recommended.
	
	Note: TCP server on PC can be created using freely available tool sokit: 
	https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml

	Hardware connection:
	********************
	EK-RA2E1/EK-RA4M2/EK-RA4M3/EK-RA6M3/EK-RA6M3G/EK-RA6M4/EK-RA6M5:
	-----------
	Connect
	Renesas Silex UART Pmod ----> PMOD 1 (J26)	

	EK-RA2L1/FPB-RA4E1/FPB-RA6E1
	----------
	connect
	Renesas Silex UART Pmod ----> PMOD 2 (J25)
	
	EK-RA2E2
	----------
	1)Renesas Silex UART Pmod (CN1:2) ----> P400
	2)Renesas Silex UART Pmod (CN1:3) ----> P401
	3)Renesas Silex UART Pmod (CN1:5) ----> GND
	4)Renesas Silex UART Pmod (CN1:6) ----> Vcc
	5)Renesas Silex UART Pmod (CN1:7) ----> P111
	6)Renesas Silex UART Pmod (CN1:8) ----> P015

Note : For EK-RA2E2, scanning wifi access points does not work. Please connect to wifi access points by entering its credentials manually.

Note : For EK-RA4M2/EK-RA4M3/EK-RA6M4/EK-RA6M5/FPB-RA4E1/FPB-RA6E1, Provide RTT Block Address in RTT viewer to run the EP.

Sokit setup on PC as TCP server:

	1. Download sokit tool form https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml
	2. Once downloaded, extract the contents and start sokit.exe.
	3. Once started, in the 'Server' tab configure the 'Network Setup'.
	4. In the 'TCP Addr' field, enter the PC's IPv4 address. And in 'Port' field, enter the port number.
	Note: Please provide an unused port number. It is recommended to give port number greater than 10,000.
	5. Then click the 'TCP Listen' button. This would start the TCP server on PC.
	6. The 'Output' window would display the message '15:20:25 MSG start TCP server successfully!'.
	7. Now wait for the EP to connect to server.
	8. Once the EP connects to the server, the 'Connections' window would display the IP address and port number of the client.
	9. There are three user input buffers provided for sending messages i.e. 'Buf 1', 'Buf 2' and 'Buf 3'.
	10. In one buffer, enter 'on'. In the second buffer, enter 'off'. And in the third buffer enter any invalid text.
	11. While sending, select the IP address of the client i.e. the MCU from the 'Connections' window. 
	12. And then click 'Send' button of the appropriate buffer to send message.
	13. Message received from the client would be displayed on the 'Output' window.
	14. The 'Output' window and console displays messages upon each transaction.
	15. To stop the server, click the 'TCP Listen' button again. This would stop the server.
	Note: Even if the server is stopped in sokit, the MCU may still be connected due to internal network connection.

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200041a4
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
