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
  	
	This project demonstrates the basic functionality of the AWS HTTPS client library with Ethernet driver on Renesas RA MCUs
	based on Renesas FSP using FreeRTOS. The AWS client is used to connect to the HTTPS adafruit server, which is a cloud platform. 
	Upon successful connection, the menu is displayed allowing the user to send GET, PUT, POST requests to the adafruit server. 
	On POST/PUT request, MCU die temperature is read via(using) ADC and uploaded to server. On GET request, the most recently uploaded
	MCU die temperature data is read from the adafruit server. JLink RTT Viewer is used to display the status and responses of the 
	requests made to the server.

2. Hardware Requirement:

        i. micro usb cable - 1x
       ii. Ethernet Cable (RJ45 CAT5/6) - 1x
      iii. (Ethernet Switch or Router with Ethernet port) connecting to Internet- 1x

3. Hardware Connections:
	Supported Board EK_RA6M3, EK_RA6M5: 
	i. Connect the board to Host machine using micro usb cable. 
       ii. Connect an Ethernet cable from the Ethernet port on the RA boards to an Ethernet switch.
	   The Ethernet switch should have a WAN connection to communicate to the server over the internet. 
	   The Ethernet switch is connected to the internet through a router.


NOTE  : On RA MCUs with TrustZone, IDAU boundaries are programmed by this project due to the use of Ethernet and EDMAC peripherals.
        Consequentially, it is necessary to connect the serial programming interface to meet this requirement.
	