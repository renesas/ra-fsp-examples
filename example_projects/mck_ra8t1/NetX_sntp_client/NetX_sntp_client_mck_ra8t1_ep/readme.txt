/**********************************************************************************************************************
 * File Name    : readme.txt
 * Description  : Contains general information about Example Project and detailed instructions
 *********************************************************************************************************************/

1. Project Overview:
        This project demonstrates the real-time clock functionality without the RTC hardware present on the RA MCU,
	using the Renesas FSP NetX Duo SNTP stack and Azure RTOS with an Ethernet interface. Initially, the system
	time is set based on the compiler's build time and the Azure RTOS tick count. To start with the DHCP client
	obtains the IP address from the DHCP server for RA board IP assignment. The RA board's IP address, obtained
	via DHCP, is displayed on the Jlink RTTViewer upon successful execution of the DHCP client functionality.
	An SNTP client is created, and the SNTP server URL's IP address is resolved through the DNS stack functions.
	The SNTP client then requests the server to provide network timestamp updates. Upon successfully receiving
	a timestamp from the server, the system timestamp is read and compared with the received timestamp, and the
	system time is updated if there is a difference. Status, informational, and error messages (if any occur)
	will be displayed on the Jlink RTTViewer.

2. Hardware Requirements:
	Supported RA Board: MCK-RA8T1
	1 x RA board
	1 x type C USB cable.
        2 x Ethernet/LAN cable (Ethernet Cable CAT5/6).
      	1 x Ethernet Switch to connect to the router or LAN (connected to the Internet).

3. Hardware Connections:
	i. Connect MCK-RA8T1 board (CN13) to Host machine using type C USB cable.
       ii. Connect LAN cable to the Ethernet Port of MCK-RA8T1 and the other end to the Ethernet Port on the switch 
           or Router. Switch connecting to the router or router should have access to the internet.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220009b0
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

3) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
   Consequentially, it is necessary to connect the serial programming interface to meet this requirement.