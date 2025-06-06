/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionalities of ADC periodic using GPT module on Renesas RA MCUs  
	based on Renesas FSP. When ADC periodic data Sampling is performed GPT triggers an ADC group scan at a periodic interval. 
	When each group scan completes, the data transfer controller (DTC) triggers a data transfer, copying the scan result
	to a user buffer which is configured as ping-pong between the two buffers to allow for continuous processing of the acquired data.
	The sample data can be observed using waveform rendering in the memory viewer in e2studio. 

2. Hardware Requirements:
	Supported RA boards: EK-RA2E1, EK-RA2L1, EK-RA4M1, EK-RA4M2, FPB-RA6E1, EK-RA6M3, EK-RA6M3G, EK-RA6M1, EK-RA6M2, EK-RA2E2,
			     EK-RA2A1, EK-RA4M3, EK-RA6M4, EK-RA6M5, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1,
			     FPB-RA2E3, MCK-RA8T1, EK-RA2A2, FPB-RA8E1, EK-RA4L1, EK-RA2L2
	1 x Renesas RA board.
	1 x Micro USB cable for programming and debugging.
	1 x Breadboard.
	1 x Signal generator.
	Some jumper wires.
      
3. Hardware Connections:
	The connection from the signal generator to breadboard:
	Connect input signal of (i.e 800Hz) to the horizontal line of the breadboard.

	Connect pins mentioned bellow to horizontal holes of the breadboard so that all pins are shorted to receive a input signal. 
	Connect GND of the signal generator to GND of the RA board.
      	
	Note: All the channels are connected to the same input signal for testing, the user can configure channels to other input
              frequencies as per requirement.

	Board Supporting 32-bit GPT Timer with Enhanced features:
	For EK-RA6M3, EK-RA6M3G:
		ADC UNIT 0: AN000--P000, AN001--P001, AN002--P002, AN003--P008
		ADC UNIT 1: AN100--P004, AN101--P005, AN102--P006, AN103--P010
      
	For EK-RA6M1: 
		ADC UNIT 0: AN000--P000, AN001--P001, AN002--P002, AN003--P008 
		ADC UNIT 1: AN100--P004, AN101--P005, AN102--P006, AN105--P014

	For EK-RA6M2:
		ADC UNIT 0: AN000--P000, AN001--P001, AN002--P002, AN003--P008 
		ADC UNIT 1: AN100--P004, AN101--P005, AN102--P006, AN105--P014
      
	Board Supporting 32-bit GPT timer without Enhanced features:
	For EK-RA2E1, EK-RA2L1, EK-RA4M1, EK-RA4M2, FPB-RA6E1:
		ADC UNIT 0: AN000--P000, AN001--P001, AN002--P002, AN003--P003

	For EK-RA2E2:
		ADC UNIT 0: AN009--P014, AN010--P015, AN019--P103, AN020--P102

	For EK-RA2A1:
		ADC Unit 0: AN00--P500, AN01--P501, AN02--P502, AN03--P015 
		Connect AVSS0 (J2:36) to VREFLO (J2:34)
		Connect AVCC0 (J2:38) to VREFHO (J2:32)
      
	For EK-RA4M3: 
		ADC Unit 0: AN000--P000, AN001--P001, AN002--P002, AN003--P003
		ADC Unit 1: AN119--P503, AN120--P504, AN121--P505, AN122--P506
      
	For EK-RA6M4:
		ADC Unit 0: AN000--P000, AN001--P001, AN002--P002, AN003--P003
		ADC Unit 1: AN116--P500, AN117--P501, AN118--P502, AN119--P503

	For EK-RA6M5: 
		ADC Unit 0: AN004--P004, AN005--P005, AN006--P006, AN007--P007 
		ADC Unit 1: AN100--P000, AN101--P001, AN102--P002, AN116--P500

	For EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3: (Supporting 16-bit GPT timer) 
		ADC Unit 0: AN000--P000, AN001--P001, AN002--P002, AN004--P004
	  
	For EK-RA8M1:
		ADC Unit 0: AN002--P006, AN004--P007, AN005--P010, AN007--P014
		ADC Unit 1: AN101--P001, AN102--P002, AN116--P513, AN117--P805

	For FPB-RA2E3: (Supporting 16-bit GPT timer) 
		ADC Unit 0: AN000--P000, AN001--P001, AN008--P013, AN009--P014

	For EK-RA8D1:
		ADC Unit 0: AN002--P006, AN004--P007, AN005--P010, AN007--P014
		ADC Unit 1: AN100--P000, AN101--P001, AN102--P002, AN116--P513

	For MCK-RA8T1:
		ADC Unit 0: AN000--P004, AN001--P005, AN002--P006, AN004--P007
		ADC Unit 1: AN100--P000, AN101--P001, AN102--P002, AN105--P015

	For EK-RA2A2: (Supporting 16-bit GPT timer) 
		ADC Unit 0: AN000--P014, AN001--P001, AN002--P002, AN003--P015
	  
	For FPB-RA8E1:
		ADC Unit 0: AN000--P004 (J1:20), AN001--P005 (J1:21), AN002--P006 (J1:22), AN004--P007 (J1:23)
		ADC Unit 1: AN100--P000 (J1:15), AN101--P001 (J1:17), AN102--P002 (J1:18), AN104--P003 (J1:19)

	For EK-RA4L1: 
		ADC Unit 0: AN001--P003 (J1:06), AN002--P004 (J1:07), AN022--P513 (J1:13), AN023--P512 (J1:14)

	For EK-RA2L2:
		ADC Unit 0: AN000--P000 (J1:07), AN001--P001 (J1:06), AN002--P002 (J1:05), AN003--P003 (J1:04)

Note: 
1) This example project is tested with 800Hz input sinewave signal frequency with amplitude between 0-3.3V.

2) When changing the sampling frequency, the user will also need to change:
   The time period in General -> Time period property tab under GPT module.
   ADC Compare A and B Match raw count values, in General -> Extra features -> ADC trigger property tab under GPT module for MCUs
   supporting enhanced GPT timer. Below is the calculation to calculate the above values.
     a. To calculate the Time period of GPT timer use 
        Time period = 1/input frequency (where input frequency is 2 * Input frequency of signal generator).
        This calculated Time Period can be set as the GPT Time period in the properties tab.
     b. To calculate Raw period count for ADC trigger use 
        Raw period count = (Time period in seconds * (PCLKD / (2^ Pre-scaler))
        (where Prescaler value is 0 by default so 2^0 = 1)
        Note: PCLKD frequency will vary as per the MCU variant.
     c. ADC Trigger value for ADC Compare match value for A and B are set to 50% and 99% duty cycle (Raw period count).
        So ADC Compare match value for A = (0.5 * Raw period count) and ADC Compare match value for B = (0.99 * Raw period count).
        Note : ADC Compare A and B Match raw count values are supported for MCU's supporting enhanced GPT timer.
               For MCU's without enhanced GPT timer support, the user only needs to set GPT Time period as ADC group are set to trigger
	       ADC group A at 50% of the cycle and ADC group B at full cycle (GPT overflow interrupt).

3) To add waveform rendering for each channel go to memory view add respective user buffer address for each channel under memory monitor,
   then click on New Rendering -> Waveform -> Waveform properties
   Select Data Size as 16bit, user-specified minimum and maximum values to 0 and 4095 respectively, Channel as Mono, buffer size as 512 
   for each of the channels as Ping-Pong buffer is used of size 256 each.
   Click Real-time Refresh on Memory monitor tab to check buffer in real-time.
   
4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200048cc
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
