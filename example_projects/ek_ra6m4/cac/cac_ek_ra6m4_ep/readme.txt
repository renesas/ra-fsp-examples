/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the CAC HAL module APIs. 
	On any key press from RTT viewer, CAC module starts measuring the 
        accuracy of selected target clock against the selected reference clock.
        Result of the measurement is displayed on RTTViewer.

NOTE: 
       1. User may select any reference clock and clock divider.
       2. User may select any measurement clock and clock divider.
       3. User must set upper limit and lower limit in configurator based on the selected reference and measurement clocks.

Formula to calculate threshold(upper/lower) limits: 

1. Get Measurement clock frequency with clock divider i.e. say 'M'. 
   for ex. select HOCO(24MHZ on EK_RA4M1) and clock divider as '1', then M = 24MHZ.
2. Get Reference clock frequency with clock divider i.e. say 'R'.
   for ex. select Main OSC(12MHZ on EK_RA4M1) and clock divider as '32', then R = (12MHZ/32).
3. Calculate count value, say 'C'
   i.e. C = (M/R), so C = 64.
4. Take a tolerance of 10%, say 'T'.
   i.e. T = (C * (10/100)) = C/10 = 6.4.
5. Upper limit Threshold = (count value + tolerance) = (C + T) = 70.4
6. Lower limit Threshold = (count value - tolerance) = (C - T) = 57.6

Note :  
* For clock frequencies, refer "Clocks" tab in configurator or board specific user manual.


	
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000454
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address in .map file generated in the build configuration folder (Debug/Release)
