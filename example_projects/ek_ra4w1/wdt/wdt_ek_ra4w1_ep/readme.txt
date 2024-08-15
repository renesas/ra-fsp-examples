/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
   The example project demonstrates the typical use of the WDT HAL module APIs.
   User can give input through JLinkRTTViewer to start the WDT. WDT gets refreshed periodically through GPT timer.
   User can press push button to stop the GPT timer which in turn stops refreshing WDT timer.
   Approximately after 2 seconds, WDT resets the MCU and turn LED ON until user gives the input once again.
   
Note: 
1. On RA2L1-EK/RA2E1-EK/RA4M2-EK/RA4M3-EK/RA6M4/RA4E1-FPB/RA6E1-FPB-EK board, use Switch S1 to stop WDT from refreshing.
2. On RA2E2-EK/RA6T1-RSSK/RA6M3-EK/RA6T2-MCK board, use Switch S2 to stop WDT from refreshing.