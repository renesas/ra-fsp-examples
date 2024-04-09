/***********************************************************************************************************************
* Copyright [2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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

      This project demonstrates the basic functionality of the USBX Peripheral Printer driver on Renesas RA MCUs based on 
      Renesas FSP using Azure RTOS. An RA MCU board will be connected to the PC where the RA board will act as a peripheral 
      printer and the PC (Windows System) will act as a host device. A print command for a test file will be sent from the 
      host PC, and printing data will be transferred from the host PC to MCU. RA MCU will display Windows machine printer 
      information, settings, and printing data on the J-Link RTT Viewer. Error and info messages will be printed on J-Link RTT Viewer.

      Following is the sequence used by the application.

	1. Connect two USB Cables to the PC : One for USB Debug port and other for USB HS/FS port.
	2. In Device Manger, printer should be detected as "Generic / Text Only" under Print queues section.
	3. On the Host PC, select any text file or print Test page from the printer properties and execute print command.
	4. MCU will receive the print command and start printing the data.
	5. The printer settings and printing data will be displayed on the J-Link RTT Viewer.


2. Hardware Requirement:

	1. RA MCU board
	2. 2x Micro USB cables
	3. Host System (Windows PC) with at least 2 USB ports

3. Hardware Connections:
	
	User needs to confirm the default USB speed selection used in the example project prior to set up the hardware connection.
	
	Supported Board (Full Speed)- EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1:		
	1. Connect RA board to Host PC machine using micro USB cable.
	2. Connect another cable to the Full Speed port of the RA MCU board, connect RA board directly to PC (Not use any USB Hubs to connect).
	
	Supported Board (High Speed)- EK-RA6M3, EK-RA6M5, EK-RA8M1:		
	1. Connect RA board to Host PC machine using micro USB cable.
	2. Connect another cable to the Full Speed port of the RA MCU board, connect RA board directly to PC (Not use any USB Hubs to connect).

4. Hardware settings for the project:
	
    Jumper Settings:
	
	EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1 (Full Speed)
	1. Jumper J12: Connect pins 2-3
	2. Connect Jumper J15 pins
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	port (J11) of the board. Connect the other end of this cable to USB port of the host PC.
	
	EK-RA6M3 (High Speed)
	1. Jumper J7: Connect pins 2-3
	2. Connect Jumper J17 pins
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed
	port (J6) of the board. Connect the other end of this cable to USB port of the host PC.
	
	EK-RA6M5, EK-RA8M1(High Speed)
	1. Jumper J7: Connect pins 2-3
	2. Connect Jumper J17 pins
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed
	port (J31) of the board. Connect the other end of this cable to USB port of the host PC.

5. Configuration changes to switch Speed in configurator:

    	i.  Full-Speed : (EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4)
            USB Speed                     :    Full Speed
            USB Module Number             :    USB_IP0 Port

        ii. High-Speed :(EK-RA6M3, EK-RA6M5, EK-RA8M1)
            USB Speed                     :    Hi Speed
            USB Module Number             :    USB_IP1 Port

NOTE:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000a40
   b. Keil: Not Available 
   c. IAR: Not Available
   
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

