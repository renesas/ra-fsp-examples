/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	This example project demonstrates basic functionalities of vee flash driver on Renesas RA MCUs based on Renesas FSP.
    	Based on User input, EP performs following operations:
    	1. Writing different data types to the different record IDs of vee flash.
    	2. Reading back the data from the different record IDs.
    	3. Format operation.
    	4. Get the status of Virtual EEPROM.
    	5. Refresh operation.
   	On successful completion of each operation, success message will be displayed on the J-Link RTT Viewer.
    	Error and info messages will be printed on J-Link RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.	
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

Note:
1) Initialize device back to factory default by using Renesas Device Partition Manager before run the example project.
   The user can refer Section 5.3 "Setting up Hardware" in the document via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection
   For Renesas Device Partition Manager tool on each IDE:
   - Start E2Studio -> Run -> Renesas Debug Tools -> Renesas Device Partition Manager (Available on e2studio)

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001e04
   b. Keil: 	Not Available
   c. IAR: 	Not Available

3) EK-RA4L1 board must have HOCO running and stable in order to program and erase flash.
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the build configuration folder (Debug/Release).

5) For GCC ARM Embedded Toolchain: To enable printing floats to RTT Viewer, edit the project settings and make sure use nano
   with printf is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Misc > Use float with nano printf