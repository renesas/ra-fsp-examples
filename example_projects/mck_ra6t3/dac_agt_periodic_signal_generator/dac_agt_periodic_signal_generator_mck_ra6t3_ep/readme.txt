/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This example project demonstrates configuration of the DAC, AGT, and DTC to generate a continuous sinusoid signal. The
	user will define the samples per period and frequency in Hz of the desired output wave in JLink RTT Viewer. The 
	samples per period is used to compute and store 1 period of the sinusoid in a look up table (LUT), and the frequency is 
	used to set the	AGT period to the appropriate sampling rate. The AGT generates an interrupt which triggers the DTC to 
	transfer the next value of the LUT to the DADR0 register.  
	
	The output sinusoid wave will oscillate between the minimum and maximum DAC values at the user-specified samples per period
	and frequency.
	
2. Hardware Requirements:
	
	  i. Renesas RA board (EK-RA2A1, EK-RA2L1, EK-RA4M2, EK-RA6M4, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3)
	 ii. 1x micro-USB cable
	iii. PC with USB port and JLink RTT Viewer software
	
3. Hardware Connections:

	Connect micro-USB cable between J10 on RA board and USB port on PC. 

	The following pin connections are required to capture the output signal from the DAC:
	* RA2L1, RA4M2, RA6M4, RA4E2, RA6E2, RA4T1, RA6T3:
		P014 is DA0 ouput
	* RA2A1:
		P500 is DA12_0 ouptut
	
	
NOTE: 
	JLink RTT Viewer only supports integer values for user input.

	
	

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000cc4
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
