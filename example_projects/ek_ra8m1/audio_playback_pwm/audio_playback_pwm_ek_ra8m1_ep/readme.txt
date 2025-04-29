/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

This example project demonstrates Audio Playback with PWM module's functionality. It uses ADPCM module to decode the PCM data. The Decoded PCM data is in signed 16 bit format. Application has to convert this signed PCM data into unsigned PCM data and provide the converted unsigned data to the audio playback with PWM module. The converted unsigned data is provided as input and output is generated as PWM signal. Error and info messages will be printed on JlinkRTTViewer.

2. Supported Boards:
	EK-RA2A1, EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, RSSK-RA6T1, EK-RA6M5, 
	FPB-RA4E1, FPB-RA6E1, MCK-RA6T2, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA2A2

3. Hardware requirements:
	- Renesas RA board
	- 1 x Micro USB cable or 1 x Type C USB cable (For MCK-RA6T2,MCK-RA4T1,MCK-RA6T3,MCK-RA8T1) or 1 x Mini USB cable (For RSSK-RA6T1) 
	- Digilent PMOD AMP2 (SKU: 410-233)
	- Wired Headsets with 3.5mm audio jack
	- Connection wires

4. Hardware Connection:

	EK-RA2A1                          PMOD AMP2
	---------------------------------------------
	PWM_OUTPUT_PIN(P109)       ---->  AIN  (Pin 1)
	VCC --(via 22k ohms resistor)--> ~SHUTDOWN (Pin 4)
	VCC                        ---->  VCC (Pin 6)
	GND                        ---->  GAIN(Pin 2)
	GND                        ---->  GND (Pin 5)


	EK-RA2L1                          PMOD AMP2
	---------------------------------------------
	PWM_OUTPUT_PIN(P111)       ---->  AIN  (Pin 1)
	VCC --(via 22k ohms resistor)--> ~SHUTDOWN (Pin 4)
	VCC                        ---->  VCC (Pin 6)
	GND                        ---->  GAIN(Pin 2)
	GND                        ---->  GND (Pin 5)


	EK-RA4M1/EK-RA6M1
	EK-RA6M5/RSSK-RA6T1
	FPB-RA4E1/FPB-RA6E1
	EK-RA4E2/EK-RA6E2
	MCK-RA4T1/MCK-RA6T3               PMOD AMP2
	---------------------------------------------
	PWM_OUTPUT_PIN(P500)       ---->  AIN  (Pin 1)
	VCC --(via 22k ohms resistor)--> ~SHUTDOWN (Pin 4)
	VCC                        ---->  VCC (Pin 6)
	GND                        ---->  GAIN(Pin 2)
	GND                        ---->  GND (Pin 5)


	EK-RA4M2/EK-RA6M3                 PMOD AMP2
	---------------------------------------------
	PWM_OUTPUT_PIN(P107)       ---->  AIN  (Pin 1)
	VCC --(via 22k ohms resistor)--> ~SHUTDOWN (Pin 4)
	VCC                        ---->  VCC (Pin 6)
	GND                        ---->  GAIN(Pin 2)
	GND                        ---->  GND (Pin 5)


	EK-RA4W1                          PMOD AMP2
	---------------------------------------------
	PWM_OUTPUT_PIN(P501)       ---->  AIN  (Pin 1)
	VCC --(via 22k ohms resistor)--> ~SHUTDOWN (Pin 4)
	VCC                        ---->  VCC (Pin 6)
	GND                        ---->  GAIN(Pin 2)
	GND                        ---->  GND (Pin 5)


	EK-RA4M3/EK-RA6M2
	EK-RA6M4/EK-RA8M1/EK-RA8D1        PMOD AMP2                         
	---------------------------------------------
	PWM_OUTPUT_PIN(P713)       ---->  AIN  (Pin 1)
	VCC --(via 22k ohms resistor)--> ~SHUTDOWN (Pin 4)
	VCC                        ---->  VCC (Pin 6)
	GND                        ---->  GAIN(Pin 2)
	GND                        ---->  GND (Pin 5)


	MCK-RA6T2                          PMOD AMP2
	---------------------------------------------
	PWM_OUTPUT_PIN(PB04(CN4:29))--->  AIN  (Pin 1)
	VCC --(via 22k ohms resistor)--> ~SHUTDOWN (Pin 4)
	VCC                        ---->  VCC (Pin 6)
	GND                        ---->  GAIN(Pin 2)
	GND                        ---->  GND (Pin 5)


	MCK-RA8T1				    PMOD AMP2
	-------------------------------------------------------
	PWM_OUTPUT_PIN (P411(CN6:P9))        ---->  AIN  (Pin 1)
	VCC (CN3:P23) --(via 22k ohms resistor)--> ~SHUTDOWN (Pin 4)
	VCC (CN3:P24)                        ---->  VCC (Pin 6)
	GND (CN3:P32)                        ---->  GAIN(Pin 2)
	GND (CN3:P34)                        ---->  GND (Pin 5)


	EK-RA2A2                          PMOD AMP2
	---------------------------------------------
	PWM_OUTPUT_PIN(P110(J4:P9)) --->  AIN  (Pin 1)
	VCC --(via 22k ohms resistor)--> ~SHUTDOWN (Pin 4)
	VCC                        ---->  VCC (Pin 6)
	GND                        ---->  GAIN(Pin 2)
	GND                        ---->  GND (Pin 5)

Note: 
1) User is expected to enter data of size not exceeding above 15 bytes.
   Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through RTT.

2) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000528
   b. Keil: Not Available 
   c. IAR: Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

