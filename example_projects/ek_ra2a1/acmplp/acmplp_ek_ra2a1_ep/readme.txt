/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:	
	The example project demonstrates the typical use of the ACMPLP HAL module APIs.
	The project initializes ACMPLP in normal mode. Using Segger RTT, user can enter DAC value in the range of 0-4096 
	as one of the input to comparator. The user entered DAC value is compared with reference voltage.	
	Resulting comparator output can be viewed using on-board LED and displayed on the JLinkRTTViewer.	
        Any failure will also be displayed using JLinkRTTViewer.

2. Hardware and Software Requirements:

	External Hardware :
	Resistor 470 ohms (2 nos ) is used to create voltage divider circuit.

3. Hardware settings for the project:

	Hardware connections:	
	
	RA4M1-EK/RA2L1-EK:
	1) CMPIN0(P100)  ----> DA0(P014)
	2) CMPREF0(P101) ----> Output of voltage divider

	RA2A1-EK:
	1) CMPIN0(P400)  ----> DA0(P500)
	2) CMPREF0(P109) ----> Output of voltage divider

        RA4W1-EK: 
        1) CMPIN1(P501)  ----> DA0(P014)
	2) CMPREF1(P103) ----> Output of voltage divider

	Voltage divider circuit connection:
	Connect two 470 ohms resistors in series and provide VCC and GND accordingly to both ends of voltage divider circuit.
	The output voltage(1.65 V)is taken from center node of resistors to input of CMPREF0 pin.
	User can change reference volatge value by changing(inreasing/decreasing) resistor value.

NOTE  : User is expected to enter data of size not exceeding above 15 bytes.
	Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through RTT.

