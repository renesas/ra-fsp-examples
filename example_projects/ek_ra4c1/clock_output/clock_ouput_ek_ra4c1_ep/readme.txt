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
		   - To display information, the user can select between the SEGGER J-Link RTT Viewer and the Serial
		     Terminal (UART) with J-Link OB VCOM. It is important to note that the user should only operate a
		     single terminal application at a time to avoid conflicts or data inconsistencies. For instructions
		     on how to switch between these options, please refer the first step in the "Verifying Operation"
		     section.
		   - By default, EP information is printed to the host PC using the Serial Terminal for boards that
		     support J-Link OB VCOM. LED1 is turned ON to indicate the UART initialized unsuccessfully.
		   - For RA boards that do not support J-Link OB VCOM, EP information is instead printed via
		     the SEGGER J-Link RTT Viewer.
		   - RA boards supported for J-Link OB VCOM: EK-RA2A2, EK-RA4E2, EK-RA6E2, EK-RA8D1, EK-RA8M1,
		                                             EK-RA8P1, EK-RA4C1.

		For Verifying Operation:
		   - To verify EP, the user can select between the GPT or oscilloscope.
		   - By default, the EP uses GPT to capture clock output value. To use oscilloscope, please follow the
		     instructions in "Verifying Operation" section.
		
2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.0.0
	e2 studio: Version 2025-04.1
	SEGGER J-Link RTT Viewer: Version 8.44a
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application
	
3. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2,
			     EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA4C1.
	1 x Renesas RA board.
	1 x Type-C USB cable.
	1 x Signal analyzer to verify the CLKOUT signal (e.g., Saleae logic, Oscilloscope).
	Some jumper wires.
	
4. Hardware Connections:
	Connect the RA board USB debug port to the PC via Type-C USB cable for EP debugging.

	CLKOUT Pin Configuration for RA Development Kits
	====================================================
	| Development Kit | CLKOUT_PIN | Connector and Pin | 
	|-----------------|------------|-------------------|
	| EK-RA2A1        | P110       | J1:35             |
	| EK-RA2A2        | P210       | J2:8              |
	| EK-RA2E1        | P109       | J2:27             |
	| EK-RA2L1        | P109       | J1:33             |
	| EK-RA4E2        | P109       | J2:27             |
	| EK-RA4M1        | P205       | J2:14             |
	| EK-RA4M2        | P205       | J4:10             |
	| EK-RA4M3        | P205       | J4:11             |
	| EK-RA6E2        | P109       | J2:27             |
	| EK-RA6M1        | P205       | J4:10             |
	| EK-RA6M2        | P205       | J2:14             |
	| EK-RA6M3        | P205       | J2:4              |
	| EK-RA6M3G       | P205       | J2:4              |
	| EK-RA6M4        | P205       | J4:11             |
	| EK-RA6M5        | P205       | J2:5              |
	| EK-RA8M1        | P205       | J51:36            |
	| EK-RA8D1        | P913       | J57:19            |
	| EK-RA8P1        | P611       | J14:19            |
	| EK-RA4C1        | P600       | J24:2             |

	For Verifying Operation:
	* Using the GPT to capture clock output value (Default):
		For EK-RA2A1:
			Connect CLKOUT pin - P110 (J1:35) to GTIOCA pin - P408 (J1:27).

		For EK-RA2A2:
			The user must cut E12, E13 and then short E7, E11 to use main clock (MOSC).
			Connect CLKOUT pin - P210 (J2:8) to GTIOC4A pin - P011 (J1:9).

		For EK-RA2E1:
			The user must cut E12, E13 and then short E7, E11 to use main clock (MOSC).
			The user must cut E1, E8 and then short E9, E10 to use sub-clock (SOSC).
			Connect CLKOUT pin - P109 (J2:27) to GTIOC4A pin - P105 (J2:34).

		For EK-RA2L1:
			The user must cut E12, E13 and then short E7, E11 to use main clock (MOSC).
			The user must cut E1, E8 and then short E9, E10 to use sub-clock (SOSC).
			Connect CLKOUT pin - P109 (J1:33) to GTIOC0A pin - P415 (J1:3).

		For EK-RA4E2:
			The user must short E81 to use P410 for RXD0_PIN.
			Connect CLKOUT pin - P109 (J2:27) to GTIOC1A pin - P409 (J2:7).

		For EK-RA4M1:
			The user must cut E15 and then short E12 to use P205 for CLKOUT_PIN.
			Connect CLKOUT pin - P205 (J2:14) to GTIOC0A pin - P415 (J2:11).

		For EK-RA4M2:
			Connect CLKOUT pin - P205 (J4:10) to GTIOC1A pin - P105 (J3:21).

		For EK-RA4M3:
			Connect CLKOUT pin - P205 (J4:11) to GTIOC1A pin - P405 (J1:7).

		For EK-RA6M1:
			The user must cut E15 and then short E12 to use P205 for CLKOUT_PIN
			Connect CLKOUT pin - P205 (J4:10) to GTIOC0A pin - P415 (J2:5).

		For EK-RA6M2:
			The user must cut E15 and then short E12 to use P205 for CLKOUT_PIN
			Connect CLKOUT pin - P205 (J2:14) to GTIOC0A pin - P415 (J2:5).

		For EK-RA6M3, EK-RA6M3G:
			Connect CLKOUT pin - P205 (J2:4) to GTIOC0A pin - P415 (J3:32).

		For EK-RA6M4:
			Connect CLKOUT pin - P205 (J4:11) to GTIOC2A pin - P113 (J3:8).

		For EK-RA6M5:
			Connect CLKOUT pin - P205 (J2:5) to GTIOC0A pin - P415 (J1:31).

		For EK-RA6E2:
			Connect CLKOUT pin - P109 (J2:27) to GTIOC3A pin - P403 (J1:11).

		For EK-RA8D1:
			Turn OFF SW1-3 to use pin P415 for GTIOC0A.
			Connect CLKOUT pin - P913 (J57:19) to GTIOC0A pin - P415 (J51:12).

		For EK-RA8M1:
			Connect CLKOUT pin - P205 (J51:36) to GTIOC0A pin - P415 (J51:42).

		For EK-RA8P1:
			Connect CLKOUT pin - P611 (J14:19) to GTIOC0A pin - P302 (J37:25).

		For EK-RA4C1:
			The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
			the on-board debug functionality.
			The user must cut E12, E13 and then short E7, E11 to use main clock (MOSC).
			The user must cut E10, E14 and then short E15, E16 to use sub-clock (SOSC).
			Connect CLKOUT pin - P600 (J24:2) to GTIOC0A pin - P415 (J23:4).

	* Using the oscilloscope to measure clock output waveform:
		Connect the ground clip of the oscilloscope probe to a ground point on the RA board.
		Connect the probe tip to the CLKOUT pin (CLKOUT_PIN) on the RA board. 

