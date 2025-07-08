/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the KINT HAL module APIs using matrix keypad for 4 keys (2X2)(row, column).
        It utilizes two keymatrix channel masks (KINT pins) and two GPIOs which will be connected to keypad's columns and rows respectively.
        On press of each key (one at a time), Row and Column information for the key pressed will be displayed in JlinkRTTViewer.

2. Hardware settings for the project:
        Hardware : 4x4 Matrix Membrane Keypad (Optional)

        Hardware connections:
        EK-RA6M3, EK-RA6M3G, EK-RA6M2, EK-RA6M1, EK-RA4M1, EK-RA4W1, EK-RA2L1, EK-RA2E1:
          1) (KINT pin)P106 -> Any Column pin of keypad.
          2) (KINT pin)P107 -> Any Column different from above pin of keypad.
          3) (GPIO)P101     -> Any row pin of keypad.
          4) (GPIO)P102     -> Any other row pin of keypad.
        
        RSSK-RA6T1 :
          1) (KINT pin)P100 -> Any Column pin of keypad.
          2) (KINT pin)P103 -> Any Column different from above pin of keypad.
          3) (GPIO)P101     -> Any row pin of keypad.
          4) (GPIO)P102     -> Any other row pin of keypad.

        EK-RA2A1:
          1) (KINT pin)P304 -> Any Column pin of keypad.
          2) (KINT pin)P303 -> Any Column different from above pin of keypad.
          3) (GPIO)P301     -> Any row pin of keypad.
          4) (GPIO)P302     -> Any other row pin of keypad.

        EK-RA2E2:
          1) (KINT pin)P110 -> Any Column pin of keypad.
          2) (KINT pin)P109 -> Any Column different from above pin of keypad.
          3) (GPIO)P111     -> Any row pin of keypad.
          4) (GPIO)P112     -> Any other row pin of keypad.

        MCK-RA6T2:
	  1) (KINT pin)PA08 (CN 7:33) -> Any Column pin of keypad.
          2) (KINT pin)PA09 (CN 7:31) -> Any Column different from above pin of keypad.
          3) (GPIO)PA10     (CN 6:3)  -> Any row pin of keypad.
          4) (GPIO)PA11     (CN 6:1)  -> Any other row pin of keypad.

	FPB-RA2E3:
          1) (KINT pin)P100 -> Any Column pin of keypad.
          2) (KINT pin)P101 -> Any Column different from above pin of keypad.
          3) (GPIO)P102     -> Any row pin of keypad.
          4) (GPIO)P103     -> Any other row pin of keypad.

Note :
1) External pull down resistors (4.7 Kohm) should be connected to KINT pins.

2) If matrix keypad is not available, Please connect the Jumper wires to pins as specified above for particular MCU 
   and short row pin with any of the KINT pin.This will lead to generate key-event.
   The generated key-event is detected and printed on RTTViewer.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004840
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
4. If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
 