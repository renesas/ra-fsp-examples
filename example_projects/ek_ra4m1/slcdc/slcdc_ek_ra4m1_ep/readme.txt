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
    This example project demonstrates the typical use of the SLCDC HAL module APIs.
    The project displays digits from 990 to 9999 on Segment LCD with increment of 21.
    If an error occurs in the operation, the project displays "Err" on the LCD screen.

2. Hardware and Software Requirements:
    External Hardware:
        Segment LCD: Lumex LCD-S401M16KR (https://www.mouser.in/datasheet/2/244/lumex_lumx-s-a0001420318-1-1737691.pdf)
        Capacitors: Four 0.47uF 
        Connecting wires.

3. Hardware settings for the project:
    Wiring details:
    LCD Pin 1  --> P104 (COM0)
    LCD Pin 2  --> P105 (COM1)
    LCD Pin 3  --> P106 (COM2)
    LCD Pin 4  --> P107 (COM3)
    LCD Pin 5  --> P301 (SEG1)
    LCD Pin 6  --> P302 (SEG2)
    LCD Pin 7  --> P303 (SEG3)
    LCD Pin 8  --> P400 (SEG4)
    LCD Pin 9  --> P401 (SEG5)
    LCD Pin 10 --> P402 (SEG6)
    LCD Pin 11 --> P411 (SEG7)
    LCD Pin 12 --> P410 (SEG8)
    Open Trace-cut E3 and Isolate P106 from User LED (COM2 Pin).

    Additional wiring required as per RA4M1 HW manual (See RA4M1 User's Manual (R01UH0887EJ0100) section 45.7.2 Notes)
    
    Connect/Short P111 (CAPH) & P112 (CAPL) with a non polar capacitor of 0.47uF value.
    Connect each of P100 (VL1), P101 (VL2) & P103 (VL4) pins with a 0.47uF capacitor as shown in User's Manual.
    
