/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the use of the FreeRTOS + TCP Module with Ethernet. The example project
   	demonstrate the usage of FreeRTOS DHCP Client, DNS Client and ICMP code. This Example Project has 2 modes for the user
	1) DHCP Mode, where the IP credentials are obtained from the DHCP Server on the Network.
	2) Static IP Address mode, where in the Static IP address can be configured for the Ethernet based on the Availbilty of 
	   free address in the LAN. 
  	
	Note: 
	  User is expected to enter the static IP address according to the network settings of the LAN.
          In case of DHCP, User just needs to run the application and DHCP client will communicate to the DHCP server and gets the IP address.
	  After obtaining the IP address and DNS lookup for the domain name requested, please wait for pinging to the requested IP address then 
	  the response information will be displayed in the RTT Viewer.
	  Following is example of successful response in RTT Viewer:
	  Ping Statistics for 172.217.160.174 :
	  Packets: Sent  = 100, Received = 99, Lost = 00

2. Software Requirements:
	Refer to the Tools section in the Release Notes for set up suitable Software version.
	https://github.com/renesas/fsp/releases

3. Hardware Requirement:
	i. Micro usb cable - 1no or Type C usb cable (For MCK-RA8T1).
       ii. LAN cable - 1no.
      iii. Ethernet Switch - 1no.

4. Hardware Connections:
	i. Connect RA board to Host machine using micro usb cable or using type C usb cable (For MCK-RA8T1).
       ii. Connect LAN cable on RA board at ethernet port and other end connect to ethernet switch/router.

5. Configuration Settings:
		
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

6. Hardware Configuration:

- For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |      OFF    |      OFF     |     OFF    |     ON     |      OFF   |      OFF    |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

	CAUTION: Do not enable SW1-4 and SW1-5 together.

- For EK-RA8M1: Remove jumper J61 to enable Ethernet B.

Note:
1) User should view section 5 of application note R11AN0467 on how to build and run a trustzone project.
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

2) For the IAR project, user can launch the Workspace using the .eww workspace file in the non-secure project folder. 
   It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

3) For the KEIL project, user can launch the Workspace using the .uvmpw file in keil folder. It included multiple workspace 
   which has both secure (_s) and non-secure (_ns) project.

4) User must build secure project (_s) first, then build non-secure (_ns) project.

5) After the projects are built successfully, user should follow these three steps in sequence to launch the debug session:
   - Initialize device back to factory default. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Set Trustzone secure / non-secure boundaries. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Launch the Debug session from the non-secure project.

6) For IAR project, User can change setting in the Tools -> Option -> Stack -> Uncheck "Stack pointer(s) not
   valid until program reaches" option to ignore Stack Warning at launch. 	

7) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x32002930
   b. Keil:	0x32007278
   c. IAR: 0x32011650
 
8) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

9) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
   Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
