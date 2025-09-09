/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates some of the capabilities of the EK and FSP by using multiple peripherals.
        The project initializes the ADC and GPIO modules and uses SEGGER RTT interface to create RTT Termial.
        Once programmed the user can control the board by button presses on device and see regulaly updated board 
        configuration on the RTT Terminal.

2. Software Requirements:
        Renesas Flexible Software Package (FSP): Version 6.1.0
        e2 studio: Version 2025-07
        SEGGER J-Link RTT Viewer: Version 8.58
        GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware settings for the project:
        Please configure the Jumper settings as described in the Quick Start Guide.
        Link to access Quick Start Guide -  https://www.renesas.com/us/en/document/qsg/ek-ra2e2-quick-start-guide

4. Configuration Settings:
        Please explore the configuration settings of each module in the RA configurator.

Note: 
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200045a0
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
