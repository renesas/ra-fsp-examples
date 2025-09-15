/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionalities of ADC interval using TAU module on Renesas RA MCUs
	based on Renesas FSP. When ADC interval data sampling is performed TAU triggers an ADC group scan at a periodic
	interval. When group scan completes, the data transfer controller (DTC) triggers a data transfer, copying the
	scan result to a user buffer which is configured as ping-pong between the two buffers to allow for continuous
	processing of the acquired data. The sample data can be observed using waveform rendering in the memory viewer
	in e2studio.

2. Hardware Requirements:
	Supported RA boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Breadboard.
	1 x Signal generator (e.g., https://www.keysight.com/us/en/product/DSOX1202G/oscilloscope-70-100-200-mhz-2-analog-channels-waveform-generator.html).
	Some jumper wires.

3. Hardware Connections:
	The connection from the signal generator to the breadboard:
		- Connect an input signal (e.g., 800 Hz) to a horizontal rail on the breadboard.
		- Insert the pins listed below into the same horizontal row on the breadboard so that all pins are
		  shorted together and receive the input signal.
		- Connect the GND of the signal generator to the GND of the RA board.
	Note: All channels are connected to the same input signal for testing. The user can configure the channels
	      with different input frequencies if required.

	For FPB-RA0E1 (Supporting 16 bit TAU timer):
		To use P010 and P011 on J2: E26 and E25 must be closed.
		ADC Unit: AN000 P010 (J2:32), AN001 P011 (J2:31).

	For FPB-RA0E2 (Supporting 16 bit TAU timer):
		To use P010 and P011 on J2: E49 and E48 must be closed.
		ADC Unit: AN000 P010 (J2:27), AN001 P011 (J2:26).

	For FPB-RA0L1 (Supporting 16 bit TAU timer):
		To use the on-board debug functionality, the user must place jumper J9 on pins 1-2.
		To use P010 and P011 on J2: E54 and E53 must be closed.
		ADC Unit: AN000 P010 (J2:21), AN001 P011 (J2:20).

Note:
1) This example project is tested with 800Hz input sinewave signal frequency with amplitude between 0-3.3V.

2) When changing the sampling frequency, the user will also need to change:
   The time period in General -> Time period property tab under TAU module.
   Below is the calculation to calculate the above values.
   a. To calculate the Time period of TAU timer use:
      Time period = 1/input frequency (where input sampling frequency is 2 * signal generator input frequency).
      This calculated Time Period can be set as the TAU Time period in the properties tab.
   b. To calculate Raw period count for ADC trigger, use:
      Raw period count = (Time period in seconds * (ICLK / (2^ pre_scaler)))
      (where pre_scaler value is 0 by default, so 2^0 = 1)
      Note: ICLK frequency will vary depending on the MCU variant.

3) To add waveform rendering for each channel go to memory view add respective user buffer address for each channel
   under memory monitor, then click on New Rendering -> Waveform -> Waveform properties.
   The configuration parameters of the Waveform properties are as follows:
	Select Data Size:   16bit
	User-specified:     Tick on the box
	Minimum values:     0
	Maximum values:     4095
	Channel:            Mono
	Buffer size:        512
   As Ping-Pong buffer is used, the buffer size is 512 (i.e., 256*2) for each channel.
   Click Real-time Refresh on Memory monitor tab to check buffer in real-time.

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004cf0
   b. Keil:	Not Available
   c. IAR:	Not Available

5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the build configuration folder (Debug/Release).
