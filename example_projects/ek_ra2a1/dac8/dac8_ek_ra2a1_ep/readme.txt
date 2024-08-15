/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:     
     The project initializes DAC8 module in Normal mode.
     In Normal mode, user can enter DAC value within permitted range(1-255).
     When DAC output value is given as input to ADC channel 0, ADC output value is printed on the JlinkViewer.

2. Hardware settings for the project:

        Hardware connections:

        Pin Connection for EK-RA2A1:
          Connect P013 (DAC8 Output) to P500(ADC Channel 0).
          J2 Connector Pin no:36(AVSS0)to Pin no: 34(VREFLO).

NOTE:
1. User is expected to enter data of size not exceeding above 15 bytes.
   Operation is not guaranteed for any user input value other than integer (i.e. char, float, special char) through RTT.
   Example: Integer part of float values are processed.
2. For DAC input as 0, ADC output will not be 0 because of offset correction value. Hence 0 is not considered in the valid range. 
3. ADC output is in 16-bit range.