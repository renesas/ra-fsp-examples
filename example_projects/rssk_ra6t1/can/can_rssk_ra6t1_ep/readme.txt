/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	 
	This project demonstrates the basic functionality of CAN module on Renesas RA MCUs using 2 RA boards.
	On pressing any key on the RTTViewer, data is transmitted from Board1 to Board2.
	On reception, Board2 displays the received data on the RTTViewer. Board2, then, transmits the
	framed data back to Board1. On successful transmission, Board1 prints the data on to the RTTViewer.

2. Hardware Connections:
       Require 2 CAN transceiver boards (Part# Waveshare SN65HVD230) and 2 RA boards to run the project

       Supported RA Boards: ek-ra2a1, ek-ra4m1, ek-ra2l1, ek-ra4m2, ek-ra4m3, ek-ra6m1, ek-ra6m2, ek-ra6m3, ek-ra6m3g, ek-ra6m4, rssk-ra6t1, fpb-ra6e1, fpb-ra4e1
       (Please refer Note* for ek-ra2a1, ek-ra4m2 and ek-ra4m3,fpb-ra6e1,fpb-ra4e1 pin connections) 
 
        CAN Transceiver Board1:
	   CANL:Transceiver Board 2 (CANL)
           CANH:Transceiver Board 2 (CANH)
	   CTX: P-401 (RA Board 1)
	   CRX: P-402 (RA Board 1)
	   3.3V: Vcc (RA Board 1)
	   GND: Vss/GND (RA Board 1)

        CAN Transceiver Board2:
	   CANL:Transceiver Board 1 (CANL)
	   CANH:Transceiver Board 1 (CANH)	 
	   CTX: P-401 (RA Board 2)
	   CRX: P-402 (RA Board 2)
	   3.3V: Vcc (RA Board 2)
	   GND: Vss/GND (RA Board 2)
        
        For EK-RA2L1 soldering of E7 and E11 bridge is required to enable 20MHz crystal.

*Note: 1. On ek-ra4m2 ,fpb-ra4e1 and fpb-ra6e1 configured channel:0 and pins are CTX:P-103 and CRX:P-102
       2. On ek-ra4m3, configured channel:1 and pins are CTX:P-609 and CRX:P-610
       3. On ek-ra2a1, configured channel:0 and pins are CTX:P-304 and CRX:P-303
       4. On ek-ra2l1, configured channel:0 and pins are CTX:P-103 and CRX:P-102
       5. User should update WAIT_TIME macro in hal_entry.c based on MCU's ICLK because ICLK value varies for each MCU series.
	   6. Current support is only limited to static baud rate, dynamic Baud rate change is not supported.	   

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x1ffe0d74
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
