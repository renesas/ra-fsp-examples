/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of the Port Output Enable for GPT (POEG) driver on Renesas
	RA MCUs based on Renesas FSP. The user can disable the GPT output pins using one of the following options:
	1. Disable GPT output pins based on the GTETRG input pin level.
	2. Disable GPT output pins by software API.
	3. Disable GPT output pins when GTIOC pins are at the same logic level.
	4. Reset the POEG status.
	Information, guideline and error messages are displayed on the J-Link RTT Viewer. LED1 is turned ON when the GPT 
	output pin is disabled successfully. LED1 is turned OFF when POEG reset successfully. Additionally, the user can
	verify the EP operation using a logic analyzer.

	Note: EK-RA4L1, EK-RA2L2, EK-RA4C1 boards do not support disabling GPT output pins based on comparator (ACMPHS) 
	      crossing events.

	The user can refer the "Verifying Operation" section to run the example project properly. 

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware Requirements:
   	Supported RA boards: EK-RA6M1, EK-RA6M2, EK-RA6M3, RSSK-RA6T1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, EK-RA8D1,
			     EK-RA8M1, MCK-RA8T1, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA2L2, FPB-RA2T1, EK-RA8P1, EK-RA4C1.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x External Variable DC Power supply (For FPB-RA2T1).
	1 x Logic Analyzer (e.g., Saleae logic) (Optional).
	Some jumper wires.

4. Hardware Connections:		
	- Connect the USB debug port on the RA board to the host PC via a Type-C USB cable.
	
	- After selecting option 1, the user needs to give active LOW for GTETRG pin by:
		For EK-RA6M1, EK-RA6M2: 		
			Connect GTETRG P104 (J3:39) to GND.
		For EK-RA6M3: 		
			Connect GTETRG P104 (J1:3) to GND.
		For RSSK-RA6T1: 		
			Connect GTETRG P104 (CNC:16) to GND.
		For MCK-RA4T1: 		
			Connect GTETRG P104 (CN3:12) to GND.
		For MCK-RA6T2: 		
			Connect GTETRG PA12 (CN6:12) to GND.
		For MCK-RA6T3: 		
			Connect GTETRG P104 (J4:12) to GND.
		For EK-RA8D1: 		
			Connect GTETRG P401 (J51:7) to GND.
		For EK-RA8M1: 		
			Connect GTETRG P614 (J54:27) to GND.
		For MCK-RA8T1: 		
			Connect GTETRG P104 (CN4:8) to GND.
		For FPB-RA8E1: 		
			Connect GTETRG P104 (J1:45) to GND.
		For EK-RA4L1: 		
			Connect GTETRG P104 (J3:9) to GND.
		For EK-RA8E2: 		
			Connect GTETRG P614 (J37:9) to GND.
		For EK-RA2L2: 		
			Connect GTETRG P104 (J2:35) to GND.
		For FPB-RA2T1:
			Connect GTETRG P104 (J4:8) to GND.
		For EK-RA8P1: 		
			Connect GTETRG P614 (J14:20) to GND.
		For EK-RA4C1:
			The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
			the on-board debug functionality.
		 	The user must close E63 to use P104.	
			Connect GTETRG P104 (J2:20) to GND.

	- Before selecting option 4, the user needs to set up as shown below:
		For MCK-RA6T2:
			Connect PA07 (CN4:11) to PA00 (CN4:9).

		For FPB-RA8E1:
			Connect DA0 P014 (J1:34) to IVCMP0_2 (AN000) P004 (J1:20).
 
 		For EK-RA8E2:
			Connect DA0 P014 (J17:2) to IVCMP0_2 (AN000) P004 (J4:12).

		For FPB-RA2T1:
			- Connect the positive output pin (V+) of the External Variable DC Power supply to CMPIN01 P015 (J4:14)
		      of the RA board.
			- Connect the ground (GND) of the External Variable DC Power supply to the ground (GND) of the RA board.

	- Connect GPT output pins to Logic Analyzer to verify operations:
		For EK-RA8E2:
			Connect GTIOC0A P415 (J2:22) to Logic Analyzer Channel 0.
			Connect GTIOC0B P414 (J2:21) to Logic Analyzer Channel 1.
			Connect GTIOC5B P202 (J2:36) to Logic Analyzer Channel 2.
			Connect GND (J2:39) to Logic Analyzer GND.

		For EK-RA2L2:
			Connect GTIOC7A P302 (J2:23) to Logic Analyzer Channel 0.
			Connect GTIOC7B P301 (J2:24) to Logic Analyzer Channel 1.
			Connect GTIOC5B P501 (J3:4) to Logic Analyzer Channel 2.
			Connect GND (J2:40) to Logic Analyzer GND.

		For EK-RA8P1:
			Connect GTIOC0A P415 (J4:17) to Logic Analyzer Channel 0.
			Connect GTIOC0B P414 (J17:20) to Logic Analyzer Channel 1.
			Connect GTIOC5B P609 (J14:22) to Logic Analyzer Channel 2.
			Connect GND (J4:19) to Logic Analyzer GND.

		For FPB-RA2T1:
			Connect GPT output pins to Logic Analyzer to verify operations:
			Connect GTIOC0A P208 (J3:16) to Logic Analyzer Channel 0.
			Connect GTIOC0B P407 (J3:12) to Logic Analyzer Channel 1.
			Connect GTIOC1B P913 (J3:15) to Logic Analyzer Channel 2.
			Connect GND (J3:6) to Logic Analyzer GND.

		For EK-RA4C1:
			Connect GPT output pins to Logic Analyzer to verify operations:
			The user must close E58, E79 to use P415, P414.
			Connect GTIOC0A P415 (J2:33) to Logic Analyzer Channel 0.
			Connect GTIOC0B P414 (J2:40) to Logic Analyzer Channel 1.
			Connect GTIOC1B P110 (J4:20) to Logic Analyzer Channel 2.
			Connect GND (J4:5) to Logic Analyzer GND.

