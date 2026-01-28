/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of AWS https client library with ethernet driver on Renesas RA MCUs 
based on Renesas FSP using FreeRTOS. aws client is used to connect to https adafruit server which is cloud platform. 
On successful connection, menu is displayed enabling user to send GET,PUT,POST requests to adafruit server. On POST/PUT 
request, MCU Die temperature is read via(using) ADC and uploaded to server. On GET request, the last MCU die temperature data 
is read from the adafruit server. JlinkRTTViewer is used to display the status and responses of the requests made to server.
  	

2. Hardware Requirement:

	i. micro usb cable - 1no.
       ii. LAN cable - 1no.
      iii. Ethernet Switch - 1no.

3. Hardware Connections:
 	Supported Board EK_RA6M3, EK_RA6M5:		
	i. Connect EK_RA6M3 board to Host machine using micro usb cable.
       ii. connect LAN cable on EK_RA6M3/EK_RA6M5 at ethernet port and other end connect to ethernet Switch. 
           Switch should have WAN connection to communicate the server over internet. The Switch should be connected to the router 
           which is connected to the Internet.
	

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200044e8
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
