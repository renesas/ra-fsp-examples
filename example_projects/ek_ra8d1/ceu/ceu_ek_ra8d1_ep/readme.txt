/***********************************************************************************************************************
* Copyright [2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
	This Example Project demonstrates the basic functionality of CEU driver on Renesas RA MCUs based on FSP. 
	On successful initialization of CEU and camera setup, user is requested to select memory (1. SDRAM and 2. SRAM)
	to save image. 
	
	By default, the Example Project configures the camera in test pattern mode, and the image captured is a color bar.
	The user can get the actual image by modifying the macro OV3640_TEST_PATTERN in the ov3640.h file with a value of 0U.
	
	Type '1' and enter to select 'SDRAM' to choose SXGA (1280 x 960) format and store the image in SDRAM.
	Type '2' and enter to select 'SRAM' to choose VGA (640 x 480) format and store the image in SRAM.
	The captured image can be viewed in e2studio using the 'image viewer'.

3. Supported Boards:
	EK-RA8D1

3. Hardware Requirements:
	1 x Renesas RA boards: EK-RA8D1
	1 x OV3640 camera module (included in the kit)
	1 x Micro USB cable

4. Hardware Connections:
	Connect the USB Debug port on the EK-RA8D1 board to the host PC via a micro USB cable.
	Connect the camera module to the camera connector J59 on the EK-RA8D1 board.
	Set the configuration switches on the EK-RA8D1 board as below:
		SW1-1 PMOD1: 	OFF
		SW1-2 TRACE: 	OFF
		SW1-3 CAMERA: 	ON
		SW1-4 ETHA: 	OFF
		SW1-5 ETHB: 	OFF
		SW1-6 GLCD: 	OFF
		SW1-7 SDRAM: 	ON
		SW1-8 I3C: 		OFF
NOTE:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200061c
   b. Keil:	Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
3) Refer ceu_notes.md for viewing captured image in e2studio.
