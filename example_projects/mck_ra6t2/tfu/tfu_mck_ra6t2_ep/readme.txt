/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This example project demonstrates operation of the TFU to calculate the results of the sine, cosine, arctangent, 
	and hypotenuse functions at various points around the unit circle. The user will be prompted to set the stepping 
	resolution around the unit circle at runtime in JLink RTT Viewer. The program will halt after one full revolution 
	on the unit circle, and the results of each function will be printed to JLink RTT Viewer in CSV format. 
	
2. Hardware Requirements:
	
	  i. Renesas RA board (MCK-RA6T2)
	 ii. 1x micro-USB cable
	iii. PC with USB port and JLink RTT Viewer software
	

	
	

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000a0c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
