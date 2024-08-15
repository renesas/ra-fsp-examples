/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of Netx Duo DHCPv6 server with an ethernet driver on Renesas 
        RA MCUs based on Renesas FSP using AzureRTOS. DHCPV6 server is created and based on user input, user can start,
        and print DHCPV6 client info. On successful completion of each operation, the success status is displayed on 
        the RTT viewer. Error and info messages will be printed on JlinkRTTViewer.
  	
2. Hardware Requirement:

	i. 1x micro usb cable.
       ii. 1x Ethernet/LAN cable(Ethernet Cable CAT5/6).

3. Hardware Connections:
 	Supported Board EK_RA6M3/EK_RA6M4/EK_RA6M5:		
	i. Connect RA board to Host machine using micro usb cable.
       ii. Connect LAN cable on RA board at ethernet port and other end connect to ethernet port of PC/another RA board 
           on which dhcpv6 client is residing. 
	
NOTE  : User should wait for message "Ethernet link is up." on RTT viewer for ethernet/network link up status case.﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000944
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).


NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
