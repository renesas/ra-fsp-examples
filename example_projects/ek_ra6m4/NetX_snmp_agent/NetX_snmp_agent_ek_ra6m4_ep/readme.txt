/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

        This project demonstrates the basic functionality of NetX Duo SNMP agent with Ethernet driver on Renesas 
        RA MCUs using Renesas FSP NetX Duo stack with Azure RTOS.  In this sample code, a NetX Duo IP instance is 
        created with IP address as (default 0.0.0.0 - Not assigned). The NetX stack is enabled for UDP, ICMP, ARP. 
        SNMP is a protocol that utilizes the connectionless User Datagram Protocol (UDP) services to perform its 
        management function. SNMP Agent creates its own packet pool based on the settings minimum packet payload 
        size and number of packets in the packet pool. Once SNMP Agent is created and initialized,  An agent is 
        responsible for responding to SNMP Manager’s commands and sending event driven traps. On Pressing any key,
        from JlinkRTTViewer the Agent sends trapv2 event to the manager. The status messages, client's info, and the code flow 
        informational messages as part of the initialization are displayed on the JlinkRTTViewer.

2. Hardware Requirement:

	i. 1x micro usb cable.
       ii. 2x Ethernet/LAN cable(Ethernet Cable CAT5/6).
      iii. 1x Ethernet Switch to connect to the router or LAN(connected to Internet).
       iv. Linux machine or Raspberry pi to run the SNMP Manager

3. Hardware Connections:
 	Supported Board EK_RA6M3, EK_RA6M4, EK_RA6M5:		
	i. Connect EK_RA6M3 board(J10)/ EK_RA6M5 board(J10)/ EK_RA6M4 board(J10) to Host machine using micro USB cable.
       ii. Connect LAN cable to the Ethernet Port of EK-RA6M3 and other end to the Ethernet Port on the switch 
           or Router. Switch connecting to the router or router should have access to the internet.
	

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001a28
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).


NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
