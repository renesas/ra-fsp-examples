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
	
	Note: EK-RA4L1 board does not support using POEG trigger ACMPHS Mode.

2. Software Requirements:
     	Renesas Flexible Software Package (FSP): Version 6.3.0
     	e2 studio: Version 2025-12
     	SEGGER J-Link RTT Viewer: Version 8.92
     	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware Requirements:
     	Supported RA boards: EK-RA6M1, EK-RA6M2, EK-RA6M3, RSSK-RA6T1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, EK-RA8D1, EK-RA8M1,
			     MCK-RA8T1, FPB-RA8E1, EK-RA4L1
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Logic Analyzer (Optional).

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
	   GPT output pins on the input level. Then reset the respective POEG status by entering Menu option "4".
		
	4. For Selecting POEG Output disable using Software API, enter "2": The GPT output pins are disabled on R_POEG_OutputDisable()
	   software API. Then reset the respective POEG status by entering Menu option "4".

	5. For Selecting POEG Output disable on GPT output level, enter "3": The GPT output pins are disabled when the GTIOC pins are
	   on the same level. Then reset the respective POEG status by entering Menu option "4".
	
	6. For Selecting POEG Reset of channels, enter "4", and then Enter option "1" or "2" to reset POEG 0 or POEG 1 respectively.
	
	7. The EP performs the GPT output disable functioning using the above mentioned options.
	   Verify the GPT operation using either an Logic analyzer or an LED output.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200014b4
   b. Keil:     Not Available
   c. IAR:      Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).


