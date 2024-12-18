/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project the capability of the EK and FSP by usign multiple preipherals.
        The project initializes the ADC and GPIO modules and uses SEGGER RTT interface to create RTT Termial.
        Once programmed the user can control the board by button presses on device and see regulaly updated board 
        configuration on the RTT Terminal.

2. Hardware settings for the project:
        Please configure the Jumper settings as described in the Quick Start Guide.

3. Configuration Settings:
        Please explore the configuration settings of each module in the RA configurator.






 
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200004ac
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
