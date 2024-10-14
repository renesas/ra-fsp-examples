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
	      - RA boards supported for J-Link OB VCOM: EK-RA8D1.

	      For Verifying Operation:
 	      - To verify EP, the user can select between the GPT or oscilloscope.
		By default, the EP uses GPT to capture clock output value. To use oscilloscope, please follow the instructions 
	        in "Verifying Operation" section.
		
2. Software Requirements:
	Renesas Flexible Software Package (FSP) version 5.5.0
	e2 studio: Version 2024-07
	SEGGER J-Link RTT Viewer: Version 7.98b
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application
	
3. Hardware Requirements:
	Supported RA boards: EK-RA8D1
	1 x RA board
	1 x Micro USB cable
	1 x Oscilloscope to verify the CLKOUT signal

4. Hardware Connections:
	Connect the RA board USB debug port to the PC via micro-USB cable for EP debugging.

	CLKOUT Pin Configuration for RA Development Kits
	====================================================
	| Development Kit | CLKOUT_PIN | Connector and Pin | 
	|-----------------|------------|-------------------|
	| EK-RA8D1        | P913       | J57-P19           |

	For Verifying Operation:
	* Using the GPT to capture clock output value (Default):
		For EK-RA8D1:
			Turn OFF SW1-3 to use pin P415 for GTIOC0A
			Connect CLKOUT pin - P913 (J57-P19) to GTIOC0A pin - P415 (J51-P12)

	* Using the oscilloscope to measure clock output waveform:
		Connect the ground clip of the oscilloscope probe to a ground point on the RA board.
		Connect the probe tip to the CLKOUT pin(CLKOUT_PIN) on the RA board. 

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
		- Connect the probe tip to the CLKOUT pin(CLKOUT_PIN) on the RA board. CLKOUT pin for each board refer "Hardware Connections" section.

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
