/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:     
     The project initializes and demonstrates DAC module on Renesas RA MCUs using ADC.
     User can enter DAC value within permitted range(0-4095) and the converted output 
     is given as input to ADC channel 0.ADC output value is printed on the RTT JlinkViewer.

2. Hardware settings for the project:

        Hardware connections:

        Pin Connection for EK-RA6M1, EK-RA2L1, EK-RA6M5, EK-RA6M4, EK-RA6M3, EK-RA4M1, EK-RA4M3, FPB-RA4E1, FPB-RA6E1, EK-RA4M2, EK-RA6M2:
          Connect P014 (DAC Output) to P000(ADC Channel 0). 

        Pin Connection for EK-RA2A1:
         Connect P500 (DAC Output) to P501(ADC Channel 1).
         J2 Connector Pin no:36(AVSS0)to Pin no: 34(VREFLO).

        Pin Connection for MCK-RA6T2:
          Connect PA06 (CN4 Pin 3) (DAC Ouput) to PA00 (CN4 Pin 9) (ADC Channel 0).

	Pin Connection for EK-RA4W1:
          Connect P014 (DAC Output) to P004(ADC Channel 4).
	
NOTE:
1. User is expected to enter data of size not exceeding above 15 bytes.
   Operation is not guaranteed for any user input value other than integer (i.e. char, float, special char) through RTT.
   Example: Integer part of float values are processed.
2. For DAC input as 0, ADC output will not be 0 because of offset correction value. Hence 0 is not considered in the valid range. 
3. ADC output is in 12-bit/16-bit range.
4. For RSSK_RA6T1 Board, the ADC value would be approx. half of DAC value because there is resistive ciruit connected between CNB->Pin 5 and P000 of MCU.

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200010a4
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