5. Verifying Operation:
	1. Import the example project. Note that the EP supports the Serial terminal by default for RA boards that support
	   J-Link OB VCOM.
		* By default, the EP supports Serial terminal for RA boards that support J-Link OB VCOM
		Define USE_VIRTUAL_COM=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings ->
		-> GNU Arm Cross C Compiler -> Preprocessor -> Defined symbols(-D)
	
		* To use SEGGER J-Link RTT Viewer, please follow the instructions as below:
		Define USE_VIRTUAL_COM=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings ->
		GNU Arm Cross C Compiler -> Preprocessor -> Defined symbols(-D)
	2. Generate, build the example project.
	3. Connect the RA board debug port to the host PC via a Type-C USB cable.
	4. Open a Serial terminal application on the host PC and connect to the COM Port provided by the J-Link on-board or
	   Open J-Link RTT Viewer (In case user selected SEGGER J-Link RTT Viewer or RA boards do not support J-Link OB VCOM).
	5. Debug or flash the example project to the RA board.
	6. After the main menu is displayed on the terminal application, the user can select option to perform clock output
	   operation as desired.
		Type '1' to Start Main_Clock Output
		Type '2' to Start Sub_Clock Output
		Type '3' to Stop Clock Output
	7. Verifying Operation:
		* By default, the EP uses the GPT to capture clock output value
		Define GPT_MEASURE_CLKOUT=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings ->
		GNU Arm Cross C Compiler  -> Preprocessor -> Defined symbols(-D)
		- The user must connect CLKOUT pin to GTIOC0A pin as "Hardware Connections" section.
		- View clock output value on Terminal Application.

		* For using the oscilloscope to measure clock output waveform
		Define GPT_MEASURE_CLKOUT=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings ->
		GNU Arm Cross C Compiler -> Preprocessor -> Defined symbols(-D)
		- Connect the ground clip of the oscilloscope probe to a ground point on the RA board.
		- Connect the probe tip to the CLKOUT pin (CLKOUT_PIN) on the RA board. CLKOUT pin for each board refer
		  "Hardware Connections" section.

Note:
1) For Serial terminal application:
   a. To echo back characters typed in Tera Term, the user needs to enable it through
      [Setup] -> [Terminal...] -> Check [Local echo].
   b. The configuration parameters of the serial port on the terminal application are as follows:
	COM port is port provided by the J-Link on-board.
	Baud rate: 115200 bps
	Data length: 8-bits  
	Parity: none
	Stop bit: 1-bit
	Flow control: none
 
2) For using the J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
