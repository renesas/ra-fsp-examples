/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionalities of POEG driver on Renesas RA MCUs based on Renesas FSP.
	RA MCU is used to demonstrate the various operating modes of a POEG driver.
	GPT channels are configured to generate output via the GTIOCxA and GTIOCxB pins.
	The Port Output can be disabled using the below four options
	1. Using POEG trigger pin - This mode supports disabling GPT output pins based on GTETRG input pin level
	2. Using POEG software API - This mode supports disabling GPT output pins by R_POEG_OutputDisable() software API request
	3. Using POEG trigger GPT output level - This mode supports disabling GPT output pins when GTIOC pins are the same level
	4. Using POEG trigger ACMPHS Mode - This mode disabling GPT output pins based on comparator crossing event
	5. Reset the POEG status - This option resets the POEG0 and POEG1 status 
	The Port Output can be enabled using the POEG Reset API.
	The status of each of the mode is displayed on J-Link RTT Viewer. The POEG disabled operation is displayed with LED indications.
 
2. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below instructions to observe the operation of the system.
	
	1) Connect the RA MCU device to the host PC using a USB Cable.

	2) Open the RTT viewer to view the application status/transfer messages.	

	3) For Selecting POEG Trigger on GTETRG Input pin level,enter "1":The POEG1 channel number corresponds to the GTETRG input pin which is used with this channel.GTETRG pin level (active LOW or active HIGH based on configuration) disables GPT output pins on the input level. 
	Then reset the resepective POEG status by entering Menu option "5"
		
	4) For Selecting POEG Output disable using Software API,enter "2":The GPT output pins are disabled on R_POEG_OutputDisable() software API.
	Then reset the resepective POEG status by entering Menu option "5"

	5) For Selecting POEG Output disable on GTIOC output level,enter "3":The GPT output pins are disabled when the GTIOC pins are on the same level.
	Then reset the resepective POEG status by entering Menu option "5"
	
	6) For Selecting POEG output disbale on ACMPHS events,enter "4":The GPT output pins are disabled on comparator crossing events.
	Then reset the resepective POEG status by entering Menu option "5"
	Reset of application is needed prior to continuous selection of the menu option "4" to restart the DAC0 and DAC1 and ACMPHS 
	
	7) For Selecting POEG reset of channels,enter "5" , and then Enter option "1" or "2" to reset POEG 0 or POEG 1 respectively.
	
	8) The EP performs the GPT output disable functioning using the above mentioned options.
	Verify the GPT operation using either an Logic analyzer or an LED output.
     
3. Hardware and Software Requirements:
   
   Hardware : 
	1) 1x RA board 
	2) 1x Micro USB cable for programming and debugging
	3) 1x Logic Analyzer (Optional)
	4) 1x Jumper Cable 
	
   Supported Boards: EK-RA6M1, EK-RA6M2, EK_RA6M3, MCK_RA6T2, RSSK_RA6T1

   Hardware Connection:
	MCK-RA6T2 :
		PA07(CN4:11) --> PA00(CN4:9)
   Software: 
	1) e2studio 2022-10 (22.10.0) or greater.
	2) FSP v4.1.0 or greater.
	3) J-Link RTT Viewer version 7.80c or greater.

4. Hardware settings for the project:
	
	1) Connect the micro USB end of the micro USB device cable to Debug port of the board. Connect the other end of this cable to USB port of the 
	   host PC.

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200010bc
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
