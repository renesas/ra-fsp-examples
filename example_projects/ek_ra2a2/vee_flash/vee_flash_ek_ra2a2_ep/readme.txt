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
    	On successful completion of each operation, success message will be displayed on the Jlink RTTViewer.
    	Error and info messages will be printed on JlinkRTTViewer.

2. Hardware Requirements:
	- RA board	
	- Micro USB device cable - 1x

3. Hardware Connections:
	Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	port of the respective board. Connect the another end of this cable to USB port of the host PC.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001e04
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

3) To enable printing floats to RTT Viewer, edit the project settings and make sure use nano with printf is enabled. 
   The setting can be found by Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Misc > Use float with nano printf
