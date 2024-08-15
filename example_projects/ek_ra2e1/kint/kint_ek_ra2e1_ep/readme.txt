/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

        The example project demonstrates the typical use of the KINT HAL module APIs using matrix keypad for 4 keys (2X2)(row, column).
        It utilizes two keymatrix channel masks(KINT pins) and two GPIOs which will be connected to keypad's columns and rows respectively.
        On press of each key (one at a time), Row and Column information for the key pressed will be displayed in JlinkRTTViewer.

2. Hardware settings for the project:

        Hardware : 4x4 Matrix Membrane Keypad(Optional)
        Hardware connections:
        EK_RA6M3, EK_RA6M3G, EK_RA6M2, EK_RA6M1, EK_RA4M1, EK_RA4W1, EK-RA2L1, EK-RA2E1:
        1) (KINT pin)P106 -> Any Column pin of keypad.
        2) (KINT pin)P107 -> Any Column different from above pin of keypad.
        3) (GPIO)P101     -> Any row pin of keypad.
        4) (GPIO)P102     -> Any other row pin of keypad.
        
        RSSK_RA6T1 :
        1) (KINT pin)P100 -> Any Column pin of keypad.
        2) (KINT pin)P103 -> Any Column different from above pin of keypad.
        3) (GPIO)P101     -> Any row pin of keypad.
        4) (GPIO)P102     -> Any other row pin of keypad.

        EK_RA2A1:
        1) (KINT pin)P304 -> Any Column pin of keypad.
        2) (KINT pin)P303 -> Any Column different from above pin of keypad.
        3) (GPIO)P301     -> Any row pin of keypad.
        4) (GPIO)P302     -> Any other row pin of keypad.

        EK_RA2E2:
        1) (KINT pin)P110 -> Any Column pin of keypad.
        2) (KINT pin)P109 -> Any Column different from above pin of keypad.
        3) (GPIO)P111     -> Any row pin of keypad.
        4) (GPIO)P112     -> Any other row pin of keypad.

        MCK_RA6T2:
	1) (KINT pin)PA08 (CN 7:33) -> Any Column pin of keypad.
        2) (KINT pin)PA09 (CN 7:31) -> Any Column different from above pin of keypad.
        3) (GPIO)PA10     (CN 6:3)  -> Any row pin of keypad.
        4) (GPIO)PA11     (CN 6:1)  -> Any other row pin of keypad.

Note :
1. External pull down resistors (4.7 Kohm) should be connected to KINT pins
2. If matrix keypad is not available, Please connect the Jumper wires to pins as specified above for particular MCU 
   and short row pin with any of the KINT pin.This will lead to generate key-event.
   The generated key-event is detected and printed on RTTViewer.