5. Verifying Operation:
	To run this example project, follow below instructions to observe the EP operation:
	
	1. Import, Generate and Build the example project.

	2. Connect the USB debug port on the RA board to the host PC via a Type-C USB cable.

	3. Download/Debug the example project to the RA board.

	4. Run the project and open J-Link RTT Viewer to see the output status.

	5. For FPB-RA2T1: When using option 1 (triggering POEG by GTETRG input pin level) or option 2 (triggering POEG
	   by software API), ensure that pin P015 (J4:14), which is configured as the analog input voltage source and
	   is in a high-impedance (Hi-Z) state, has a voltage lower than 1.44V. This helps prevent accidental
	   triggering by the comparator detection signals (option 4).
	   
	6. To disable GPT output pins based on the GTETRG input pin level, enter "1": The POEG1 channel number corresponds
	   to the GTETRG input pin which is used with this channel. GTETRG pin level (active LOW or active HIGH based on
	   configuration) disables GPT output pins on the input level.
	   - After selecting option 1, refer to the guidelines on RTT Viewer or "Hardware Connection" section to connect
	     hardware properly. Then reset the respective POEG status by entering Menu option "4".
	   - Remove connection of GTETRG pin and reset of application are needed prior to repeatedly selecting menu 
	     option "1", or selecting other options from the menu (except option "4")
	   
	7. To disable GPT output pins by software API, enter "2": The GPT output pins are disabled on 
	   R_POEG_OutputDisable() software API. Then reset the respective POEG status by entering Menu option "4".

	8. To disable GPT output pins when GTIOC pins are at the same logic level, enter "3": The GPT output pins are
	   disabled when the GTIOC pins are on the same level. Then reset the respective POEG status by entering Menu
	   option "4".
	  
	9. To reset POEG channel 0 (Required after running option 3): enter "4", then enter "1".
	   To reset POEG channel 1 (Required after running options 1 or 2): enter "4", then enter "2".
	
	10. Verify the POEG operation through output status on RTT Viewer, LED behavior or
	    waveform on a Logic Analyzer (optional).

Note:
1) For FPB-RA2T1, the user must Close E8, E15 and Cut E10 to use P213 for LED1.

2) To verify the EP operation waveform with a Logic Analyzer, see the 'Special Topics' section in the poeg_notes.md file.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200014b4
   b. Keil:     Not Available
   c. IAR:      Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
