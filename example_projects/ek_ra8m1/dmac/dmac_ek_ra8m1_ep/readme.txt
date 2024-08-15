/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:

This example project demonstrates basic functionalities of DMAC driver on Renesas RA MCUs based on Renesas FSP.
Project initializes 3 DMAC transfer instances. Upon successful initialization, a menu option is displayed.
On user input "1", AGT generates 100msec interrupt to trigger a transfer from the source buffer to port control
register for 60 times indicated by LED blinking for 60 times. On user input "2", 32 bit data gets transferred 
from GPT counter register to destination array and display destination array data on RTT viewer.On user input "3",
32 bit data gets transferred from predefined source array to port register(LED) and state of LED changes accordingly. 

Note:
1. On starting or reseting the board, at times the dmac transfer operation takes more time. Due to this the led blinking 
frequency decreases/pauses. Please reset the board in that case, and it will run as expected. 
2. LED blinking pauses in-between and then continues on RA6M3 and RA6M3G.
3. For input on RTT Viewer, please set the end-of-line character as a newline character(\n) to avoid invalid input: At menu elements: Input -> End of Line -> Unix format (LF)
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220009d8
   b. Keil:	0x22000128
   c. IAR: 0x22000384
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
