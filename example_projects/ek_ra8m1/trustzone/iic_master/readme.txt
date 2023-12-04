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

    RA6M4_EK / EK-RA6M5
    --------
    SDA0/P401  ----> SDA (on PMOD-ACL)
    SCL0/P400  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA4M3_EK
    --------
    SDA0/P401  ----> SDA (on PMOD-ACL)
    SCL0/P408  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA4M2_EK
    --------
    SDA1/P206  ----> SDA (on PMOD-ACL)
    SCL1/P205  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA8M1_EK
    --------
    SDA1 P511  ----> SDA (on PMOD-ACL)
    SCL1 P512  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

Note:
1) For the functioning of IIC Master on all the boards except for EK-RA6M3/EK-RA6M3G,EK-RA8M1 external pull up resistors of value
   3.9 or 4.7K ohms are required to be connected on I2C(SDA/SCL) lines.﻿

2) User can refer Section 5 "Example Project for IP Protection" in the document to run trustzone EP via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

3) For IAR, user can open non-secure (_ns) project. It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

4) For KEIL, user can open .uvmpw file in keil folder for multiple workspace which has both secure (_s) and non-secure (_ns) project.

5) User must build secure project (_s) first, then build non-secure (_ns) project.

6) The first compiling in non-secure (_ns) of IAR project may have issue with error message, this is because of timing issue 
   between EWARM and RSAC operation which described in step 10 of section 5.5.1 in
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection  

7) After building successfully, user must open Renesas Device Partition Manager -> Initialize device back to factory default -> Set Trustzone 
   secure / non-secure boundaries -> Run non-secure (_ns) project. 

8) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x32002628
   b. Keil:	Not available
   c. IAR: 	Not available
 
9) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
