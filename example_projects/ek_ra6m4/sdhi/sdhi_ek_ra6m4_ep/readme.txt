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
    This example project demonstrates basic functionalities of sdhi driver with FreeRTOS+FAT file system on Renesas
    RA MCUs based on Renesas FSP. FreeRTOS+FAT uses the underlying Block media driver.
    The Block media driver utilizes SDHI driver to perform file operations on the SD Card.
    Error and info messages will be printed on JlinkRTTViewer.

Configuration changes to switch Bus Width in configurator: (Same for EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G and EK-RA6M4 boards)

    For 4 Bit Width
    Under SDMMC Stack
        Bus Width: 4 Bits
        
    For 1 Bit Width
    Under SDMMC Stack
        Bus Width: 1 Bits

2. Hardware Requirements:
    External Hardware : PMOD SD Full sized SD Card slot

3. Hardware Connections:

For EK-RA6M3 & EK-RA6M3G
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P415(CD)     --->    PMOD SD Pin9(CD)
        P414(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M2
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P210(CD)     --->    PMOD SD Pin9(CD)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        Tracecut Jumper E15 and solder to E12
        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M1
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P210(CD)     --->    PMOD SD Pin9(CD)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        Tracecut Jumper E15 and solder to E12
        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M4
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P210(CD)     --->    PMOD SD Pin9(CD)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M5
    1 Bit Width

        P412(CMD)(J1 Pin34) ---> PMOD SD Pin2(MOSI)
        P411(DAT0)(J1 Pin35) ---> PMOD SD Pin3(MISO)
        P413(CLK)(J1 Pin33) ---> PMOD SD Pin4(SCLK)
        GND ---> PMOD SD Pin5(GND)
        +3V3(VCC) ---> PMOD SD Pin6(VCC)
        P210(CD)(J2 Pin17) -→ PMOD SD Pin9(CD)
        P209(WP)(J2 Pin18) ---> PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width.In addition below connections


        P410(DAT1)(J1 Pin36) ---> PMOD SD Pin7(DAT1)
        P206(DAT2)(J2 Pin4) ---> PMOD SD Pin8(DAT2)
        P205(DAT3)(J2 Pin5) ---> PMOD SD Pin1(CS)


For FPB_RA6E1
    1 Bit Width

       P412(CMD)(J3 Pin20) ---> PMOD SD Pin2(MOSI)
       P411(DAT0)(J3 Pin21) ---> PMOD SD Pin3(MISO)
       P413(CLK)(J3 Pin19) ---> PMOD SD Pin4(SCLK)
       GND ---> PMOD SD Pin5(GND)
       +3V3(VCC) ---> PMOD SD Pin6(VCC)
       P210(CD)(J3 Pin35) -→ PMOD SD Pin9(CD)
       P209(WP)(J3 Pin36) ---> PMOD SD Pin10(WP)

    4 Bit Width
       Above mentioned connections for 1 Bit Width. In addition below connections


       P410(DAT1)(J3 Pin22) ---> PMOD SD Pin7(DAT1)
       P206(DAT2)(J3 Pin31) ---> PMOD SD Pin8(DAT2)
       P205(DAT3)(J3 Pin32) ---> PMOD SD Pin1(CS)

Note:
0. Connect PMOD: use short wiring connections (should be shorter than 10cm)
1. Format the SD Card before performing any operation.
2. Data written can also be manually verified in file ra_sdhi.txt.
3. User is expected to execute "safely remove SD Card" option before removing SD Card else further file operations may fail and
   SD Card data may get corrupted.
4. User has to remove and re-insert the SD Card after executing the "safely remove SD Card" & "Initialize FreeRTOS+FAT" option.
5. If SD Card is removed without "safely remove SD Card" option, user is recommended to perform "safely remove SD Card" and
   "Initialize FreeRTOS+FAT" commands in sequence. This will make sure no operations fails after unexpected removal of SD Card.
6. User is expected to enter RTT input data of size not exceeding 20 bytes.
7. Operation is not guaranteed for any user input value other than integer, char(i.e. float, special char) through
   JlinkRTTViewer input.
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200000f4
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
