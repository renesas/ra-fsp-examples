/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionalities of SDADC running on Renesas RA MCUs using an RA board and an external variable power supply. 
	SDADC channel 0 is connected as a single-ended input mode, and channel 2 is connected as a differential-ended input mode to the power supply.
	For the EK-RA2A2 board, SDADC channel 0 and channel 2 are connected as differential-ended input modes to the power supply.
	Data read from both channels is displayed on the JLink RTT Viewer.
       

2. Hardware settings for the project:
	
     For EK-RA2A1:

       	Hardware  : External power supply with more than one output source(variable voltages).

       	Hardware connections:
   
       	RA2A1-EK Board                            Power supply

       	Header J3 Pin 8(P100)  ----------------  +v supply(variable between 0.2 to 1.8V).

       	Differential Ended(+ve end voltage should be greater than -ve end.Differential input max difference is 800mv)
       	Header J1 Pin 9(P104)  ----------------  +v supply(variable between 0.2 to 1.8V)
       	Header J1 Pin 7(P105)  ----------------  +v supply(variable between 0.2 to 1.8V).
       	Header J1 Pin 17(VSS)  ----------------  Gnd
		
	Note: In absence of power supply with multiple output sources, the voltage can be supplied with single output and voltage divider circuit as explained below.
​
      	For external power supply with a single output, use resistor (R1 = 10 ohms) and (R2 = 100 ohms) in series (as a voltage divider) across one end of the output. This
      	setup provides different input voltages to differential ended pins of SDADC. 
​
	Example: Output from power supply = 1.6 V. Using resistor (R1 =10ohms) and (R2 =100 ohms) in series (as voltage divider) gives output of 1.45V (approx). Now connect
        1.6V to Pin 9 and 1.45V to Pin 7.
		
     For EK-RA2A2:
	  
	Hardware  : External power supply with more than one output source(variable voltages).
		
	Hardware connections:
		
	RA2A2-EK Board                            Power supply
	
	Header J1 Pin 23 (ANIP0)  --------------  +v supply(variable between 0 to 0.5V).
	Header J1 Pin 24 (ANIN0)  --------------  +v supply(variable between 0 to 0.5V).

	Header J1 Pin 19 (ANIP2)  --------------  +v supply(variable between 0 to 0.5V).
	Header J1 Pin 20 (ANIN2)  --------------  +v supply(variable between 0 to 0.5V).

	Header J1 Pin 28 (AVSS)   --------------  Gnd

	Note: In absence of power supply with multiple output sources, the voltage can be supplied with single output and voltage divider circuit as explained below.

	For external power supply with a single output, use resistor (R1 = 10 kohms),  (R2 = 330 ohms), and (R3 = 330 ohms) in series (as a voltage divider). 
	This setup provides two different input voltages to differential ended pins of SDADC.
​
	Example: 
		Output from power supply = 3.3 V. 
		Using resistors (R1 = 10 kohms), (R2 = 330 ohms) and (R3 = 330 ohms) in series (as a voltage divider) gives two outputs of 0.204V and 0.102V (approximately).
		Connect 0.102V to J1 Pin 23 (ANIP0) and 0.204V to J1 Pin 24 (ANIN0). SDADC channel 0 will have a negative value of: 0.102V - 0.204V = -0.102V (approximately).
		Connect 0.204V to J1 Pin 19 (ANIP2) and 0.102V to J1 Pin 20 (ANIN2). SDADC channel 2 will have a positive value of: 0.204V - 0.102V = 0.102V (approximately).

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000e04
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

3) To enable printing floats to RTT Viewer, edit the project settings and make sure use nano with printf is enabled. 
   The setting can be found by Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Misc > Use float with nano printf
