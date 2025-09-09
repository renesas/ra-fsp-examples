/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example projects demonstrates Message Queue and Semaphore between tasks and interrupt. Message Queue is demonstrated 
	between Tasks and between Task and interrupt. GPT timer periodically generates interrupt at 1000msec. For first few seconds, 
	messages are shared between Sender and Receiver Tasks and GPT msgq (Message Queue) ISR. Receiver task pends on Message Queue, 
	receives and displays message received on RTT viewer periodically at 500msec. For next few seconds, Semaphore Task waits for 
	sempahore until it is released by GPT sem (sempahore) ISR. GPT sem ISR releases semaphore periodically at 1000msec. All the 
	tasks run with equal priority level.
   
﻿Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000854
   b. Keil:	0x20000a20
   c. IAR: 0x20001f2c
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
