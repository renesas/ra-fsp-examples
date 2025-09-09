/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of POEG driver on Renesas RA MCUs based on Renesas FSP.
	RA MCU is used to demonstrate the various operating modes of a POEG driver.
	GPT channels are configured to generate output via the GTIOCxA and GTIOCxB pins.
	The Port Output can be disabled using the below four options:
	1. Using POEG trigger pin - This mode supports disabling GPT output pins based on GTETRG input pin level.
	2. Using POEG software API - This mode supports disabling GPT output pins by R_POEG_OutputDisable() software API request.
	3. Using POEG trigger GPT output level - This mode supports disabling GPT output pins when GTIOC pins are the same level.
	4. Using POEG trigger ACMPHS Mode - This mode disabling GPT output pins based on comparator crossing event.
	5. Reset the POEG status - This option resets the POEG0 and POEG1 status.
	The Port Output can be enabled using the POEG Reset API.
	The status of each of the mode is displayed on J-Link RTT Viewer. The POEG disabled operation is displayed with LED indications.

2. Software Requirements:
     	Renesas Flexible Software Package (FSP): Version 6.1.0
     	e2 studio: Version 2025-07
     	SEGGER J-Link RTT Viewer: Version 8.58
     	LLVM Embedded Toolchain for ARM: Version 18.1.3

3. Hardware Requirements:
     	Supported RA boards: EK-RA6M1, EK-RA6M2, EK-RA6M3, RSSK-RA6T1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, EK-RA8D1, EK-RA8M1,
			     MCK-RA8T1, FPB-RA8E1
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Logic Analyzer (Optional).
	Some jumper wires.

4. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.
	For MCK-RA6T2:
		Connect PA07(CN4:11) --> PA00(CN4:9)

	For FPB-RA8E1:
		Connect DA0 - P014 (J1:P34) to IVCMP0_2 (AN000) - P004 (J1:P20)
 
5. Verifying Operation
	To run this example project, the user needs to compile and download the example project to the evaluation kit and then
	follow below instructions to observe the operation of the system.
	
	1. Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

	2. Open the J-Link RTT Viewer to view the application status/transfer messages.	

	3. For Selecting POEG Trigger on GTETRG Input pin level, enter "1": The POEG1 channel number corresponds to the GTETRG
	   input pin which is used with this channel. GTETRG pin level (active LOW or active HIGH based on configuration) disables
	   GPT output pins on the input level. Then reset the resepective POEG status by entering Menu option "5".
		
	4. For Selecting POEG Output disable using Software API, enter "2": The GPT output pins are disabled on R_POEG_OutputDisable()
	   software API. Then reset the resepective POEG status by entering Menu option "5".

	5. For Selecting POEG Output disable on GPT output level, enter "3": The GPT output pins are disabled when the GTIOC pins are
	   on the same level. Then reset the resepective POEG status by entering Menu option "5".
	
	6. For Selecting POEG Output disbale on ACMPHS events, enter "4": The GPT output pins are disabled on comparator crossing events.
	   Then reset the resepective POEG status by entering Menu option "5".
	   Reset of application is needed prior to continuous selection of the menu option "4" to restart the DAC0 and DAC1 and ACMPHS.
	   Note: Take out the Jumper Cable (use in option 1) if it is still connected while testing Menu option "4".

	   For FPB-RA8E1:
	   	a. Internal Reference Voltage (Vref) is used as reference voltage source and AN000 is used as input voltage source for ACMPHS
	   	   module, which is supplied by output of DAC0. The user can enter DAC0 value within permitted range to provide an analog 
	           signal input for AN000.
	   	   Reset of application is needed prior to continuous selection of the menu option "4" to restart the DAC0 and ACMPHS.
	   	b. Internal Reference Voltage (Vref) is used as reference voltage source, it is set to 1.18V (Equivalent to a digital value 1465).
   		c. The user has to input DAC0 value in between (0 - 1400) or (1600 - 4095) to prevent voltage fluctuations caused by electrical
		   noise, which may affect the comparator output.
		d. Connect DA0 - P014 (J1:P34) to IVCMP0_2 (AN000) - P004 (J1:P20) while testing Menu option "4".
	
	7. For Selecting POEG Reset of channels, enter "5", and then Enter option "1" or "2" to reset POEG 0 or POEG 1 respectively.
	
	8. The EP performs the GPT output disable functioning using the above mentioned options.
	   Verify the GPT operation using either an Logic analyzer or an LED output.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060450
   b. Keil:     Not Available
   c. IAR:      Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).


