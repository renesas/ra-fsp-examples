/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of Netx Duo FTP server with an ethernet driver on Renesas 
        RA MCUs based on Renesas FSP using AzureRTOS. FTP server is created and based on user input, user can use
	FTP client machine for communication with the created server.
        On successful completion of each operation, the success status is displayed 
        on the RTT viewer. Error and info messages will be printed on JlinkRTTViewer.
  	
2. Hardware Requirement:

	i. 1x micro usb cable.
       ii. 1x Ethernet/LAN cable(Ethernet Cable CAT5/6).

3. Hardware Connections:
 	Supported Board EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1:		
	i. Connect RA board to Host machine using micro usb cable.
        ii. Connect LAN cable on RA board at ethernet port and other end connect to ethernet switch. 
	
4. Software requirements:
 	i.  User need to run FTP client preferably on windows or linux machine command prompt using default ftp OS utility.

﻿5. Hardware Configuration:

- For EK-RA8M1: Remove jumper J61 to enable Ethernet B

NOTE: It is expected that user should use necessary network set up and user need to run FTP client by passing FTP server IP address which is set in configurator property of g_ip0_Netx Duo IP instance -> IPv4 Address as per their network environment.
      FTP server IP : if it's a home/close network user can use 192.168.0.x where x represent any available ip address on network.
      FTP client    : For FTP client use PC command prompt with default ftp utility(available in both windows and Linux) which will act as ftp client and the PC should be running in the same network environment.
      For establishing connection with FTP server using FTP client, user should wait for message "FTP server started and ready to use !!" on RTT viewer.
      User is expected to enter rtt input data of size not exceeding 15 bytes.

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22001380
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).


NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
