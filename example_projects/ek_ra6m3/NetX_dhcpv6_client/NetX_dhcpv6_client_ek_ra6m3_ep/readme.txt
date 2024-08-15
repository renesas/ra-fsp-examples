/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of Netx Duo DHCPv6 client with an ethernet driver on Renesas 
        RA MCUs based on Renesas FSP using AzureRTOS. DHCP Client is created and started. On successful receiving of 
        IP address from DHCPv6 server, It pings the server to verify the received IP address. On successful completion, 
        the success status and IP address are displayed on the RTT viewer. Error and info messages will be printed on JlinkRTTViewer.
  	
2. Hardware Requirement:

	i. 1x micro usb cable.
       ii. 2x Ethernet/LAN cable(Ethernet Cable CAT5/6).
      iii. 1x Ethernet Switch.

3. Hardware Connections:
 	Supported Board EK_RA6M3/EK_RA6M4/EK_RA6M5:		
	i. Connect RA board to Host machine using micro usb cable.
       ii. connect LAN cable on RA board at ethernet port and other end connect to ethernet switch/router. 
	

NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.