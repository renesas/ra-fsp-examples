/***********************************************************************************************************************
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
* **********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USB Multiport on Renesas RA MCUs
	based on Renesas FSP using FreeRTOS. USB PCDC is configured for full speed with priority
	lower than USB HMSC configured as high speed. Jlink RTTViewer is used for the input and
	status display. Tera term is used to display the output and also to capture the data from
	it based on user input on Jlink RTT viewer. The project checks for the test.txt and
	reads the content and sends the data to usb_pcdc which is displayed on the Tera term. Data
	captured from Tera term(max 512 bytes) will be written to file.
	
2. Hardware and Software Requirements:
   
   	Hardware : 
	  1) RA board (EK-RA6M3/EK-RA6M3G/EK-RA6M5/EK-RA8M1/EK-RA8D1).
	  2) Micro USB device cable - 2Nos.
	  3) A PC with at least 2 USB port(1 for debug and 1 for com port).
	  4) USB flash drive - 1Nos.
	  5) USB OTG cable   - 1Nos.
    
	Software: 
	  1) Serial Terminal/TeraTerm.
    	  2) Windows® 10 operating system.
	  3) USB Serial Drivers (included in Windows 10). 
	
3. Hardware Connections:
        *Note: User need to connect both USBFS and USBHS to run EP.
    	High-Speed (EK-RA6M3/EK-RA6M3G):
          Jumper j7: Connect pins 1-2.
          Remove Jumper j17 pins.
          Connect USB flash device to J6 connector with help of micro USB Host cable.

 	High-Speed (EK-RA6M5/EK-RA8M1/EK-RA8D1):
          Jumper j7: Connect pins 1-2.
          Remove Jumper j17 pins.
          Connect USB flash device to J31 connector with help of micro USB Host cable.

    	Full-Speed:
          Jumper j12 placement is pins 2-3.
          Connect Jumper j15 pins.
          Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	  port (J11) of the board.  Connect the other end of this cable to USB port of the 
	  host PC. 

Note:
1) USB Mass Storage Device must be formatted with FAT file system.

2) Data written can also be manually verified in file.

3) If the respective board is not powered through micro-AB USB (DEBUG USB) port (J11)
   from the host PC, then the functionality cannot be demonstrated.﻿

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000bc0
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
