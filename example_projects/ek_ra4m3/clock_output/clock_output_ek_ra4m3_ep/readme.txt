/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the typical use of the Multiple Clocks Output.
	The user inputs key from Terminal Application to start Clocks Output Option to the clock output pin.
	Enter number 3 in the Terminal Application to stop the clocks output from the clock output pin of the MCU.
	The EP information or any error messages will be displayed on the Terminal Application.
	
	Note:
	      For Terminal Application:
	      - To display information, the user can select between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART) 
	        with J-Link OB VCOM. It is important to note that the user should only operate a single terminal application 
		at a time to avoid conflicts or data inconsistencies. For instructions on how to switch between these options, 
		please refer the first step in the "Verifying Operation" section.
	      - By default, EP information is printed to the host PC using the Serial Terminal for boards that support J-Link OB VCOM. 
		LED1 is turned ON to indicate the UART initialized unsuccessfully. 
	      - For RA boards that do not support J-Link OB VCOM, EP information is instead printed via the SEGGER J-Link RTT Viewer.
	      - RA boards supported for J-Link OB VCOM: EK-RA2A2, EK-RA4E2, EK-RA6E2, EK-RA8D1, EK-RA8M1.

	      For Verifying Operation:
 	      - To verify EP, the user can select between the GPT or oscilloscope.
		By default, the EP uses GPT to capture clock output value. To use oscilloscope, please follow the instructions 
	        in "Verifying Operation" section.
		
2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.7.0
	e2 studio: Version 2024-10
	SEGGER J-Link RTT Viewer: Version 8.10f
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application
	
3. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, 
			     EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1.
	1 x RA board.
	1 x Micro USB cable.
	1 x Oscilloscope to verify the CLKOUT signal.

4. Hardware Connections:
	Connect the RA board USB debug port to the PC via micro-USB cable for EP debugging.

	CLKOUT Pin Configuration for RA Development Kits
	====================================================
	| Development Kit | CLKOUT_PIN | Connector and Pin | 
	|-----------------|------------|-------------------|
	| EK-RA2A1        | P110       | J1-P35            |
	| EK-RA2A2	  | P210       | J2-P8             |
	| EK-RA2E1        | P109       | J2-P27            |
	| EK-RA2L1        | P109       | J1-P33            |
	| EK-RA4E2        | P109       | J2-P27            |
	| EK-RA4M1        | P205       | J2-P14            |
	| EK-RA4M2        | P205       | J4-P10            |
	| EK-RA4M3        | P205       | J4-P11            |
	| EK-RA6E2        | P109       | J2-P27            |
	| EK-RA6M1        | P205       | J4-P10            |
	| EK-RA6M2        | P205       | J2-P14            |
	| EK-RA6M3        | P205       | J2-P4             |
	| EK-RA6M3G       | P205       | J2-P4             |
	| EK-RA6M4        | P205       | J4-P11            |
	| EK-RA6M5        | P205       | J2-P5             |
	| EK-RA8M1        | P205       | J51-P36           |
	| EK-RA8D1        | P913       | J57-P19           |

	For Verifying Operation:
	* Using the GPT to capture clock output value (Default):
		For EK-RA2A1:
			Connect CLKOUT pin - P110 (J1-P35) to GTIOCA pin - P408 (J1-P27).

		For EK-RA2A2:
			The user must cut E12, E13 and then short E7, E11 to use main clock (MOSC).
			Connect CLKOUT pin - P210 (J2-P8) to GTIOC4A pin - P011 (J1-P9).

		For EK-RA2E1:
			The user must cut E12, E13 and then short E7, E11 to use main clock (MOSC).
			The user must cut E1, E8 and then short E9, E10 to use sub-clock (SOSC).
			Connect CLKOUT pin - P109 (J2-P27) to GTIOC4A pin - P105 (J2-P34).

		For EK-RA2L1:
			The user must cut E12, E13 and then short E7, E11 to use main clock (MOSC).
			The user must cut E1, E8 and then short E9, E10 to use sub-clock (SOSC).
			Connect CLKOUT pin - P109 (J1-P33) to GTIOC0A pin - P415 (J1-P3).

		For EK-RA4E2:
			The user must short E81 to use P410 for RXD0_PIN.
			Connect CLKOUT pin - P109 (J2-P27) to GTIOC1A pin - P409 (J2-P7).

		For EK-RA4M1:
			The user must cut E15 and then short E12 to use P205 for CLKOUT_PIN.
			Connect CLKOUT pin - P205 (J2-P14) to GTIOC0A pin - P415 (J2-P11).

		For EK-RA4M2:
			Connect CLKOUT pin - P205 (J4-P10) to GTIOC1A pin - P105 (J3-P21).

		For EK-RA4M3:
			Connect CLKOUT pin - P205 (J4-P11) to GTIOC1A pin - P405 (J1-P7).

		For EK-RA6M1:
			The user must cut E15 and then short E12 to use P205 for CLKOUT_PIN
			Connect CLKOUT pin - P205 (J4-P10) to GTIOC0A pin - P415 (J2-P5).

		For EK-RA6M2:
			The user must cut E15 and then short E12 to use P205 for CLKOUT_PIN
			Connect CLKOUT pin - P205 (J2-P14) to GTIOC0A pin - P415 (J2-P5).

		For EK-RA6M3, EK-RA6M3G:
			Connect CLKOUT pin - P205 (J2-P4) to GTIOC0A pin - P415 (J3-P32).

		For EK-RA6M4:
			Connect CLKOUT pin - P205 (J4-P11) to GTIOC2A pin - P113 (J3-P8).

		For EK-RA6M5:
			Connect CLKOUT pin - P205 (J2-P5) to GTIOC0A pin - P415 (J1-P31).

		For EK-RA6E2:
			Connect CLKOUT pin - P109 (J2-P27) to GTIOC3A pin - P403 (J1-P11).

		For EK-RA8D1:
			Turn OFF SW1-3 to use pin P415 for GTIOC0A.
			Connect CLKOUT pin - P913 (J57-P19) to GTIOC0A pin - P415 (J51-P12).

		For EK-RA8M1:
			Connect CLKOUT pin - P205 (J51-P36) to GTIOC0A pin - P415 (J51-P42).

	* Using the oscilloscope to measure clock output waveform:
		Connect the ground clip of the oscilloscope probe to a ground point on the RA board.
		Connect the probe tip to the CLKOUT pin (CLKOUT_PIN) on the RA board.

