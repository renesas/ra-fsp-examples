/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	This example project demonstrates the typical use of the SLCDC HAL module APIs.
    	The project displays digits from 990 to 9999 on Segment LCD with increment of 21.
    	If an error occurs in the operation, the project displays "Err" on the LCD screen.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M1, EK-RA2A2, EK-RA4L1
    	1 x Renesas RA board.
	For EK-RA4M1, EK-RA2A2:
        	1 x The Segment LCD: Lumex LCD-S401M16KR (https://www.mouser.in/datasheet/2/244/lumex_lumx-s-a0001420318-1-1737691.pdf).
          	4 x 0.47uF Capacitors.
          	Some jumper wires.
	For EK-RA4L1:
    		1 x The Segment LCD Board orderable part number: RTKLCDSEG1S00001BE

3. Hardware Connections:
        For EK-RA4M1:
            	Wiring details:
                        LCD Pin 1  --> P104 (COM0)
                        LCD Pin 2  --> P105 (COM1)
                        LCD Pin 3  --> P106 (COM2)
                        LCD Pin 4  --> P107 (COM3)
                        LCD Pin 5  --> P301 (SEG1)
                        LCD Pin 6  --> P302 (SEG2)
                        LCD Pin 7  --> P303 (SEG3)
                        LCD Pin 8  --> P400 (SEG4)
                        LCD Pin 9  --> P401 (SEG5)
                        LCD Pin 10 --> P402 (SEG6)
                        LCD Pin 11 --> P411 (SEG7)
                        LCD Pin 12 --> P410 (SEG8)

                        Open Trace-cut E3 and Isolate P106 from User LED (COM2 Pin).
                        Additional wiring required as per RA4M1 HW manual (See RA4M1 User's Manual (R01UH0887EJ0100) 
			section 45.7.2 Notes)

                        Connect/Short P111 (CAPH) & P112 (CAPL) with a non polar capacitor of 0.47uF value.

                        Connect each of P100 (VL1), P101 (VL2) & P103 (VL4) pins with a 0.47uF capacitor as shown in
			User's Manual.

        For EK-RA2A2:
                Wiring details:
                        LCD Pin 1  --> P206 (COM0)
                        LCD Pin 2  --> P205 (COM1)
                        LCD Pin 3  --> P204 (COM2)
                        LCD Pin 4  --> P203 (COM3)
                        LCD Pin 5  --> P302 (SEG1)
                        LCD Pin 6  --> P303 (SEG2)
                        LCD Pin 7  --> P304 (SEG3)
                        LCD Pin 8  --> P305 (SEG4)
                        LCD Pin 9  --> P306 (SEG5)
                        LCD Pin 10 --> P307 (SEG6)
                        LCD Pin 11 --> P308 (SEG7)
                        LCD Pin 12 --> P309 (SEG8)

                        Additional wiring required as per RA2A2 HW manual (See RA2A2 User's Manual (r01uh1005ej0100)
			section 36.7.2 Notes)

                        Connect/Short P209 (CAPH) & P208 (CAPL) with a non polar capacitor of 0.47uF value.

                        Connect each of VL1, VL2 & VL4 pins with a 0.47uF capacitor as shown in User's Manual.

        For EK-RA4L1:
                Important: Do not set jumper J7 and connect jumper J17 to pins 1-2 at the same time. This may damage
			   the SLCD display.

                The user needs to remove jumper J7 and set jumper J17 to pins 1-2 to enable P407 for SLCD function.

                Connect the Segment LCD Board (J1:1) --> J2 on the RA board (J2:1)

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000084c
   b. Keil: 	Not Available 
   c. IAR:  	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).