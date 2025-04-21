/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example projects demonstrates Message Queue and Semaphore between tasks and interrupt.
	Message Queue is demonstrated between Tasks and between Task and interrupt. TAU timer periodically generates
	interrupt at 1000msec. For first few seconds, messages are shared between Sender and Receiver Tasks and
	TAU msgq (Message Queue) ISR. Receiver task pends on Message Queue, receives and displays message received on
	J-Link RTT Viewer periodically at 500msec. For next few seconds, Semaphore Task waits for semaphore until
	it is released by TAU sem (semaphore) ISR. TAU sem ISR releases semaphore periodically at 1000msec.
	All the tasks run with equal priority level.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

﻿Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004464
   b. Keil:	0x20004204
   c. IAR:      0x200057c4
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
