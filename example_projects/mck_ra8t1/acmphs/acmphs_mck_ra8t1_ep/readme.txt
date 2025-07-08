/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:	
	The example project demonstrates the typical use of the ACMPHS HAL module APIs.
	DAC0 is used as reference voltage source and DAC1 is used as input voltage source for ACMPHS module.
	DAC0 value is set to 2048(i.e. 1.6V). User can enter DAC1 value within permitted range. When DAC1 input value is greater
	than set DAC0 reference voltage, Comparator output status is HIGH and on-board LED is turned ON. When DAC1 input is less
	than reference voltage, Output status is LOW and the LED is turned OFF.

2. Hardware Connection:
	MCK-RA6T2 :
		PA07(CN4:11) --> PA00(CN4:9)

NOTE  :	User has to input DAC value in between (0 - 2000) or (2100 - 4095).
	User is expected to enter data of size not exceeding above 15 bytes.
 	Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through RTT.

﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220008c0
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