5. Verifying Operation:
	1. Import the example project. Note that the EP supports the Serial terminal by default for RA boards that support J-link OB VCOM.
		* By default, the EP supports Serial terminal for RA boards that support J-link OB VCOM
		Define USE_VIRTUAL_COM=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> GNU ARM Cross C Compiler -> Preprocessor
	
		* To use SEGGER J-Link RTT Viewer, please follow the instructions as below:
		Define USE_VIRTUAL_COM=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> GNU ARM Cross C Compiler -> Preprocessor
	2. Generate, build the Example project.
	3. Connect the RA board debug port to the host PC via a micro USB cable.
	4. Open a Serial terminal application on the host PC and connect to the COM Port provided by the J-Link on-board or 
	   Open J-Link RTT Viewer (In case user selected SEGGER J-Link RTT Viewer or RA boards do not support J-Link OB VCOM).
	5. Debug or flash the example project to the RA board.
	6. After the main menu is displayed on the terminal application, the user can select option to perform clock output operation as desired.
		Type '1' to Start Main_Clock Output
		Type '2' to Start Sub_Clock Output
		Type '3' to Stop Clock Output
	7. Verifying Operation:
		* By default, the EP uses the GPT to capture clock output value
		Define GPT_MEASURE_CLKOUT=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> GNU ARM Cross C Compiler -> Preprocessor
		- The user must connect CLKOUT pin to GTIOC0A pin as "Hardware Connections" section.
		- View clock output value on Terminal Application.

		* For using the oscilloscope to measure clock output waveform
		Define GPT_MEASURE_CLKOUT=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> GNU ARM Cross C Compiler -> Preprocessor
		- Connect the ground clip of the oscilloscope probe to a ground point on the RA board.
		- Connect the probe tip to the CLKOUT pin (CLKOUT_PIN) on the RA board. CLKOUT pin for each board refer "Hardware Connections" section.

	Note:
	For Serial terminal application:
	1) To echo back characters typed in Tera Term, the user needs to enable it through [Setup] -> [Terminal...] -> Check [Local echo].	
	2) The configuration parameters of the serial port on the terminal application are as follows:
		COM port is port provided by the J-Link on-board.
		Baud rate: 115200 bps
		Data length: 8-bits  
		Parity: none
		Stop bit: 1-bit
		Flow control: none

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008a4
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
