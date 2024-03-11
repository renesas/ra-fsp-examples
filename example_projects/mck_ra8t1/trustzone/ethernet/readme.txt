/***********************************************************************************************************************
* 
* Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
*
* This software is supplied by Renesas Electronics America Inc. and may only be used with products of Renesas Electronics Corp.
* and its affiliates (“Renesas”).  No other uses are authorized.  This software is protected under all applicable laws, 
* including copyright laws.
* Renesas reserves the right to change or discontinue this software.
* THE SOFTWARE IS DELIVERED TO YOU “AS IS,” AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT 
* PERMISSIBLE UNDER APPLICABLE LAW,DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE.  TO THE MAXIMUM 
* EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE (OR ANY PERSON 
* OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING, WITHOUT LIMITATION, 
* ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES;
* ANY LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF 
* THE POSSIBILITY OF SUCH LOSS,DAMAGES, CLAIMS OR COSTS.
* 
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

3. Hardware Configuration:

- For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |      OFF    |      OFF     |     OFF    |     ON     |      OFF   |      OFF    |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

	CAUTION: Do not enable SW1-4 and SW1-5 together.

- For EK-RA8M1: Remove jumper J61 to enable Ethernet B.

Note:
1) User can refer Section 5 "Example Project for IP Protection" in the document to run trustzone EP via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

2 For IAR, user can open non-secure (_ns) project. It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

3) For KEIL, user can open .uvmpw file in keil folder for multiple workspace which has both secure (_s) and non-secure (_ns) project.

4) User must build secure project (_s) first, then build non-secure (_ns) project.

5) The first compiling in non-secure (_ns) of IAR project may have issue with error message, this is because of timing issue 
   between EWARM and RSAC operation which described in step 10 of section 5.5.1 in
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection  

6) After building successfully, user must open Renesas Device Partition Manager -> Initialize device back to factory default -> Set Trustzone 
   secure / non-secure boundaries -> Run non-secure (_ns) project. 	

7) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x32002930
   b. Keil:	Not available
   c. IAR:      0x32011658
 
8) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

9) On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
   Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
