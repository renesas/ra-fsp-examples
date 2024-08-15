/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

        This project demonstrates the the basic functionality of PTP module along with Ethernet driver on Renesas 
        RA MCUs using Renesas FSP NetX Duo stack with Azure RTOS. The Example uses the DHCP client to get the IP 
        address from the DHCP server. On successful receiving of IP address from DHCP server, the success status 
        and Client IP address are displayed on Jlink RTTViewer. Once PTP is initialized in end to end slave mode, 
        it will get the current time stamp from Linux/R-pi master clock and upon successful reception of time stamp,
        time stamp will be displayed on Jlink RTTViewer. Error and info messages will be printed on Jlink RTTViewer.

2. Hardware Requirement:

	i. 1x Micro USB cable.
       ii. 2x Ethernet/LAN cable(Ethernet Cable CAT5/6).
      iii. 1x Router/Ethernet Switch (with a DHCP Server).
       iv. Raspberry-Pi or another EK-RA6M3 board (with PTP support) to operate as the PTP master clock.

3. Hardware Connections:
 	Supported Board EK-RA6M3:		
	i. Connect EK-RA6M3 board(J10) to Host machine using micro USB cable.
       ii. Connect LAN cable to the Ethernet Port of EK-RA6M3 and other end to the Ethernet Port on the switch 
           or Router.
      iii. Connect Raspberry-Pi or second EK-RA6M3 to the same network as EK-RA6M3.
	
