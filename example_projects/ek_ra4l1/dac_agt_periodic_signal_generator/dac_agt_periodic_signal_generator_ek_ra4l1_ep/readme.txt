/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     	This example project demonstrates configuration of the DAC, AGT, and DTC to generate a continuous sinusoid signal.
     	The user will define the samples per period and frequency in Hz of the desired output wave in J-Link RTT Viewer.
     	The samples per period is used to compute and store 1 period of the sinusoid in a look up table (LUT), and the
     	frequency is used to set the AGT period to the appropriate sampling rate. The AGT generates an interrupt which
     	triggers the DTC to transfer the next value of the LUT to the DADR0 register.  
     	The output sinusoid wave will oscillate between the minimum and maximum DAC values at the user-specified samples
     	per period and frequency.

	For MCK-RA8T1: The AGT generates an interrupt which triggers the DTC to transfer the next value of the LUT to
	the DADR1 register.
	
2. Hardware Requirements:
     	Supported RA boards: EK-RA2A1, EK-RA2L1, EK-RA4M2, EK-RA6M4, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1,
                             EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA4L1
     	1 x Renesas RA board.
     	1 x Type-C USB cable for programming and debugging.
	
3. Hardware Connections:
     	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

     	The following pin connections are required to capture the output signal from the DAC:
     	For EK-RA2L1, EK-RA4M2, EK-RA6M4, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1:
		P014 is DA0 output.

       	For EK-RA2A1:
		P500 is DA12_0 output.

     	For MCK-RA8T1:
		P015 (CN1 Pin 13) is DA12_1 output.

	For FPB-RA8E1:
		P014 (J1 Pin 34) is DA0 output.

	For EK-RA4L1:
		P004 (J1 Pin 7) is DA0 output
	
Note:
1) J-Link RTT Viewer only supports integer values for user input.
	
2) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200009ac
   b. Keil:     Not Available 
   c. IAR:      Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
