/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

This project demonstrates the basic functionalities of USBX OTG CDC driver on Renesas RA MCUs based on Renesas FSP using Azure RTOS.
Two RA MCU boards(Device-A and Device-B) will be connected to back to back using USB OTG Board(EB-USB-OTG) and its associated OTG cable.
Initially Device-A will act as a CDC host device and Device-B will act as a CDC peripheral device. The host (Device-A) will send data to 
the peripheral(Device-B) and peripheral(Device-B) will echo the data back to host(Device-A). Upon pressing (PUSH BTN)SW1 on Peripheral(Device-B),
the EP will switch the roles of the two devices, i.e. host acting as peripheral and peripheral as host. After switching roles, the new host 
device(Device-B) will send data to the new peripheral device(Device-A) and the peripheral device(Device-A) will echo the data back to host(Device-B).
USBX status and any errors will be printed on the JLinkRTTViewer which will be handled in a separate thread.
 
2. To run this example project, the user needs to compile and download the example project to the evaluation kits and follow the below instructions to observe the operation of the system.

   Supported Board: EK RA6M2

	1) Connect the USBX OTG CDC host Device A (MCU 1) to USBX OTG CDC peripheral Device B (MCU 2) using USB OTG Board(EB-USB-OTG) and its associated OTG cable.
   	2) Connect the USB J-Link debug ports of both RA MCU Boards(Device-A and Device-B) to the PC(USB Ports) for J-Link RTT Viewer messages.
	3) USBX OTG CDC host Device A will send a message to USBX OTG CDC peripheral Device B. The USBX OTG CDC-B device will echo back the message to the USBX OTG CDC-A device.	
	4) Press the (PUSH BTN)SW1 switch on the USBX OTG CDC Peripheral(Device-B). Device roles are changed, i.e. host acting as peripheral and peripheral as host. After switching roles, the new host will be device(Device-B) and new peripheral device will be (Device-A).
	5) After switching device roles, USBX OTG CDC new host (Device B) sends a message to USBX OTG CDC peripheral (Device A). The USBX OTG CDC peripheral (Device-A) will echo back the message to the USBX OTG CDC new host (Device B).
	6) The message exchanged between host and peripheral device can be verified with JLink RTT Viewer on the connected PC.
	7) LED blinking can be verified on the peripheral device.
	8) Visit markdown file "Verifying operation" section for further information on project setup and verification.
   
   Hardware Connections:
   
    1) Connect P1(All Pins) of EB-USB-OTG1 to J1(All Pins) of EK-RA6M2.	
	2) Connect P2(All Pins) of EB-USB-OTG1 to J2(All Pins) of EK-RA6M2.
	
     
3. Hardware and Software Requirements:
   
   Hardware : 
	1) Renesas RA board: 2 X EK-RA6M2
	2) 2 X EB-USB-OTG1 board.(https://app.circuithub.com/projects/REA-MLP/EB-USB-OTG1_v1/revisions/35287/project)
	3) 2 X Micro USB cable.
	4) 1 X Micro USB OTG cable.
    
   Software: 
	1) e2studio.
	2) JLink RTTViewer.
	3) FSP v3.8.0 or higher.
		

	

NOTE: USBX OTG-CDC Example Project runs on USB Full-speed mode only. USB High-speed mode is not supported.  

