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
    	The example project demonstrates the typical use of the IIC master HAL module APIs.
    	The project initializes IIC master module with standard rate and interfaces with PmodACL™ Board for ADXL345.
    	On power up after establishing the connection of sensor with RA board, it displays accelerometer axis data on
    	RTTviewer. Any API/event failure will be displayed on RTTviewer.

2. Hardware Settings:
   	Hardware Connection
    	*******************
    	PMOD ACL has two on board connectors, J2 is used for I2C communication.

    	EK-RA6M3G / EK-RA6M3
    	--------
    	SDA2 P511  ----> SDA (on PMOD-ACL)
    	SCL2 P512  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA6M2
    	--------
    	SCL1/P100  ----> SCL (on PMOD-ACL)
    	SDA1/P206  ----> SDA (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA6M1
    	---------
    	SCL0/P400  ----> SCL (on PMOD-ACL)
    	SDA0 P401  ----> SDA (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA6M4 / EK-RA6M5
    	--------
    	SDA0/P401  ----> SDA (on PMOD-ACL)
    	SCL0/P400  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA4M1
    	--------
    	SDA1/P206  ----> SDA (on PMOD-ACL)
   	SCL1/P100  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA2A1
    	--------
    	SDA0/P401  ----> SDA (on PMOD-ACL)
    	SCL0/P000  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	RSSK-RA6T1
    	----------
    	SDA1/P101  ----> SDA (on PMOD-ACL)
    	SCL1/P100  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA2L1
    	--------
    	SDA0/P407  ----> SDA (on PMOD-ACL)
    	SCL0/P408  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA4M3
    	--------
    	SDA0/P401  ----> SDA (on PMOD-ACL)
    	SCL0/P408  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA2E1
    	--------
    	SDA0/P401  ----> SDA (on PMOD-ACL)
    	SCL0/P400  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA4M2
    	--------
    	SDA1/P206  ----> SDA (on PMOD-ACL)
    	SCL1/P205  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	FPB-RA6E1/FPB-RA4E1/FPB-RA2E3
    	---------
    	SDA0/P401  ----> SDA (on PMOD-ACL)
    	SCL0/P400  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	MCK-RA6T2
    	---------
    	SDA0/PB07  ----> SDA (on PMOD-ACL)
    	SCL0/PB06  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	EK-RA8M1/EK-RA8D1
    	--------
    	SDA1 P511  ----> SDA (on PMOD-ACL)
    	SCL1 P512  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

    	MCK-RA8T1
    	--------
    	SDA1/P206 (CN2:P24)  ----> SDA (on PMOD-ACL)
    	SCL1/P205 (CN2:P25)  ----> SCL (on PMOD-ACL)
    	VCC pin   (CN1:P24)  ----> VCC (on PMOD-ACL)
    	GND pin   (CN1:P34)  ----> GND (on PMOD-ACL)
    
    	EK-RA2A2
    	--------
    	SDA0 P410  ----> SDA (on PMOD-ACL)
    	SCL0 P411  ----> SCL (on PMOD-ACL)
    	VCC pin    ----> VCC (on PMOD-ACL)
    	GND pin    ----> GND (on PMOD-ACL)

Note:
1) For the functioning of IIC Master on all the boards except for EK-RA6M3,EK-RA6M3G,FPB-RA6E1,FPB-RA4E1,EK-RA8M1,FPB-RA2E3,EK-RA8D1 external pull up resistors of value
   3.9 or 4.7K ohms are required to be connected on I2C(SDA/SCL) lines.﻿

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000060c
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

4) To enable printing floats to RTT Viewer, edit the project settings and make sure use nano with printf is enabled. 
   The setting can be found by Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Misc > Use float with nano printf
