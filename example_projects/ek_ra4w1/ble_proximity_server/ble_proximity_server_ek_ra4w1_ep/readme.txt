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

This project demonstrates the basic functionality of an RA4 MCU operating as a BLE Server by establishing a connection with Renesas 
browser installed on the device client. An RA MCU using freeRTOS and the Renesas FSP was used to create this example project. This 
EP "ra4w1_proximity_server" is configured to establish the connection with the Renesas GATTBrowser installed on the client device. Led1 
on the server board will provide visual feedback to indicate once the client has established connection or when a disconnection 
occurs. LED1 is turned on when the connection is established. It will turn off if the client is disconnected. The user can send 
alert levels, connect requests, or disconnect requests from the Renesas GATT Browser on the client device. LED0 blink speed is used 
to indicate which user alert level value is changed. If user chooses alert level 0, the LED0 does not blink(it remains lighted). If user 
chooses alert level 1, then LED0 blinks slow. If the user chooses alert level 2. then LED0 blinks fast. LED0 Alert levels set for both 
operation of Immediate Alert Level and Link Loss Alert Level. The user also can read and test Tx_power level (TPS) and test Receive Signal 
Strength Indicated (RSSI). The connection status and LEDs status messages will display on Jlink RTT viewer.

  	
2. Hardware Requirement:

	i. 1x micro usb cable.
        ii. 1x board of EK-RA4W1.
        iii. 1x mobile device with Renesas GATT Browser installed.

3. Hardware Connections:
 	Supported Board EK_RA4W1:		
	i. Connect RA board to Host machine using micro usb cable.
       	
4. Software requirements:
 	1) Device client (iphone or ipad,..v.v.) has Renesas GATT browser installed.
	2) Renesas Flexible Software Package (FSP)
	3) e2 studio: Version 2024-04
	4) SEGGER J-Link RTT Viewer: Version 7.96j


Note: After running EP "ra4w1_proximity_server". The User must use Renesas GATT browser to search and connect to Local Name "RA4W1_SRV" 
	  displayed on your device RA4W1_SRV. Use Write without response "Immediate Alert Level" to set Immediate Alert Level. The User can 
	  Write and Read "Link Loss Level", Read TX Power and RSSI from your device GATTBrowser and the messages will be shown on RTT Viewer.
      



