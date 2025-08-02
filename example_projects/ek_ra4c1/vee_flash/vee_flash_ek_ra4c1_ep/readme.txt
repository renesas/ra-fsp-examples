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
	1 x Renesas RA board
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

Note:
1) It is important to note that a refresh occurs automatically when a segment lacks space. Applications may force 
   a refresh (option 5) if space is low and more data is expected. However, a refresh should not be forced when
   the flash is empty (e.g., after a format or erase), as there is no data to preserve. In such cases, performing
   a refresh is unnecessary and may lead to unintended behavior or errors during flash operations.

2) For EK-RA8D1, EK-RA8M1, MCK-RA8T1, FPB-RA8E1, EK-RA8E2 boards:
   Initialize device back to factory default by using Renesas Device Partition Manager before run the example project.
   The user can refer Section 5.3 "Setting up Hardware" in the document via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection
   For Renesas Device Partition Manager tool on each IDE:
   - Start E2Studio -> Run -> Renesas Debug Tools -> Renesas Device Partition Manager (Available on e2studio)

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x20001e04
   b. Keil:	Not Available
   c. IAR: 	Not Available

4) EK-RA4L1 board must have HOCO running and stable in order to program and erase flash.
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

6) For GCC ARM Embedded Toolchain: To enable printing floats to RTT Viewer, edit the project settings and make sure use nano
   with printf is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf
