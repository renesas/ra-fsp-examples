/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        This Example Project demonstrates the functionality of QSPI module using On board QSPI chip macronix MX25L
        in QPI and Extended SPI modes. The SPI Protocol is selected from RA configurator.
        Fixed data is written to QSPI Flash device and read back. 
        Read and Write data is compared and the result is displayed on Jlink RTTViewer.
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000848
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
