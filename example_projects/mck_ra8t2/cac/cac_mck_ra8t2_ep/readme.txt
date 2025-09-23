/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the CAC HAL module APIs. 
	On any key press from J-Link RTT Viewer, CAC module starts measuring the 
        accuracy of selected target clock against the selected reference clock.
        Result of the measurement is displayed on J-Link RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

Note:
1) The user is expected to enter data not exceeding 15 bytes in size.

2) The user may select any reference clock and clock divider. However, ensure that the reference clock is appropriately
   chosen relative to the CPU clock. A high reference clock may cause infinite interrupts upon measurement completion.
   To avoid this, use a lower reference clock frequency or adjust the clock divider accordingly. 

3) The user may select any measurement clock and clock divider.

4) The user needs to ensure that the Main clock/Sub-clock oscillator is available on the board before selecting it.

5) The user must set upper limit and lower limit in configurator based on the selected reference and measurement clocks.

6) Formula to calculate threshold (upper/lower) limits: 
   a. Get Measurement clock frequency with clock divider i.e. say 'M'. 
      for ex. select HOCO(24MHZ on EK-RA4M1) and clock divider as '1', then M = 24MHZ.
   b. Get Reference clock frequency with clock divider i.e. say 'R'.
      for ex. select Main OSC(12MHZ on EK-RA4M1) and clock divider as '32', then R = (12MHZ/32).
   c. Calculate count value, say 'C'.
      i.e. C = (M/R), so C = 64.
   d. Take a tolerance of 10%, say 'T'.
      i.e. T = (C * (10/100)) = C/10 = 6.4.
   e. Upper limit Threshold = (count value + tolerance) = (C + T) = 70.4.
   f. Lower limit Threshold = (count value - tolerance) = (C - T) = 57.6.

7) For clock frequencies, refer to the "Clocks" tab in the configurator or the board-specific user manual.

8) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil: 	Not Available
   c. IAR: 	Not Available

9) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
