/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    This project demonstrates the direct access, configuration, & operation of DOC SFRs.
    The ISR for the DOC is configured through the Interrupts tab of the FSP Configurator.
    For more information on configuring interrupts, refer to:
    	https://renesas.github.io/fsp/ > RA FSP Documentation > Starting Development > e2 studio User Guide > Configuring a Project > Configuring Interrupts from the Stacks Tab 
    The SFRs & interrupts are configured & enabled, to perform a comparison match, using direct access and using BSP provided APIs.
    Red LED turns ON if there is an expected error encountered during operation.
	Blue LED is turned ON while program control waits for the User Defined ISR to execute.
	Green LED turns ON to indicate the successful conclusion of application execution.


1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000044c
   b. Keil: 	Not Available 
   c. IAR: 	Not Available