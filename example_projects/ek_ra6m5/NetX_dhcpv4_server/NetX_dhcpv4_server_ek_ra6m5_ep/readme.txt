/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of Netx Duo DHCPv4 server with an ethernet driver on Renesas 
        RA MCUs based on Renesas FSP using AzureRTOS. DHCPV4 server is created and based on user input, user can start,
        stop and print DHCPV4 client info. On successful completion of each operation, the success status is displayed 
        on the RTT viewer. Error and info messages will be printed on JlinkRTTViewer.
  	
2. Hardware Requirement:

	i. 1x micro usb cable.
       ii. 1x Ethernet/LAN cable(Ethernet Cable CAT5/6).

3. Hardware Connections:
 	Supported Board EK_RA6M3/EK_RA6M4/EK_RA6M5:		
	i. Connect RA board to Host machine using micro usb cable.
       ii. Connect LAN cable on RA board at ethernet port and other end connect to ethernet port of another RA board 
           on which dhcpv4 client is residing. 
	
NOTE  : User should wait for message "Ethernet link is up." on RTT viewer for ethernet/network link up status case.﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001128
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

NOTE:	On RA6 devices (RA6M4/RA6M5), Ethernet may fail if TrustZone memory boundaries are not properly configured, 
	even for flat applications. When debugging with e² studio, the required TrustZone boundaries (IDAU) for Ethernet operation 
	are configured automatically but these settings are not applied when programming the device using a standalone method (e.g., via a HEX file).
	In such cases, users must configure the TrustZone memory partitions using Renesas Device Partition Manager (RDPM) or Renesas Flash Programmer.
	For more information on configuring TrustZone on RA devices, refer to:
	https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33 
