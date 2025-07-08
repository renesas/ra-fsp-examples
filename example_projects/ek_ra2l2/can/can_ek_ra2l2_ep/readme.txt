/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of CAN module on Renesas RA MCUs using two RA boards.
	On pressing any key on the J-Link RTT Viewer triggers data transmission from Board 1 to Board 2.
	On reception, Board 2 displays the received data on the J-Link RTT Viewer and then transmits the
	framed data back to Board 1. On successful transmission, Board 1 prints the data on the J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA Boards: EK-RA2A1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA2L1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G,
  			     EK-RA6M4, FPB-RA4E1, FPB-RA6E1, RSSK-RA6T1, EK-RA2L2.
	2 x Renesas RA boards.
	2 x Micro USB cables for programming and debugging.
	2 x CAN transceiver boards (Waveshare SN65HVD230).
	Some jumper wires.

3. Hardware Connections:
	CAN Transceiver Board 1:
		CANL: Transceiver Board 2 (CANL)
		CANH: Transceiver Board 2 (CANH)
		CTX:  P401 (RA Board 1)
		CRX:  P402 (RA Board 1)
		3.3V: VCC (RA Board 1)
		GND:  VSS/GND (RA Board 1)

	CAN Transceiver Board 2:
		CANL: Transceiver Board 1 (CANL)
		CANH: Transceiver Board 1 (CANH)	 
		CTX:  P401 (RA Board 2)
		CRX:  P402 (RA Board 2)
		3.3V: VCC (RA Board 2)
		GND:  VSS/GND (RA Board 2)
        
	For EK-RA2L1 soldering of E7 and E11 bridge is required to enable 20MHz crystal.

	*Note:  
		1. On EK-RA4M2, FPB-RA4E1, FPB-RA6E1, and EK-RA2L1: Configured channel 0 with pins CTX: P103 and CRX: P102.
		2. On EK-RA4M3: Configured channel 1 with pins CTX: P609 and CRX: P610.
		3. On EK-RA2A1: Configured channel 0 with pins CTX: P304 and CRX: P303.
		4. On EK-RA2L2: Configured channel 0 with pins CTX: P102 (J2:37) and CRX: P103 (J2:36).

Note:
1) The user should update WAIT_TIME macro in hal_entry.c based on MCU's ICLK because ICLK value varies for each MCU series.

2) Current support is only limited to static baud rate, dynamic Baud rate change is not supported.	   

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004d70
   b. Keil:     Not Available 
   c. IAR:      Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).