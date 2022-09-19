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
	The example project demonstrates the typical use of the CAC HAL module APIs. 
	On any key press from RTT viewer, CAC module starts measuring the 
        accuracy of selected target clock against the selected reference clock.
        Result of the measurement is displayed on RTTViewer.

NOTE: 
       1. User may select any reference clock and clock divider.
       2. User may select any measurement clock and clock divider.
       3. User must set upper limit and lower limit in configurator based on the selected reference and measurement clocks.

Formula to calculate threshold(upper/lower) limits: 

1. Get Measurement clock frequency with clock divider i.e. say 'M'. 
   for ex. select HOCO(24MHZ on EK_RA4M1) and clock divider as '1', then M = 24MHZ.
2. Get Reference clock frequency with clock divider i.e. say 'R'.
   for ex. select Main OSC(12MHZ on EK_RA4M1) and clock divider as '32', then R = (12MHZ/32).
3. Calculate count value, say 'C'
   i.e. C = (M/R), so C = 64.
4. Take a tolerance of 10%, say 'T'.
   i.e. T = (C * (10/100)) = C/10 = 6.4.
5. Upper limit Threshold = (count value + tolerance) = (C + T) = 70.4
6. Lower limit Threshold = (count value - tolerance) = (C - T) = 57.6

Note :  
* For clock frequencies, refer "Clocks" tab in configurator or board specific user manual.


	
﻿
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000045c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address in .map file generated in the build configuration folder (Debug/Release)
