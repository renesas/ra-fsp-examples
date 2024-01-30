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
***********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the DOC HAL module APIs. The project performs the different event 
        operations selected by the user in configurator properties. Events can be selected viz.,Compare Match, Compare Mismatch, 
        Addition Overflow, Subtraction Underflow and compares with the reference data. Upon Success/Failure of operation, the 
        result will be displayed on RTT viewer.LED will turn ON only if the selected event operation is success.﻿

Note:
1) User can refer Section 5 "Example Project for IP Protection" in the document to run trustzone EP via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

2) For IAR, user can open non-secure (_ns) project. It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

3) For KEIL, user can open .uvmpw file in keil folder for multiple workspace which has both secure (_s) and non-secure (_ns) project.

4) User must build secure project (_s) first, then build non-secure (_ns) project.

5) The first compiling in non-secure (_ns) of IAR project may have issue with error message, this is because of timing issue 
   between EWARM and RSAC operation which described in step 10 of section 5.5.1 in
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection  

6) After building successfully, user must open Renesas Device Partition Manager -> Initialize device back to factory default -> Set Trustzone 
   secure / non-secure boundaries -> Run non-secure (_ns) project. 

7) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x32002468
   b. Keil:	Not Available
   c. IAR: Not Available
 
8) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
