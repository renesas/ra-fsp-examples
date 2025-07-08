/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	The Example Project demonstrates the basic functionalities of DRW device driver on Renesas RA MCUs based on Renesas FSP.
	On successful initialization of DRW and GLCDC module, the EP will display the frame buffer set in the GLCDC module and
    	draw various shape and shape fills to cover the entire screen of external LCD connected to RA MCU. Additionally, API 
 	Failure messages will be displayed on RTT Viewer.

2. Hardware Requirements:
   	Renesas RA boards: EK-RA6M3G, EK-RA8D1, EK-RA8E2
    	1 x RA Board.
	1 x Graphics Expansion Board:
		For EK-RA6M3G: Graphics Expansion Board P/N RTK7EK6M3B00001BU.
		For EK-RA8D1: MIPI Graphics Expansion Board.
		For EK-RA8E2: Graphics Expansion Board P/N RTKAPPLCDPS02001BE.
	1 x Type-C USB cable.

3. Hardware Connections:
	For EK-RA6M3G:
		1. Connect the USB Debug port on EK-RA6M3G to the PC using a micro USB cable.
		2. Connect Graphics Expansion Board to the RA board.

	For EK-RA8D1:
	  	1. Connect the USB Debug port on EK-RA8D1 to the PC using a micro USB cable.
	  	2. Connect the MIPI Graphics Expansion Board to J58 of EK-RA8D1.
	  	3. Set the configuration switches (SW1) on the EK-RA8D1 board as below:
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |       OFF    |     OFF    |      OFF   |     OFF    |      ON     |    OFF    |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

	For EK-RA8E2:
		Connect J1 on Graphic Expandsion Board to J1 on EK-RA8E2.
		Please carefully align pin 2 on the Graphic Expandsion Board with pin 1 on the EK-RA8E2.
		Connect the USB Debug port on EK-RA8E2 to the PC using a Type-C USB cable.

Note: 
1) The user can also view the "Raw Image" in memory monitor of the e2studio IDE using the address of g_framebuffer 
   and specifying the resolution.
   For detailed information, please refer to the "Verifying Operation" section in drw_notes.md.
   
2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22042550
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).