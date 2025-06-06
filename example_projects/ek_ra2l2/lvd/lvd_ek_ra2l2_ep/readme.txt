/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	The example project demonstrates the typical use of the LVD HAL module APIs.
    	This project enables LVD0 circuit to reset MCU and LVD1 to generate maskable interrupt.
    	LVD0 resets MCU only when input voltage drops below LVD0 threshold value, causing the MCU to power off. 
	To detect this reset, the user has to increase the input voltage value back to above LVD0 threshold value. 
	By doing this, board gets powered up and LED will turn ON indicating LVD0 reset detection.
    	Ex: 2.8V is LVD0 threshold value. When input Voltage is dropped from 2.9V to 2.7V, LVD0 resets MCU.
    	The user has to raise input voltage above 2.8V again to detect LVD0 reset.

    	LVD1 generates a maskable interrupt only when input voltage is raised above LVD1 threshold value.
    	Ex: 2.9V is LVD1 threshold value. When input voltage is raised from 2.8V to 3.0V,
    	LVD1 generates the maskable interrupt.

   	Please note that no interrupt will be generated when Voltage is already above the threshold value and increased further.
    	Ex: When input voltage is raised from 3.0V to 3.1V with 2.9V as threshold voltage, no interrupt is generated.

    	LVD0 is configured in OFS1 register via the properties dialog of BSP tab of FSP configurator.

    	The example project status and any failures will be displayed on J-Link RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
        1 x Variable DC Power supply.
        1 x External JLink Segger with 9 pins (19 pins for MCK-RA8T1, FPB-RA0E1) connector and Type-B USB to connect JLink with PC.
       	External Capacitor if power supply generates spikes.
	Some jumper wires.

3. Hardware Connections:
	Connect External JLink with PC via a Type-B USB cable.
    	Connect DC Power supply +ve pin  ---> Any on board VCC pin.
    	Connect DC Power supply -ve pin  ---> Any on board VSS(GND) pin.
	
    	For EK-RA2A1, EK-RA4M1:
   		Connect External JLink connector ---> J10 JTAG point (Near Debug USB).
    		LVD1 generates an interrupt when input Voltage raises above 2.9V.
    		LVD0 resets the MCU when input Voltage drops below 2.51V.

    	For EK-RA6M1, EK-RA6M2:
    		Connect External JLink connector ---> J10 JTAG point (Near Debug USB).
    		LVD1 generates an interrupt when input Voltage raises above 2.99V.
    		LVD0 resets the MCU when input Voltage drops below 2.8V.

    	For EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA4M3, EK-RA6M5, EK-RA4E2, EK-RA6E2:
    		Connect External JLink connector ---> J13 JTAG point (Near Debug USB) 
    		LVD1 generates an interrupt when input Voltage raises above 2.99V.
    		LVD0 resets the MCU when input Voltage drops below 2.8V.

    	For EK-RA2E2, EK-RA2A2:
   		Connect External JLink connector ---> J13 JTAG point (Near Debug USB).
   		LVD1 generates an interrupt when input Voltage raises above 2.9V.
    		LVD0 resets the MCU when input Voltage drops below 2.51V.

    	For MCK-RA4T1, MCK-RA6T3:
		Connect External JLink connector ---> CN10 JTAG point (Near Debug USB).
		LVD1 generates an interrupt when input Voltage raises above 2.99V.
    		LVD0 resets the MCU when input Voltage drops below 2.8V.

    	For EK-RA8M1, EK-RA8D1, FPB-RA8E1:
    		Connect External JLink connector ---> J13 JTAG point (Near Debug USB).
		LVD1 generates an interrupt when input Voltage raises above 3.08V.
    		LVD0 resets the MCU when input Voltage drops below 2.85V.

    	For FPB-RA2E3:
    		Connect External JLink connector ---> CN4 JTAG point (Near Debug USB).
    		LVD1 generates an interrupt when input Voltage raises above 2.9V.
    		LVD0 resets the MCU when input Voltage drops below 2.51V.

    	For MCK-RA8T1:
    		Connect External JLink connector ---> CN9 JTAG point (Near Debug USB).
    		LVD1 generates an interrupt when input Voltage raises above 3.08V.
    		LVD0 resets the MCU when input Voltage drops below 2.85V.

    	For FPB-RA0E1:
    		Connect External JLink connector ---> CN4 JTAG point (Near Debug USB).
		LVD1 generates an interrupt when input Voltage raises above 2.91V.
    		LVD0 resets the MCU when input Voltage drops below 2.62V.

	For EK-RA4L1:
		Note: The user must jumper J29 on pins 1-2, 3-4, 5-6, 7-8, open jumper J6 and jumper J6A,
                      close jumper J9 and jumper J8 on pins 1-2 to use external debug.
    		Connect External JLink connector ---> J13 JTAG point (Near Debug USB).
		LVD1 generates an interrupt when input Voltage raises above 3.00V.
    		LVD0 resets the MCU when input Voltage drops below 2.85V.

	For EK-RA2L2:
		Note: The user must close jumper J9 and jumper J8 on pins 1-2 to use external debug.
    		Connect External JLink connector ---> J13 JTAG point (Near Debug USB).
		LVD1 generates an interrupt when input Voltage raises above 3.00V.
    		LVD0 resets the MCU when input Voltage drops below 2.82V.

Note:
1) To run LVD example project, use external debugger for flashing the code, disconnect debugging mode, push on board
   reset button and open J-Link RTT Viewer to see the output by changing input voltage values.

2) For EK-RA2E2, FPB-RA2E3, EK-RA2A2 boards, use external debugger for flashing the code, disconnect debugging mode,
   push the on board reset button. The user needs to decrease the input voltage below 1.7V (Power-on Reset Voltage - VPOR)
   then raise the input Voltage to 3.3V and open J-Link RTT Viewer to see the output by changing input voltage values.

3) For EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1 after changing input voltage values from below low threshold (ex: 2.8V) 
   to above high threshold (ex: 3.3V), re-connect J-Link RTT Viewer to show log.

4) Use external capacitors on PMODA or PMODB (pins 5, 6 or 11, 12) if input power supply is not accurate and 
   create unwanted spikes in voltage.

5) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000484c
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the build configuration folder (Debug/Release).