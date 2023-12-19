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
	This project demonstrates basic functionality of USBX Peripheral Human Interface Device Class module on Renesas RA MCUs 
	based on Renesas FSP using Azure RTOS. User will be able to use RA board as a keyboard(HID Device) by connecting it to 
	the Host PC. Once the board is connected, user can provide any input from RTT Viewer to send character from a - z & numbers 
	0 - 9 from RA board to host PC. USB status and any error messages are displayed on RTT Viewer.

2. Hardware Requirement:
	Micro USB Cable : 2x
	
3. Hardware connection :
	EK-RA6M1,EK-RA6M2:(support only full speed)
	  Connect the micro USB end of the micro USB cable to micro-AB USB Full Speed
	  port (J9) of the board.  Connect the other end of this cable to USB port of the
	  host PC.
	
	EK-RA6M3,EK-RA6M4,EK-RA6M5,EK-RA4M3,EK-RA4M2, EK-RA8M1, EK-RA8D1:(support only full speed)
	  Jumper J12 placement is pins 2-3.
	  Connect jumper J15 pins.
	  Connect the micro USB end of the micro USB cable to micro-AB USB Full Speed
	  port (J11) of the board.  Connect the other end of this cable to USB port of the
	  host PC. 

Note:
1) Known limitation: the USBX_phid USB full speed configuration example project does not work on PCs with Crowdstrike
   software installed.

2) Open Device Manager, and check 'Human Interface Devices', to see ' USB Input Device' has been detected.

3) Lower case a - z and numbers 0 - 9 are sent from MCU to the host PC.﻿

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220009f0
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
