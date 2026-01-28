/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of Netx Duo FTP Client with an ethernet driver on Renesas 
        RA MCUs based on Renesas FSP using AzureRTOS. In this example project FTP client is configured to establish the 
	connection with FTP server. The FTP client will perform read and write operation, on to the currently opened
	file on the FTP server in host machine. The status message and other relevant data will be displayed on Jlink 
	RTT viewer. Error and info messages will be printed on JlinkRTTViewer.
  	
2. Hardware Requirement:

	i. 1x micro usb cable.
       ii. 1x Ethernet/LAN cable(Ethernet Cable CAT5/6).

3. Hardware Connections:
 	Supported Board EK_RA6M3, EK_RA6M4, EK_RA6M5:		
	i. Connect RA board to Host machine using micro usb cable.
       ii. Connect LAN cable on RA board at ethernet port and other end connect to ethernet switch. 
	
4. Software requirements:
 	i.  User need to run FTP Server preferably on windows or linux machine.


NOTE  : 1)Current ip addresses are provided according to our test environment. User has to change the IP addresses in ip instance
	  according to their network configuration
          property of g_ip0_Netx Duo IP instance -> IPv4 Address as per their network environment.
          FTP server IP : if it's a home/close network user can use 192.168.0.x where x represent any available ip address on network.


	2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
	   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000292c
   	b. Keil: Not Available 
   	c. IAR: Not Available
 
	3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   	in .map file generated in the build configuration folder (Debug/Release).


NOTE:	On RA6 devices (RA6M4/RA6M5), Ethernet may fail if TrustZone memory boundaries are not properly configured, 
	even for flat applications. When debugging with e² studio, the required TrustZone boundaries (IDAU) for Ethernet operation 
	are configured automatically but these settings are not applied when programming the device using a standalone method (e.g., via a HEX file).
	In such cases, users must configure the TrustZone memory partitions using Renesas Device Partition Manager (RDPM) or Renesas Flash Programmer.
	For more information on configuring TrustZone on RA devices, refer to:
	https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33 
