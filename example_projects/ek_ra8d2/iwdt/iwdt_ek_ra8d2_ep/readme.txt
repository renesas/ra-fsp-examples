/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the typical use of the IWDT HAL module. In RTT Viewer, the user can enter '1'
	to initialize the IWDT and start the GPT timer. The IWDT counter is refreshed periodically every 1 second when
	the GPT timer expires. Refresh status will be printed every 2 seconds. Once the user enters '2', the IWDT
	counter stops refreshing and resets the MCU.

	For FPB-RA0E1, FPB-RA0E2, FPB-RA0L1: In RTT Viewer, the user can enter '1' to initialize the IWDT and start the
	TAU timer. The IWDT counter is refreshed periodically every 1 second when the TAU timer expires.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	For FPB-RA2T1:
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

Note:
1) To calculate IWDT underflow time, please refer to the formula in the link:
   https://renesas.github.io/fsp/group___i_w_d_t.html

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil:     Not Available
   c. IAR:      Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
