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

2. Configuration Settings:
		
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
|-----------------------------------------------------------------------------------------|
		
	
﻿
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000590c
   b. Keil:	0x2000a2cc
   c. IAR: 0x20011640
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).


NOTE:	On RA6 devices (RA6M4/RA6M5), Ethernet may fail if TrustZone memory boundaries are not properly configured, 
	even for flat applications. When debugging with e² studio, the required TrustZone boundaries (IDAU) for Ethernet operation 
	are configured automatically but these settings are not applied when programming the device using a standalone method (e.g., via a HEX file).
	In such cases, users must configure the TrustZone memory partitions using Renesas Device Partition Manager (RDPM) or Renesas Flash Programmer.
	For more information on configuring TrustZone on RA devices, refer to:
	https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33 
