/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

        This project demonstrates the basic functionality of NetX Duo HTTPS client with ethernet driver on 
        Renesas RA MCUs based on Renesas FSP using AzureRTOS. The EP uses the DHCP client to get the IP address 
        from the DHCP server. On successful receiving of IP address from DHCP server and resolving the IP address 
        for the Domain name for "Adafruit IO" using the DNS lookup, On successful completion of DNS lookup, 
        the success status and IP address are displayed. Now HTTP Client is created and started.
        HTTP GET/SET/PUT operations can be performed by the user using the Menu provided on JLink RTT Viewer Console.
        Error and info messages will be printed on Jlink RTTViewer.

2. Hardware Requirement:

	i. Micro USB cable - 1no.
       ii. Ethernet Cable CAT5/6 (LAN cable) - 1no.
      iii. Ethernet Switch to connect to the router or LAN(connected to Internet) - 1no.

3. Hardware Connections:
 	Supported Board EK_RA6M3, EK_RA6M4, EK_RA6M5, EK_RA8M1:		
	i. Connect EK_RA6M3 board(J10)/ EK_RA6M4 board(J10)/ EK_RA6M5 board(J10)/ EK_RA8M1 board(J10) to Host machine using micro USB cable.
       ii. Connect LAN cable to the Ethernet Port of EK-RA6M3 and other end to the Ethernet Port on the switch 
           or Router. Switch connecting to the router or router should have access to the internet.

4. Hardware Configuration:
﻿
- For EK-RA8M1: Remove jumper J61 to enable Ethernet B

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220033c0
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).


NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
