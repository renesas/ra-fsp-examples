/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	The example project demonstrates the use of the FreeRTOS + TCP Module with Ethernet. The example project
   	demonstrate the usage of FreeRTOS DHCP Client, DNS Client and ICMP code. This Example Project has 2 modes for the user
	1)DHCP Mode, where the IP credentials are obtained from the DHCP Server on the Network.
	2)Static IP Address mode, where in the Static IP address can be configured for the Ethernet based on the Availbilty of 
	  free address in the LAN. 
  	
NOTE  : User is expected to enter the static IP address according to the network settings of the LAN.
        In case of DHCP, User just needs to run the application and DHCP client will communicate to the DHCP server and gets the IP address.
	After obtaining the IP address and DNS lookup for the domain name requested, please wait for pinging to the requested IP address then the response information will be displayed in the RTT Viewer.
	Following is example of successful response in RTT Viewer:
	Ping Statistics for 172.217.160.174 :
	Packets: Sent  = 100, Received = 99, Lost = 00

2. Hardware Requirement:

	i. Micro usb cable - 1no.
       ii. LAN cable - 1no.
      iii. Ethernet Switch - 1no.

3. Hardware Connections:

	i. Connect RA board to Host machine using micro usb cable.
       ii. Connect LAN cable on RA board at ethernet port and other end connect to ethernet switch/router.

4. Configuration Settings:
		
NOTE  : While running the application using DHCP or Static Address mode, the following settings needs to be done in the configurator. 
      : The same projects can be used for both the settings.

|-----------------------------------------------------------------------------------------|
|FreeRTOS+TCP                                   Static IP       Dynamic IP                |
|=========================================================================================|
|Use DHCP	                                Disable         Enable                    |
|-----------------------------------------------------------------------------------------|
|DHCP Register Hostname	                        Disable         Enable                    |
|-----------------------------------------------------------------------------------------|
|DHCP Uses Unicast	                        Disable         Enable                    |
|-----------------------------------------------------------------------------------------|
|DHCP Send Discover After Auto IP	        Disable         Enable                    |
|-----------------------------------------------------------------------------------------|
|DHCP callback function	                        Disable         Enable                    |
|-----------------------------------------------------------------------------------------

5. Hardware Configuration:

- For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |      OFF    |      OFF     |     OFF    |     ON     |      OFF   |      OFF    |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

	CAUTION: Do not enable SW1-4 and SW1-5 together

- For EK-RA8M1: Remove jumper J61 to enable Ethernet B
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22003910
   b. Keil:	0x220082bc
   c. IAR: 0x2201469c
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
	For IAR, please initialize device back to factory default using the Renesas Device Partition Manager tool to avoid warnings related to incorrect aligment of Stack pointer.
