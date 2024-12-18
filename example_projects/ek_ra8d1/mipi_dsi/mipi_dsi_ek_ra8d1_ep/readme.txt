/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	This Example Project demonstrates the basic functionalities of the MIPI DSI device driver on Renesas RA MCUs based on Renesas FSP. 
	On successful initialization of the MIPI DSI module, the EP will display 8-color bars on the MIPI LCD(external LCD connected to RA MCU) 
	using the graphics and MPI-DSI modules in FSP. A user menu will be provided over the RTT Viewer. User can choose the time to enter 
	Ultra-Low-Power State(ULPS) and touch the screen or wait 30s to exit this mode. Status information and error messages will be printed 
	on Jlink RTT Viewer during the execution of the project.

2. Hardware Requirements:
    	Renesas RA boards: EK-RA8D1.
	MIPI Graphics Expansion Board (included in the kit ) 
    	1 x Micro USB cable.
	

3. Hardware Connections:
    	Connect the USB Debug port on EK-RA8D1 to the PC using a micro USB cable.
	Connect the MIPI Graphics Expansion Board to J58 of EK-RA8D1
	Set the configuration switches (SW1) on the EK-RA8D1 board as below:
	
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |       OFF    |     OFF    |      OFF   |     OFF    |      ON     |    OFF    |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	
Note :	  
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220008b8
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).