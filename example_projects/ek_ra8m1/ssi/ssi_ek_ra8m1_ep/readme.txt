/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

The example project illustrates a typical use of the SSI HAL module API. The project continuously transmits and receives
audio sample data from the source buffer to the destination buffer by looping back connecting the Tx and Rx pins and 
comparing the transmitted data of the two buffers at 500ms each time apart. The comparison results will be displayed
on the RTT viewer.

Hardware Connections:

EK-RA4M1, EK-RA4M3, EK-RA4M2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1:
Connect  P405 <--> P406
For FPB-RA6E1
Connect  P405 <--> P406
         P103(GPT2) <--> P402(AUDIO_CLK)

For EK-RA6E2
Connect  P301 <--> P302

NOTE:
In EK-RA6M3 and EK-RA6M3G boards, P403(which is also connected to SSISCK) is connected to LED1.So, it will glow while running the SSI application.﻿
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000498
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
