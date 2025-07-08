/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the direct access, configuration, & operation of DOC SFRs.
	The ISR for the DOC is configured through the Interrupts tab of the FSP Configurator.
	For more information on configuring interrupts, refer to: 
	https://renesas.github.io/fsp/ > RA FSP Documentation > Starting Development > e2 studio User Guide 
	> Configuring a Project > Configuring Interrupts from the Stacks Tab 
	The SFRs & interrupts are configured & enabled, to perform a comparison match, using direct access and 
	using BSP provided APIs.

	LED Behavior:
	a. For Boards with 3 LEDs:
	   LED 1 is turned ON only while program control waits for the User Defined ISR to execute.
	   LED 2 is turned ON only to indicate the successful conclusion of application execution.
	   LED 3 is turned ON if there is an expected error encountered during operation.

	b. For Boards with 2 LEDs
	   LED 1 is turned ON only while program control waits for the User Defined ISR to execute.
	   LED 2 is turned ON only indicate the successful conclusion of application execution.
	   LED 1 and LED 2 are turned ON if there is an expected error encountered during operation.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	For FPB-RA2T1:
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.
		The user must Close E7 to use P914 for LED2.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004840
   b. Keil:	Not Available
   c. IAR: 	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
