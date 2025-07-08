/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
      	The example project demonstrates the basic functionalities of ADC interval using TAU module on Renesas RA MCUs  
      	based on Renesas FSP. When ADC interval data Sampling is performed TAU triggers an ADC group scan at a periodic interval. 
      	When group scan completes, the data transfer controller (DTC) triggers a data transfer, copying the scan result
      	to a user buffer which is configured as ping-pong between the two buffers to allow for continuous processing of the acquired data.
      	The sample data can be observed using waveform rendering in the memory viewer in e2studio.

2. Hardware Requirements:
	Supported RA boards: FPB-RA0E1, FPB-RA0E2
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Breadboard.
	1 x Signal generator.
	Some jumper wires.
      
3. Hardware Connections:
      	The connection from the signal generator to breadboard:
      	Connect input signal (i.e 800Hz) to the horizontal line of the breadboard.

	Connect pins mentioned bellow to horizontal holes of the breadboard so that all pins are shorted to receive a input signal. 
      	Connect GND of the signal generator to GND of the RA board.
      	
	Note: All the channels are connected to the same input signal for testing, the user can configure channels to other input
              frequencies as per requirement.

      	For FPB-RA0E1 (Supporting 16 bit TAU timer):
		To use P010 and P011 on J2: E26 and E25 must be closed.
      		ADC Unit: AN000 P010 (J2:32), AN001 P011 (J2:31).

	For FPB-RA0E2 (Supporting 16 bit TAU timer):
		To use P010 and P011 on J2: E49 and E48 must be closed.
		ADC Unit: AN000 P010 (J2:27), AN001 P011 (J2:26).

Note: 
1) This example project is tested with 800Hz input sinewave signal frequency with amplitude between 0-3.3V.

2) When changing the sampling frequency, the user will also need to change:
   The time period in General -> Time period property tab under TAU module.
   Below is the calculation to calculate the above values.
     a. To calculate the Time period of tau timer use
	Time period = 1/input frequency (where input sampling frequency is 2 * signal generator input frequency).
        This calculated Time Period can be set as the TAU Time period in the properties tab.
     b. To calculate Raw period count for ADC trigger use 
        Raw period count =  (Time period in seconds * (ICLK / (2^ pre-scaler)))
        (where Prescaler value is 0 by default so 2^0 = 1)
        Note: ICLK frequency will vary as per the MCU variant.

3) To add waveform rendering for each channel go to memory view add respective user buffer address for each channel under memory monitor,
   then click on New Rendering -> Waveform -> Waveform properties
   Select data size as 16 bit, user-specified minimum and maximum values to 0 and 4095 respectively, Channel as Mono, buffer size as 512 
   for each of the channels as Ping-Pong buffer is used of size 256 each.
   Click Real-time Refresh on Memory monitor tab to check buffer in real-time.
   
4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004cf0
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
