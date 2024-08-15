/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
	The example projects demonstrates Message Queue and Semaphore between tasks and interrupt. Message Queue is demonstrated 
	between Tasks and between Task and interrupt. GPT timer periodically generates interrupt at 1000msec. For first few seconds, 
	messages are shared between Sender and Receiver Tasks and GPT0 ISR. Receiver task pends on Message Queue, receives and 
	displays message received on RTT viewer periodically at 500msec. For next few seconds, Semaphore Task waits for sempahore 
	until it is released by GPT1 ISR. GPT1 ISR releases semaphore periodically at 1000msec. All the tasks run with equal 
	priority level.
   
