/***********************************************************************************************************************
* Copyright [2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
        The example project the capability of the EK and FSP by usign multiple preipherals.
        The project initializes the Ethernet, USB,ADC and GPIO modules to create a simple web server, web client with
        contro via a USB Comm Device class Termial.
        Once programmed the user can control the board by simulating button presses on a web browser and see regulaly
        updated board configuration on the screen.

2. Hardware settings for the project:
        Please configure the Jumper settings as described in the Quick Start Guide.
	Link to access Quick Start Guide - https://www.renesas.com/us/en/document/qsg/ek-ra6m5-quick-start-guide

3. Configuration Settings:
        Please explore the configuration settings of each module in the RA configurator.

NOTE - Ethernet Mask Setting
----------------------------
When using the Ethernet demonstrations in this demo (Web Server or Network Name Lookup) please ensure the subnet specified 
is the same on the PC and the RA Demonstration code. The subnet is specified as the bits in the "mask" that are set to 1. 
This ensures all devices are on the same subnet and will be able to communicate.

For example:

Where the Net Mask used is:
255:255:255:0

The PC and RA Demonstration must use the same initial three octets, and the final octet must be different:
PC: 192.168.99.100
RA: 192.168.99.200

NOTE - LittleFS stack use
-------------------------
To use LittleFS in the crypto library you must set an additional heap. 

Seting the heap 
[1] Open the configuration.xml with the FSP Configuration viewer 
[2] Select the 'BSP' tab.
[3] Open the Properties view
[4] Scroll to the RA Common section and expand the drop down list
[5] Select Heap Size and set as required by default this sample uses a heap of 0x1000 (4K Bytes)

If you remove the dependancy on littleFS this heap can be reclaimed.