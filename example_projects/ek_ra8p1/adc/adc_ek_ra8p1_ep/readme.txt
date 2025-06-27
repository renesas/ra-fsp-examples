/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the ADC HAL module APIs. The project initializes the ADC
	in Single Scan or Continuous Scan mode based on the user selection in RA Configuration. Once initialized, the
	user can initiate the ADC scan and also stop the scan (in the case of Continuous Scan mode) using J-Link RTT
	Viewer by sending commands. The user provides ADC channel input voltage from 0V to 3.3V with a voltage supply
	unit at the ADC channel voltage input pin. Once ADC scan is initialized, Window Compare Mode is enabled and
	compares the ADC channel input voltage with the upper and lower limits. The upper limit and lower limit are
	configured in RA Configurator. If the ADC channel input voltage is above the upper limit or below the lower
	limit, it triggers an event and notifies the user to act accordingly. Result and ADC status is displayed on
	the J-Link RTT Viewer.

	Note: 
	      FPB-RA0E1, FPB-RA0E2 do not support Window Compare Mode and operate with One-shot or Sequential mode.
	      MCK-RA6T2 does not support Window Compare Mode and operates with internal temperature sensor.
	      EK-RA8P1 does not support Window Compare Mode.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1,
			     MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G,
			     EK-RA6M4, EK-RA6M5, RSSK-RA6T1, FPB-RA4E1, FPB-RA6E1, EK-RA8M1, EK-RA8D1, FPB-RA2E3,
		             MCK-RA8T1, EK-RA2A2, FPB-RA0E1, FPB-RA8E1, EK-RA4L1, FPB-RA0E2, EK-RA8E2, EK-RA2L2,
			     EK-RA8P1

	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x External Variable DC Power supply.
	Some jumper wires to connect External Variable DC Power supply to the RA board.

3. Hardware Connections:
	Connect External Variable DC Power supply to the RA board as shown below:
	- Connect the positive output pin (V+) of the External Variable DC Power supply to ADC Channel Voltage
	  input pin of the RA board.
	- Connect the ground (GND) of the External Variable DC Power supply to the ground (GND) of the RA board.

	ADC Channel Voltage input pin of each the RA board:
	For EK-RA2A1:
		AN01 P501.
		Connect AVSS0 (J2:36) to VREFL0 (J2:34) using a jumper or wire.

	For EK-RA2E1, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, 
	    EK-RA6M3G, EK-RA6M4, EK-RA6M5, FPB-RA6E1, FPB-RA4E1, MCK-RA4T1, MCK-RA6T3, FPB-RA2E3:
		AN000 P000.

	For EK-RA2E2:
		AN009 P014.

	For EK-RA4W1:
		AN04 P004.

	For RSSK-RA6T1:
		AN002 P002.

	For EK-RA8M1, EK-RA8D1:
		AN00 P004.

	For MCK-RA8T1:
		AN002 P006 (CN1:9).

	For EK-RA2A2:
		AN000 P014.

	For FPB-RA0E1:
		AN000 P010.

	For FPB-RA8E1:
		AN00 P004 (J1:20).

	For EK-RA4L1:
		AN002 P004 (J1:7).

	For FPB-RA0E2:
		To use P011 on J2: The soldering bridge E48 must be closed.
		AN001 P011 (J2:26).

	For EK-RA8E2:
		AN000 P004 (J4:12).

	For EK-RA2L2:
		AN000 P000 (J1:7).

	For EK-RA8P1:
		AN001 P001 (J2:11).

4. Configuration Settings:
	By default, the example project runs in Continuous Scan mode on the boards.
	In order to switch the scan mode, change the 'Mode' field of ADC Driver in the RA Configurator to Single Scan.
	
	For FPB-RA0E1, FPB-RA0E2:
			By default the example project runs in Sequential mode on the board. 
			In order to switch the scan mode, change the 'Conversion operation' field of ADC_D Driver in the
			RA Configurator to One-shot.
		       
	For MCK-RA6T2, EK-RA8P1: 
			By default the example project runs in Continuous Scan mode on the board.
			In order to switch the scan mode, change the 'Scan mode' field of ADC_B Driver in the
			RA Configurator to Single Scan.

Note: 
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000430
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

3) For GCC ARM Embedded Toolchain: To enable printing floats to the RTT Viewer, edit the project settings and ensure that
   "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf
