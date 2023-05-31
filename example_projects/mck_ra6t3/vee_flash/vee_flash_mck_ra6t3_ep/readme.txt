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

Project Overview:
    This example project demonstrates basic functionalities of vee flash driver on Renesas RA MCUs based on Renesas FSP.
    Based on User input, EP performs following operations :
    1. Writing different data types to the different record IDs of vee flash.
    2. Reading back the data from the different record IDs.
    3. Format operation.
    4. Get the status of Virtual EEPROM.
    5. Refresh operation.   

    On successful completion of each operation, success message will be displayed on the Jlink RTTViewer.
    Error and info messages will be printed on JlinkRTTViewer.


1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000020c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

3) To enable printing floats to RTT Viewer, edit the project settings and make sure use nano with printf is enabled.
   The setting can be found by Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Misc > Use float with nano printf
