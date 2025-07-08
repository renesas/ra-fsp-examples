/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the DOC HAL module APIs. The project performs the different 
        event operations selected by the user in configurator properties. Events can be selected viz., Compare Match, 
        Compare Mismatch, Addition Overflow, Subtraction Underflow. Upon Success/Failure of operation, the result will
        be displayed on RTT Viewer. LED will turn ON only if the selected event operation is success and stays OFF if
	the operation fails.

Note:
1) Refer to R30AN0384 for example of advanced usage of DOC with other modules.﻿﻿

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22040430
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
