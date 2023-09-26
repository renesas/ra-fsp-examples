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

    This project demonstrates the basic functionality of Netx Crypto on Renesas RA MCUs based on SCE9 Hardware acceleration operating in an AzureRTOS environment.

The example project executes ECC,RSA and AES algorithms(including different chaining modes) each in its own thread in multi threaded environment.

From the RTT viewer, user can enter any key to enable concurrent execution of the different algorithms.

The algorithms being executed, and the operational status will be displayed on Jlink RTT viewer.



2. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below instructions to observe the operation of the system.
	Supported RA Boards:
	SCE9 - EK_RA4M2, EK_RA4M3, EK_RA6M4, EK_RA6M5

3. Hardware and Software Requirements:
   
   Hardware : 
	1) RA board 
	2) 1x Micro USB device cable
	3) A PC running Windows 10 with at least 1 USB port for debug
    
   Software: 
	1) Jlink RTT viewer

NOTE:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200009ec
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).