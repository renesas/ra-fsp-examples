/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the ACMPLP HAL module APIs.
	The project initializes ACMPLP in normal mode. Using the J-Link RTT Viewer, the user can enter a DAC value in the
	range of 0-4096 as one of the input to the comparator. The user's entered DAC value is compared with reference voltage.
	Resulting comparator output can be viewed using on-board LED and displayed on the J-Link RTT Viewer.
        Any failure will also be displayed using J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2L1, EK-RA4M1, EK-RA4W1, EK-RA4L1
	1 x RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Breadboard.
	Some jumper wires.
	2 x Resistors with equal resistance (Used to create voltage divider circuit).

3. Hardware Connections:
	For EK-RA4M1, EK-RA2L1:
		1) CMPIN0(P100)  ----> DA0(P014)
		2) CMPREF0(P101) ----> Output of voltage divider

	For EK-RA2A1:
		1) CMPIN0(P400)  ----> DA0(P500)
		2) CMPREF0(P109) ----> Output of voltage divider

        For EK-RA4W1:
        	1) CMPIN1(P501)  ----> DA0(P014)
		2) CMPREF1(P103) ----> Output of voltage divider
	
	For EK-RA4L1:
        	1) Connect CMPIN0  - P100 (J3:P2) to DA0 - P004 (J1:P7)
		2) Connect CMPREF0 - P101 (J3:P4) to Output of voltage divider

	Voltage divider circuit connection:
	Connect two resistors in series and connect VCC and GND to the respective ends of voltage divider circuit.
	The output voltage (1.65V) is taken from midpoint of two resistors to input of CMPREF pin.
	The user can change reference voltage value by changing (increasing/decreasing) resistor value.

Note:
1) The user is expected to enter data of size not exceeding above 15 bytes.
   Operation is not guaranteed for any user input value other than integer (i.e. float, char, special char) 
   through J-Link RTT Viewer.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000049c
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
