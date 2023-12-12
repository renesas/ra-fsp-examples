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
    This example project demonstrates basic functionalities of USBX Host Mass Storage Class (HMSC) driver
    with AZURERTOS+FAT file system on Renesas RA MCUs based on Renesas FSP. FileX interface on USBX uses the underlying
    HMSC driver to perform various file operations. Error and info messages will be printed on JlinkRTTViewer.

2. Supported Boards:
    EK-RA6M4, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1

3. Hardware Requirements:
    External Hardware : OTG cable: Micro to USB Host (Female)   - 1x
                        USB A to USB Micro B Cable              - 1x
                        USB Pendrive                            - 1x

4. Configuration changes to switch Speed in configurator based on the example project settings: 

     i. Full-Speed : (EK-RA6M4, EK-RA4M2, EK-RA4M3)
        USB Speed                      :    Full Speed
        USB Module Number              :    USB_IP0 Port
        DMA Source Address             :    FS Address
        DMA Destination Address        :    FS Address
        DMAC Transfer Size             :    2 Bytes

    ii. High-Speed : (EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1)
        USB Speed                      :    Hi Speed
        USB Module Number              :    USB_IP1 Port
        DMA Source Address             :    HS Address
        DMA Destination Address        :    HS Address
        DMAC Transfer Size             :    4 Bytes

5. Hardware Connections:
    High-Speed: (EK-RA6M3)
        Jumper j7: Connect pins 1-2
        Remove Jumper j17 pins
        Connect USB device to J6 connector with help of micro USB Host cable.

    High-Speed: (EK-RA6M5, EK-RA8M1, EK-RA8D1)
	Note: For EK-RA8D1, User need to turn OFF SW1-6 to use USBHS
        Jumper j7: Connect pins 1-2
        Remove Jumper j17 pins
        Connect USB device to J31 connector with help of micro USB Host cable.

    Full-Speed: (EK-RA6M4, EK-RA4M2, EK-RA4M3)
        Jumper j12 placement is pins 1-2
        Remove Jumper j15 pins
        Connect USB device to J11 connector with help of micro USB Host cable.

Note:
1) User is expected to execute "Eject" option before removing USB else further file operations may fail and USB data may get corrupted.

2) Operation is not guaranteed for any user input value other than integer, char(i.e. float, special char) through
   JlinkRTTViewer input.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22001290
   b. Keil: 	Not Available 
   c. IAR:  	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).