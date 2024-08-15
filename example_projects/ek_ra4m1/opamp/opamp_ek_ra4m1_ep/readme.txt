/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

        This Example Project demonstrates the functionality of OPAMP module.

        Analog voltage is provided through DAC module which is taken from user through JLINK RTT viewer.

        OPAMP processing is triggered through software once user input DAC value in the range of 0 to 4095.

        OPAMP output is fed to ADC in turn the result is displayed on RTTViewer.



2. Hardware settings for the project:

        Hardware connections:

        
RA2A1-EK:

        1) P502(J3)   --> P015(J2)

        2) VREFHO(J2) --> AVCCO(J2) and VREFLO(J2) --> AVSSO(J2)

        3) VREFL(J3)  --> GND


        
	RA4M1-EK:

        1) P014(DAC0) --> P000(both on J2)

        2) P002(J4)   --> Both P001(J2) and P003(J2)

	RA4W1-EK:
	1) P014(DAC0 (CN8-32)) --> P011(OPAMP+ (CN8-37))
	2) Measure output at P004(OPAMPO (CN8-40)) using multimeter.



NOTE:
 1. a) For RA4W1-EK, Remove R14 and R17 and install R20 and R21.
    b) Measure the output at OPAMP output pin(P004) using multimeter
.
 2. User is expected to enter data of size not exceeding above 15 bytes.

 3. Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through Jlik RTTviewer.

