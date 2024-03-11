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
	The example project shows the operation of the FileX file system on block media via the SDHI driver on the RA MCU.
The FileX file system with exFAT format enabled, the project will perform various file system operations based on 
the user's selection from the menu options, such as media management (open, get property, format, close),
directory management (create, get property, delete), and file management (create, write, read, delete).
	The J-Link RTT Viewer is used as the user interface. Menu options and system messages (errors and information messages)
will be printed on Jlink RTT Viewer during the execution of the project.

2. Hardware Requirements:
 	1 x Renesas RA boards
	1 x PmodSD module
	1 x Full-sized SD card or Micro SD card with full-sized adapter
	1 x Micro USB cable
	12 x Jumper cable (both ends female)

3. Hardware Connections:
	For EK-RA4M2: (SDHI 4 bits width by default)
		1 Bit Width:
			P412(CMD) (J1 Pin22)   --->   PMOD SD Pin2(MOSI)
			P411(DAT0)(J1 Pin21)   --->   PMOD SD Pin3(MISO)
			P413(CLK) (J1 Pin19)   --->   PMOD SD Pin4(SCLK)
			GND                    --->   PMOD SD Pin5(GND)
			+3V3(VCC)              --->   PMOD SD Pin6(VCC)
			P210(CD)  (J4 Pin13)   --->   PMOD SD Pin9(CD)
			P209(WP)  (J4 Pin14)   --->   PMOD SD Pin10(WP)

		4 Bit Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			P410(DAT1)(J1 Pin24)   --->   PMOD SD Pin7(DAT1)
			P206(DAT2)(J4 Pin9)    --->   PMOD SD Pin8(DAT2)
			P205(DAT3)(J4 Pin10)   --->   PMOD SD Pin1(CS)

	For EK-RA4M3: (SDHI 4 bits width by default)
		1 Bit Width:
			P412(CMD) (J1 Pin34)   --->   PMOD SD Pin2(MOSI)
			P411(DAT0)(J1 Pin35)   --->   PMOD SD Pin3(MISO)
			P413(CLK) (J1 Pin33)   --->   PMOD SD Pin4(SCLK)
			GND                    --->   PMOD SD Pin5(GND)
			+3V3(VCC)              --->   PMOD SD Pin6(VCC)
			P210(CD)  (J4 Pin19)   --->   PMOD SD Pin9(CD)
			P209(WP)  (J4 Pin20)   --->   PMOD SD Pin10(WP)

		4 Bit Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			P410(DAT1)(J1 Pin36)   --->   PMOD SD Pin7(DAT1)
			P206(DAT2)(J4 Pin10)   --->   PMOD SD Pin8(DAT2)
			P205(DAT3)(J4 Pin11)   --->   PMOD SD Pin1(CS)

	For EK-RA6M1: (SDHI 4 bits width by default)
		1 Bit Width:
			P412(CMD) (J2 Pin1)    --->   PMOD SD Pin2(MOSI)
			P411(DAT0)(J2 Pin20)   --->   PMOD SD Pin3(MISO)
			P413(CLK) (J2 Pin3)    --->   PMOD SD Pin4(SCLK)
			GND                    --->   PMOD SD Pin5(GND)
			+3V3(VCC)              --->   PMOD SD Pin6(VCC)
			P210(CD)  (J4 Pin22)   --->   PMOD SD Pin9(CD)
			P209(WP)  (J4 Pin24)   --->   PMOD SD Pin10(WP)

		4 Bit Width: Tracecut Jumper E15 and solder to E12. Above mentioned connections for 1 Bit Width. In addition below connections:
			P410(DAT1)(J4 Pin14)   --->   PMOD SD Pin7(DAT1)
			P206(DAT2)(J4 Pin12)   --->   PMOD SD Pin8(DAT2)
			P205(DAT3)(J4 Pin10)   --->   PMOD SD Pin1(CS)
		
	For EK-RA6M2: (SDHI 4 bits width by default)
		1 Bit Width:
			P412(CMD) (J2 Pin1)    --->   PMOD SD Pin2(MOSI)
			P411(DAT0)(J2 Pin20)   --->   PMOD SD Pin3(MISO)
			P413(CLK) (J2 Pin8)    --->   PMOD SD Pin4(SCLK)
			GND                    --->   PMOD SD Pin5(GND)
			+3V3(VCC)              --->   PMOD SD Pin6(VCC)
			P210(CD)  (J1 Pin28)   --->   PMOD SD Pin9(CD)
			P209(WP)  (J1 Pin30)   --->   PMOD SD Pin10(WP)

		4 Bit Width: Tracecut Jumper E15 and solder to E12. Above mentioned connections for 1 Bit Width. In addition below connections:
			P410(DAT1)(J2 Pin2)    --->   PMOD SD Pin7(DAT1)
			P206(DAT2)(J1 Pin3)    --->   PMOD SD Pin8(DAT2)
			P205(DAT3)(J2 Pin14)   --->   PMOD SD Pin1(CS)
		
	For EK-RA6M3 & EK-RA6M3G: (SDHI 4 bits width by default)
		1 Bit Width:
			P412(CMD) (J3 Pin33)   --->   PMOD SD Pin2(MOSI)
			P411(DAT0)(J3 Pin36)   --->   PMOD SD Pin3(MISO)
			P413(CLK) (J3 Pin34)   --->   PMOD SD Pin4(SCLK)
			GND                    --->   PMOD SD Pin5(GND)
			+3V3(VCC)              --->   PMOD SD Pin6(VCC)
			P415(CD)  (J3 Pin32)   --->   PMOD SD Pin9(CD)
			P414(WP)  (J3 Pin31)   --->   PMOD SD Pin10(WP)

		4 Bit Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			P410(DAT1)(J3 Pin35)   --->   PMOD SD Pin7(DAT1)
			P206(DAT2)(J1 Pin38)   --->   PMOD SD Pin8(DAT2)
			P205(DAT3)(J2 Pin4)    --->   PMOD SD Pin1(CS)
	
	For EK-RA6M4: (SDHI 4 bits width by default)
		1 Bit Width:
			P412(CMD) (J1 Pin34)    --->    PMOD SD Pin2(MOSI)
			P411(DAT0) (J1 Pin35)  --->    PMOD SD Pin3(MISO)
			P413(CLK)(J1 Pin33)    --->    PMOD SD Pin4(SCLK)
			GND          --->    PMOD SD Pin5(GND)
			+3V3(VCC)    --->    PMOD SD Pin6(VCC)
			P210(CD) (J4 Pin19)    --->    PMOD SD Pin9(CD)
			P209(WP) (J4 Pin20)    --->    PMOD SD Pin10(WP)

		4 Bit Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			P410(DAT1)(J1 Pin36)   --->    PMOD SD Pin7(DAT1)
			P206(DAT2)(J4 Pin10)   --->    PMOD SD Pin8(DAT2)
			P205(DAT3)(J4 Pin11)   --->    PMOD SD Pin1(CS)
			
	For EK-RA6M5: (SDHI 4 bits width by default)
		1 Bit Width:
			P412(CMD) (J1 Pin34)   ---> PMOD SD Pin2(MOSI)
			P411(DAT0)(J1 Pin35)   ---> PMOD SD Pin3(MISO)
			P413(CLK) (J1 Pin33)   ---> PMOD SD Pin4(SCLK)
			GND                    ---> PMOD SD Pin5(GND)
			+3V3(VCC)              ---> PMOD SD Pin6(VCC)
			P210(CD)  (J2 Pin17)   ---> PMOD SD Pin9(CD)
			P209(WP)  (J2 Pin18)   ---> PMOD SD Pin10(WP)

		4 Bit Width: Above mentioned connections for 1 Bit Width. In addition below connections
			P410(DAT1)(J1 Pin36)   ---> PMOD SD Pin7(DAT1)
			P206(DAT2)(J2 Pin4)    ---> PMOD SD Pin8(DAT2)
			P205(DAT3)(J2 Pin5)    ---> PMOD SD Pin1(CS)
		
	For EK_RA8D1: User must turn-off SW1-3, SW1-5, SW1-8 on board to use SDHI channel 1.(SDHI 4 bits width by default)
		1 Bit Width: 
			P401(CMD) (J51 Pin7)	--->	PMOD SD Pin2(MOSI)
			P402(DAT0)(J52 Pin15)	--->	PMOD SD Pin3(MISO)
			P400(CLK) (J51 Pin10)	--->	PMOD SD Pin4(SCLK)
			GND 					--->	PMOD SD Pin5(GND)
			+3V3(VCC) 				--->	PMOD SD Pin6(VCC)
			P406(CD)  (J51 Pin5)	--->	PMOD SD Pin9(CD)
			P700(WP)  (J51 Pin6)	--->	PMOD SD Pin10(WP)

		4 Bits Width: Above mentioned connections for 1 bit width. In addition below connections:
			P403(DAT1)(J51 Pin9)	--->	PMOD SD Pin7(DAT1)
			P404(DAT2)(J57 Pin1)	--->	PMOD SD Pin8(DAT2)
			P405(DAT3)(J51 Pin8)	--->	PMOD SD Pin1(CS)
			
	For EK_RA8M1: J61 on board must be removed.(SDHI 4 bits width by default)
		1 Bit Width: 
			P307(CMD) (J54 Pin5)   --->   PMOD SD Pin2(MOSI)
			P304(DAT0)(J54 Pin9)   --->   PMOD SD Pin3(MISO)
			P308(CLK) (J54 Pin4)   --->   PMOD SD Pin4(SCLK)
			GND                    --->   PMOD SD Pin5(GND)
			+3V3(VCC)              --->   PMOD SD Pin6(VCC)
			P306(CD)  (J54 Pin7)   --->   PMOD SD Pin9(CD)
			P305(WP)  (J54 Pin8)   --->   PMOD SD Pin10(WP)

		4 Bits Width: Above mentioned connections for 1 bit width. In addition below connections:
			P303(DAT1)(J54 Pin6)   --->   PMOD SD Pin7(DAT1)
			P302(DAT2)(J54 Pin14)  --->   PMOD SD Pin8(DAT2)
			P301(DAT3)(J54 Pin12)  --->   PMOD SD Pin1(CS)
	
	For FPB_RA6E1: (SDHI 4 bits width by default)
		1 Bit Width: 
			P412(CMD) (J3 Pin20)   --->   PMOD SD Pin2(MOSI)
			P411(DAT0)(J3 Pin21)   --->   PMOD SD Pin3(MISO)
			P413(CLK) (J3 Pin19)   --->   PMOD SD Pin4(SCLK)
			GND                    --->   PMOD SD Pin5(GND)
			+3V3(VCC)              --->   PMOD SD Pin6(VCC)
			P210(CD)  (J3 Pin35)   --->   PMOD SD Pin9(CD)
			P209(WP)  (J3 Pin36)   --->   PMOD SD Pin10(WP)

		4 Bit Width: Above mentioned connections for 1 Bit Width. In addition below connections:
			P410(DAT1)(J3 Pin22)   --->   PMOD SD Pin7(DAT1)
			P206(DAT2)(J3 Pin31)   --->   PMOD SD Pin8(DAT2)
			P205(DAT3)(J3 Pin32)   --->   PMOD SD Pin1(CS)

