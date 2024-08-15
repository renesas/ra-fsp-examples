/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the DOC HAL module APIs. The project performs the different event 
        operations selected by the user in configurator properties. Events can be selected viz.,Compare Match, Compare Mismatch, 
        Addition Overflow, Subtraction Underflow and compares with the reference data. Upon Success/Failure of operation, the 
        result will be displayed on RTT viewer.LED will turn ON only if the selected event operation is success.﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20002454
   b. Keil:	0x20002004
   c. IAR: 0x2000205c
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
