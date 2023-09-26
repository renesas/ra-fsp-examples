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

1. Project Overview:
    This example project demonstrates basic functionalities of USBX Host Hub driver with AZURE RTOS on Renesas RA MCUs based
    on Renesas FSP. The USB Host Hub functionality is demonstration is done by using HHID devices.USBX HHID driver enumerates 
    Human Interface Devices limited to keyboard or mouse using USBX middleware. Upon Connecting a keyboard and mouse to the hub, 
    pressing a key from the keyboard, the received keycode is displayed on the J-link RTT Viewer. When the mouse is moved, the 
    x and y axis position will be printed on J-link RTT Viewer. Error and info messages will be printed on J-link RTT Viewer.

2. Supported Boards:
    EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5

3. Hardware Requirements:
        1. 1 x RA MCU board
        2. 1 x Micro USB cable for programming and debugging
        3. 1 x Micro USB 2.0 OTG cable
        4. 1 x USB Hub
        5. 1 x USB PC Keyboard
        6. 1 x USB Mouse

4. Hardware Connections:
    EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5 (Full Speed)

    Full-Speed :
        Jumper J12 placement is pins 1-2
        Remove Jumper J15 pins
        Connect the USB Hub to J11 connector with OTG cable.
        Connect mouse and keyboard to the Hub.
    
    Note: EP does not support high speed as USBX HHID does not support it.


1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200012ec
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
