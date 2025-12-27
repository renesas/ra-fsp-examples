/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of OSPI_B driver on Renesas RA MCUs based on Renesas FSP. 
    	In Main menu, based on RTT Viewer input, the user selects mode for opsi_b module.
        	1. SPI 1S-1S-1S protocol mode.
        	2. OPI 8D-8D-8D protocol mode.
    	In Sub Menu, based on RTT Viewer input, the user selects operations to perform.
        	1. Write data to OSPI flash device.
        	2. Read back the data from OSPI flash device.
        	3. Erase a sector of OSPI flash device.
        	4. Go back to main menu. 
    	On successful completion of each operation, a success message and execution time of operation will be displayed 
        on J-link RTT Viewer.
	Error messages and information will be printed on J-link RTT Viewer.

2. Hardware Requirements:
	RA supported boards: EK-RA8M1, EK-RA8D1, EK-RA8E2
	1 x RA board
	1 x Type-C USB cable for programming and debugging

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2204418c
   b. Keil: 	Not Available
   c. IAR: 	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the build configuration folder (Debug/Release).