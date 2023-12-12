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
	This example project demonstrates basic functionalities of OSPI_B driver on Renesas RA MCUs based on Renesas FSP. 
    In Main menu, based on RTT input, user selects mode for opsi_b module.
        1. SPI 1S-1S-1S protocol mode.
        2. OPI 8D-8D-8D protocol mode.
    In Sub Menu, based on RTT input, user selects operations to perform.
        1. Write data to OSPI flash device.
        2. Read back the data from OSPI flash device.
        3. Erase a sector of OSPI flash device.
        4. Go back to main menu. 
      
    On successful completion of each operation, a success message and execution time of operation will be displayed on J-link RTT Viewer.
	Error messages and information will be printed on J-link RTT Viewer.

3. Supported Boards:
	EK-RA8M1, EK-RA8D1

3. Hardware Requirements:
	1 x Renesas RA boards: EK-RA8D1
	1 x Micro USB cable

4. Hardware Connections:
	Connect the USB Debug port on the EK-RA8M1 board to the host PC via a micro USB cable.

NOTE:
	1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
	RTT Block address for hex file committed in repository are as follows:
	a. e2studio: 0x22000628
	b. Keil: Not Available
	c. IAR: Not Available
	2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
	in .map file generated in the build configuration folder (Debug/Release).