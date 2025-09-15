/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the use of Message Queue and Semaphore between tasks and interrupts. Message
	Queue is demonstrated both between tasks and between a task and an interrupt. A GPT timer periodically generates
	an interrupt every 1000 milliseconds. For the first few seconds, messages are shared between Sender Task, Receiver
	Task, and the GPT msgq (Message Queue) ISR. The Receiver Task pends on Message Queue, receives meassages, and
	displays them on RTT Viewer very 500 milliseconds. For the next few seconds, the Semaphore Task waits for a
	semaphore to be released by the GPT sem (Semaphore) ISR. GPT sem ISR releases semaphore periodically every 1000
	milliseconds. All tasks run at the same priority level.

	For FPB-RA0E1, FPB-RA0E2, FPB-RA0L1: A TAU timer (with TAU msgq and sem ISRs) is used instead of a GPT timer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x20004850
   b. Keil:	Not Available
   c. IAR:	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
