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
    Red LED is turned ON if there is an expected error encountered during operation.
    Blue LED is turned ON only while program control waits for the User Defined ISR to execute.
    Green LED is turned ON only to indicate the successful conclusion of application execution.

Note:
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000468
   b. Keil:	Not Available
   c. IAR: 	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).