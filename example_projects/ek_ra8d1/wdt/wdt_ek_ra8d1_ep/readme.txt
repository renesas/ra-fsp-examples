/***********************************************************************************************************************
* 
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
* 
**********************************************************************************************************************/

Project Overview:
   The example project demonstrates the typical use of the WDT HAL module APIs.
   User can give input through JLinkRTTViewer to start the WDT. WDT gets refreshed periodically through GPT timer.
   User can press push button to stop the GPT timer which in turn stops refreshing WDT timer.
   Approximately after 2 seconds, WDT resets the MCU and turn LED ON until user gives the input once again.
   
Note: 
1) On EK-RA2L1/EK-RA2E1/EK-RA4M2/EK-RA4M3/EK-RA6M4/EK-RA4E2/EK-RA6E2/FPB-RA4E1/FPB-RA6E1 board, use Switch S1 to stop WDT from refreshing.

2) On EK-RA2E2/RSSK-RA6T1/EK-RA6M3/MCK-RA6T2/EK-RA8M1/EK-RA8D1 board, use Switch S2 to stop WDT from refreshing.

3) On FPB-RA2E3 board, P206 <---> P200, use Switch S1 to stop WDT from refreshing.

4) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220004b8
   b. Keil:	Not available
   c. IAR: 	0x2200021c
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).