4. Verifying Operation:
	1. Import, generate, build the EP project.
	2. Connect the RA MCU debug port to the host PC via a micro USB cable.
	3. Connect the PmodSD module to the RA board.
	4. Insert the SD card into the PmodSD module.
	5. Debug or flash the EP project to the RA board.
	6. Open the J-Link RTT Viewer.
	7. After the main menu is displayed on the RTT Viewer, the user selects options to perform file system management as desired.
	
	IMPORTANT:
	1. SD card must be in exFAT format before verify the EP.
	2. In case the SD is not in exFAT format, users can format the SD card in exFAT format using this EP using the following procedure:
		Open Media -> Format Media -> Close Media -> Open Media.
	3. The Media must be opened before performing other operations.
	
	a) In MAIN MENU: The user input option from the RTT Viewer will go to the next menu as follows:
		Type '1' and enter to go to MEDIA MENU.
		Type '2' and enter to go to DIRECTORY MENU.
		Type '3' and enter to go to FILE MENU.

	b) In MEDIA MENU: The user input options will perform media operations as follows:
		Type '1' and enter to open the media.
		Type '2' and enter to get media properties.
		Type '3' and enter to format the media in exFAT format.
		Type '4' and enter to close the media.
		Type '5' and enter to go back MAIN MENU.

	c) In DIRECTORY MENU: The user input options will perform directory operations as follows:
		Type '1' and enter to create a new directory.
		Type '2' and enter to get root directory properties.
		Type '3' and enter to delete a directory.
		Type '4' and enter to go back MAIN MENU.

	d) In FILE MENU: The user input options will perform file operations as follows:
		Type '1' and enter to create an empty file.
		Type '2' and enter to write more than 4 GB of fixed content into a file.
		Type '3' and enter to read the first 1 KB of content in a file.
		Type '4' and enter to delete a file.
		Type '5' and enter to go back MAIN MENU.


NOTE:
	1) The EP will write more than 4 GB of fixed data to the file. It takes few minutes to create a larger file. 
	Please validate the file size and contents on the PC.
	
	2) Segger RTT block address may need to be updated to observe the EP operation using a hex file with RTT-viewer.
	RTT Block address for hex file committed in repository are as follows:
		a. e2studio: 0x1ffe094c
		b. Keil: Not Available
		c. IAR: Not Available

	3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
	in .map file generated in the build configuration folder (Debug/Release).
