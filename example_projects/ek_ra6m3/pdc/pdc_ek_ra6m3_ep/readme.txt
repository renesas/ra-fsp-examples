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
        This Example Project demonstrates the basic functionality of PDC driver on Renesas RA MCUs based on FSP. 
        On successful initialization of PDC and camera setup, user is requested to enter any key through RTTViewer
        to start capturing the image. The captured image can be viewed in e2studio using the 'image viewer'.

2. Hardware Requirements:
    External Hardware : ov7670 camera.

3. Hardware Connections:
    For EK-RA6M3 & EK-RA6M3G
    Tracecut following jumper  
    Jumper E19, E20, E25, E23, E22, E21, E16, E15, E14, E17 on the board.

    Connection detail 
       Camera Pin     Board/Jumper
        3.3            VCC	
        GND	       GND
        SCL            P512 (J1 Pin 5/SCL2)
        SDA            P511 (J1 Pin 21/SDA2
        VS             P402 (J3 Pin 12)
        HS             P704 (J3 Pin 19)
        PCLK           P705 (J3 Pin 22)
        PIXCLK         P708 (J3 Pin 29)
        D7             P403 (J3 Pin 11)
        D6             P404 (J3 Pin 14)
        D5             P405 (J3 Pin 13)
        D4             P406 (J3 Pin 16)
        D3             P700 (J3 Pin 15)
        D2             P701 (J3 Pin 18)
        D1             P702 (J3 Pin 17)
        D0             P703 (J3 Pin 20)
        RESET          P613 (J2 Pin 32)
        PWDN           P614 (J2 Pin 21)

    For RA6M2
        Camera Pin     Board/Jumper
        3.3	       VCC	
        GND            GND	
        SCL            P400 (J4 Pin 13/SCL0)
        SDA            P401 (J4 Pin 11/SDA0)
        VS             P402 (J1 Pin 27)
        HS             P704 (J4 Pin 01)
        PCLK           P705 (J2 Pin 39)
        PIXCLK         P708 (J2 Pin 07)
        D7             P403 (J4 Pin 02)
        D6             P404 (J4 Pin 04)
        D5             P405 (J4 Pin 06)
        D4             P406 (J4 Pin 08)
        D3             P700 (J4 Pin 09)
        D2             P701 (J4 Pin 07)
        D1             P702 (J4 Pin 05)
        D0             P703 (J4 Pin 03)
        RESET          P800 (J4 Pin 30)
        PWDN           P801 (J4 Pin 29)
    
    
                                    
NOTE:
1. For the functioning of PDC on RA6M2 board, external pull up resistors of value
   3.9 or 4.7k ohms are required to be connected on I2C(SDA/SCL) lines.
2. Image viewer is added by:
    i.  Add the buffer to the "Memory" monitor feature in Debug window
    ii. Choose "New Rendering" option in Memory monitor and add rendering as mentioned below for
        the buffer.
        Rendering - Raw Image
        width -  480
        height - 272
        Encoding format - RGB565
        Line alignment  - 32bytes