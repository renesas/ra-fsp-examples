/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the typical use of the IWDT HAL module.
	The user input initializes the IWDT and start GPT timer. IWDT counter is refreshed
	periodically every 1 second when the GPT timer expires. On entering 2 from RTT Viewer,
	IWDT counter stops refreshing and resets the MCU.
	
	For FPB-RA0E1, FPB-RA0E2: The user input initializes the IWDT and start TAU timer. IWDT counter is refreshed
	periodically every 1 second when the TAU timer expires

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	For FPB-RA2T1:
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.

Note:
1) To calculate IWDT underflow time, please refer to the formula in the link:
   https://renesas.github.io/fsp/group___i_w_d_t.html
 
2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004898
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
