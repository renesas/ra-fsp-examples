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

    RA6M3G_EK / RA6M3_EK
    --------
    SDA2 P511  ----> SDA (on PMOD-ACL)
    SCL2 P512  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA6M2_EK
    --------
    SCL1/P100  ----> SCL (on PMOD-ACL)
    SDA1/P206  ----> SDA (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA6M1_EK
    ---------
    SCL0/P400  ----> SCL (on PMOD-ACL)
    SDA0 P401  ----> SDA (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA6M4_EK / EK-RA6M5
    --------
    SDA0/P401  ----> SDA (on PMOD-ACL)
    SCL0/P400  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA4M1_EK
    --------
    SDA1/P206  ----> SDA (on PMOD-ACL)
    SCL1/P100  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA2A1_EK
    --------
    SDA0/P401  ----> SDA (on PMOD-ACL)
    SCL0/P000  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA6T1_RSSK
    ----------
    SDA1/P101  ----> SDA (on PMOD-ACL)
    SCL1/P100  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA2L1_EK
    --------
    SDA0/P407  ----> SDA (on PMOD-ACL)
    SCL0/P408  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA4M3_EK
    --------
    SDA0/P401  ----> SDA (on PMOD-ACL)
    SCL0/P408  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA2E1_EK
    --------
    SDA0/P401  ----> SDA (on PMOD-ACL)
    SCL0/P400  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA4M2_EK
    --------
    SDA1/P206  ----> SDA (on PMOD-ACL)
    SCL1/P205  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA6E1_FPB/RA4E1_FPB
    ---------
    SDA0/P401  ----> SDA (on PMOD-ACL)
    SCL0/P400  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    MCK_RA6T2
    ---------
    SDA0/PB07  ----> SDA (on PMOD-ACL)
    SCL0/PB06  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

Note:
    For the functioning of IIC Master on all the boards except for EK-RA6M3/EK-RA6M3G,FPB-RA6E1,FPB-RA4E1, external pull up resistors of value
    3.9 or 4.7K ohms are required to be connected on I2C(SDA/SCL) lines.﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000614
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
