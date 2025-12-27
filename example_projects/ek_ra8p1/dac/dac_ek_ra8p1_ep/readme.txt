/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:     
     	The project initializes and demonstrates DAC module on Renesas RA MCUs using ADC.
    	The user can enter DAC value within permitted range (1-4095) and the converted output 
     	is given as input to ADC channel 0. ADC output value is printed on the J-Link RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Jumper wire (For EK-RA2A1: 2 x Jumper wires).

3. Hardware Connections:
     	For EK-RA6M1, EK-RA2L1, EK-RA6M5, EK-RA6M4, EK-RA6M3, EK-RA4M1, EK-RA4M3, FPB-RA4E1, FPB-RA6E1,
            EK-RA4M2, EK-RA6M2, EK-RA4E2, EK-RA6E2:
       		Connect P014 (DAC Output) to P000 (ADC Channel 0). 

     	For EK-RA2A1:
       		Connect P500 (DAC Output) to P501 (ADC Channel 1).
       		Connect AVSS0 (J2:36) to VREFL0 (J2:34) using a jumper wire.

     	For MCK-RA6T2:
       		Connect PA06 (CN4:3) (DAC Output) to PA00 (CN4:9) (ADC Channel 0).

     	For EK-RA4W1:
       		Connect P014 (DAC Output) to P004 (ADC Channel 4).

     	For MCK-RA4T1, MCK-RA6T3:
       		Connect P014 (CN2:12) to P000 (CN2:5) (ADC Channel 0).

     	For EK-RA8M1, EK-RA8D1:
       		Connect P014 (DAC Output) to P004 (ADC Channel 0).

     	For MCK-RA8T1:
       		Connect P014 (CN1:17) (DAC Output) to P006 (CN1:9) (ADC Channel 2).

	For FPB-RA8E1:
		Connect P014 (J1:34) (DAC Output) to P004 (J1:20) (ADC Channel 0).
	
	For EK-RA4L1:
		Connect P004 (J1:7) (DAC Output) to P003 (J1:6) (ADC Channel 1).

	For EK-RA8E2:
		Connect P014 (J17:2) (DAC Output) to P004 (J4:12) (ADC Channel 0).

	For EK-RA8P1:
		Connect P014 (J2:4) (DAC Output) to P001 (J2:11) (ADC Channel 1).

Note:
1) The user is expected to enter data not exceeding 15 bytes in size.
   Operation is not guaranteed for any user input value other than integer (e.g., char, float, special char)
   through J-Link RTT Viewer.
   Example: Only the integer part of float values will be processed.

2) For DAC input as 0, ADC output will not be 0 because of offset correction value. Hence 0 is not considered in
   the valid range.

3) ADC output is in 12-bit/16-bit range.

4) For RSSK-RA6T1 Board, the ADC value would be approx. Half of DAC value because there is resistive circuit
   connected between CNB -> Pin 5 and P000 of MCU.

5) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200042c
   b. Keil:     Not Available
   c. IAR:      Not Available

6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).