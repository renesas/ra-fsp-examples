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
    This example project demonstrates basic functionalities of USBX Host HID driver with AZURE RTOS on Renesas RA MCUs based
    on Renesas FSP. USBX HHID driver enumerates Human Interface Devices limited to keyboard or mouse using USBX middleware. 
    Upon Connecting a keyboard and pressing a key, the received keycode is displayed on the Jlink RTTViewer. 
    Upon connecting a mouse, the x and y axis position will be printed on Jlink RTTViewer. The updated value of x and y axis 
    prints only when there is change in X and Y Position. Error and info messages will be printed on JlinkRTTViewer.

2. Supported Boards:
    EK-RA6M4, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M5

3. Hardware Requirements:
    External Hardware : Micro USB 2.0 OTG cable  - 1x
                        USB PC Keyboard - 1 x
                        USB Mouse       - 1 x

4. Hardware Connections:
    EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3 (Only supported on these boards & with Full Speed mode)

    Full-Speed :
        Jumper J12 placement is pins 1-2
        Remove Jumper J15 pins
        Connect USB keyboard to J11 connector with help of OTG cable.
    
    Note: EP does not support high speed as USBX HHID does not support it.

5. Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000131c
   b. Keil: Not Available 
   c. IAR:  Not Available