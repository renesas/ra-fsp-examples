/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates typical use of GPT HAL module APIs.
        User has the provision to input value as per displayed menu through JLinkRTTViewer to select different GPT supported modes(Periodic, PWM, One-Shot).
        In periodic mode, user can enter the time period within the permitted ranges to change the frequency of the user LED.
        In PWM mode, user can enter the duty cycle within the specified range to adjust the intensity of the user LED.
        In One-Shot mode, output will be displayed on JlinkRTTViewer.
        Any failure will also be displayed on JlinkRTTViewer.

2. Hardware Settings:
    Hardware Connection
 
    For EK-RA2L1:
    Connect P414(J1 Pin 04) --> P505(J3 Pin 06)
    For EK-RA2E1:
    Connect P212(J2 Pin 04) --> P913(J2 Pin 12)
    For EK-RA6M5:
    Connect P414(J1 Pin 32) --> P008(J4 Pin 26)
    For FPB-RA4E1:
    Connect P104(J4 Pin 21) --> P407(J3 Pin 25)


NOTE: 
1.Timer period range in millisecond as per EP configuration: 
        1. RA6M1/RA6M2/RA6M3/RA6M3G/RA6T1/RA6T2       	--      (0 to 35791) ms 
        2. RA4M1/RA2L1/RA2E1                   		--      (0 to 89478) ms
        3. RA2A1/RA2E2                         		--      (0 to 1398)  ms
        4. RA4W1                               		--      (0 to 2097)  ms
        5. RA6M5/RA6M4/RA4M3/RA4M2/FPB-RA4E1   		--      (0 to 42949) ms
        6. RA6E1/RA4T1/RA6T3                            --      (0 to 671)   ms
        
2.User is expected to enter data of size not exceeding above 15 bytes.
  Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through RTT.﻿
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008a4
   b. Keil:	0x2000083c
   c. IAR: 0x200008d0
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
