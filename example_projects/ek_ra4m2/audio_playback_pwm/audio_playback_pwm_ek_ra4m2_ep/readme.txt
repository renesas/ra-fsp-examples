/***********************************************************************************************************************
* 
* Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
*
* This software is supplied by Renesas Electronics America Inc. and may only be used with products of Renesas Electronics Corp.
* and its affiliates (“Renesas”).  No other uses are authorized.  This software is protected under all applicable laws, 
* including copyright laws.
* Renesas reserves the right to change or discontinue this software.
* THE SOFTWARE IS DELIVERED TO YOU “AS IS,” AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT 
* PERMISSIBLE UNDER APPLICABLE LAW,DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE.  TO THE MAXIMUM 
* EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE (OR ANY PERSON 
* OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING, WITHOUT LIMITATION, 
* ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES;
* ANY LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF 
* THE POSSIBILITY OF SUCH LOSS,DAMAGES, CLAIMS OR COSTS.
* 
**********************************************************************************************************************/

1.Project Overview:

This example project demonstrates Audio Playback with PWM module's functionality. It uses ADPCM module to decode the PCM data. The Decoded PCM data is in signed 16 bit format. Application has to convert this signed PCM data into unsigned PCM data and provide the converted unsigned data to the audio playback with PWM module. The converted unsigned data is provided as input and output is generated as PWM signal. Error and info messages will be printed on JlinkRTTViewer.

2. Supported Boards:

EK-RA2A1, EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, RSSK-RA6T1, EK-RA6M5, FPB-RA4E1, FPB-RA6E1, MCK-RA6T2

3. Hardware requirements:
	- Renesas RA board 
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
FPB-RA4E1/FPB-RA6E1               PMOD AMP2
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
EK-RA6M4                          PMOD AMP2                         
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


NOTE:User is expected to enter data of size not exceeding above 15 bytes.
     Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through RTT.

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000234
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

