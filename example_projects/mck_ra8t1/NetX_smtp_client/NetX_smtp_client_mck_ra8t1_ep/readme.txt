/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of Netx Duo SMTP client with an ethernet driver on Renesas 
        RA MCUs based on Renesas FSP using AzureRTOS. The project creates SMTP client and transfer e-mail message
        to local configured SMTP server. Based on user input from RTTViewer, EP Performs:
        1. Enter 1 to send email message to the server.
        2. Enter 2 to view Pre-configured email message.
        On successful completion, the success status are displayed on the RTT viewer. Error and info messages will be 
        printed on JlinkRTTViewer.
  	
2. Hardware Requirement:

	i. 1x micro usb cable or type C usb cable (For MCK-RA8T1).
       ii. 2x Ethernet/LAN cable(Ethernet Cable CAT5/6).
      iii. 1x Ethernet Switch.

3. Hardware Connections:
 	Supported Board EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1:		
	i. Connect RA board to Host machine using type C usb cable.
       ii. Connect LAN cable on RA board at ethernet port and other end connect to ethernet switch.

4. Hardware Configuration:

- For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     ON     |     OFF    |     OFF     |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

	CAUTION: Do not enable SW1-4 and SW1-5 together

- For EK-RA8M1: Remove jumper J61 to enable Ethernet B
           
4. Software requirements:
       i.  User need to install and run Argo SoftMail server. For details please refer NetX_smtp_client_notes.md file.

NOTE : Users need to configure the server ip address and client ip address in the configuration property of g_smtp_client0 Netx Duo SMTP Client -> Server IPv4 Address and
       the client ip address in configurator property of g_ip0_Netx Duo IP instance -> IPv4 Address as per their network environment.
       For e.g 
       Client IP : if its an home network user can use 192.168.0.x where x represent any available ip address on network.    
       Server IP : For Server ip use ip of the pc in which smtp mail server is configured in the same network environment.
 
	
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000950
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
