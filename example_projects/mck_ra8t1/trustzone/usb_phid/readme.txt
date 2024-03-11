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

	This project demonstrates the basic functionalities of USB PHID driver on Renesas RA MCUs based on Renesas FSP.
	The project configures RA board as a Keyboard. On pressing any key from RTTViewer, MCU sends the host PC 
	characters from a - z & numbers 0 - 9.

2. Hardware Requirement:

	micro USB Cable : 1 nos or Type C USB Cable: 1 nos (For MCK-RA8T1)
	
3. Hardware connection :
		
	EK-RA6M4,EK-RA4M3,EK-RA4M2,EK-RA6M5,EK-RA8M1,EK-RA8D1:(support only full speed)
	  Jumper J12 placement is pins 2-3
	  Connect jumper J15 pins 
	  Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	  port (J11) of the board.  Connect the other end of this cable to USB port of the 
	  host PC. 

	EK-RA6E2: (support only full speed)
 	  Connect the micro USB end of the micro USB cable to micro-AB USB Full Speed
	  port (J11) of the board.  Connect the other end of this cable to USB port of the
	  host PC. 

	MCK-RA8T1:(support only full speed)
	Jumper JP9 placement is pins 2-3
	Connect jumper JP10 pins 
	Connect the micro USB end of the micro USB device cable to Type C USB Full Speed 
	port (CN14) of the board.  Connect the other end of this cable to USB port of the 
	host PC. 

Note :
1) Open Device Manager, and check 'Human Interface Devices', to see ' USB Input Device' has been detected.

2) Lower case a - z and numbers 0 - 9 is send from MCU to the host PC.

3) User can refer Section 5 "Example Project for IP Protection" in the document to run trustzone EP via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

4) For IAR, user can open non-secure (_ns) project. It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

5) For KEIL, user can open .uvmpw file in keil folder for multiple workspace which has both secure (_s) and non-secure (_ns) project.

6) User must build secure project (_s) first, then build non-secure (_ns) project.

7) The first compiling in non-secure (_ns) of IAR project may have issue with error message, this is because of timing issue 
   between EWARM and RSAC operation which described in step 10 of section 5.5.1 in
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection  

8) After building successfully, user must open Renesas Device Partition Manager -> Initialize device back to factory default -> Set Trustzone 
   secure / non-secure boundaries -> Run non-secure (_ns) project. 

9) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x320025e8
   b. Keil:	Not available
   c. IAR: 0x320060c0
 
10) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
    in .map file generated in the build configuration folder (Debug/Release).
