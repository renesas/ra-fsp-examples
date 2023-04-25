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

Project Overview:
    This example project demonstrates basic functionalities of USB Host Mass Storage Class (HMSC) driver
    with FAT file system with Baremetal using FreeRTOS+FAT on Renesas RA MCUs based on Renesas FSP.
    FreeRTOS+FAT uses the blockmedia driver, which uses an underlying USB HMSC driver to perform file operations
    on the storage devices. The project performs basic file operations (open, read, write, format and close) on
    the USB Flash drive.Error and EP status messages will be printed on J-link RTT Viewer.

Hardware Connections:
   EK-RA6M3:
    High-Speed: (For EK-RA6M3/EK-RA6M3G, EK-RA6M5)
        Jumper J7: Connect pins 1-2
        Remove Jumper J17 pins
        Connect USB device to J6 connector with help of micro USB Host cable.

    Full-Speed: (For EK-RA4M3/EK-RA6M3/EK-RA6M3G/EK-RA6M4/EK-RA4M2)
        Jumper J12 placement is pins 1-2
        Remove Jumper j15 pins
        Connect USB device to J11 connector with help of micro USB Host cable.  

Note:
1. By default USB HMSC Example Project runs in Hi-speed mode for Hi-speed supported boards and in Full speed for other boards.
2. Format the USB drive(with FAT32 file system) before performing any operation.
