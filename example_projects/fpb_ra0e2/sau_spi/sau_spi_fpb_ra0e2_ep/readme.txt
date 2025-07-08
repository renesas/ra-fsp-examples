/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the basic usage of SAU SPI driver. The project writes commands to the slave
	digital thermometer module, and reads the temperature values using SAU SPI bus configured as master.
	The temperature values are continuously printed on J-Link RTT Viewer. Status information and error messages
	will be printed on J-link RTT Viewer during the execution of the project.

2. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1, FPB-RA0E2
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Digital thermometer module (PMOD MAX31723PMB1).
		Link Product: https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31723PMB1?qs=UmMSjoC1xtH8e742i4OoUA%3D%3D
		Manufacturer Product Number: MAX31723PMB1#
	1 x Host PC.
	
3. Hardware Connections:
	PMOD MAX31723PMB1:
	---------
		Select SPI communication protocol on the PMOD by configuring the following jumper configurations on JP1:
		Connect JP1-3 to JP1-5
		Connect JP1-4 to JP1-6
		Connect JP1-9 to JP1-10

        For FPB-RA0E1, FPB-RA0E2:
    	---------
    		Connect the RA board to the host PC via a Type-C USB cable.
		Connect PMOD MAX31723PMB1 (J1) to the RA board via the Pmod1 port (PMOD1).
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.

Note:
1) The segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20005290
   b. Keil:	Not Available
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release). 

3) For GCC ARM Embedded Toolchain: To enable printing floats to RTT Viewer, edit the project settings and ensure that
   "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf
