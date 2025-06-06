/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview: 
	This project demonstrates transmit and receive FIFO operations in the context of CAN (Controller Area Network)
	on Renesas RA MCUs using 2 RA boards. On Board 1 enter any key on the J-Link RTT Viewer to send a remote frame to Board 2 
	to request data. When receiving a remote frame, Board 2 will send four messages back-to-back to Board 1. 
	Board 1 receives data and displays it on the J-Link RTT Viewer. On successful comparison of data, Board 1 will transmit
	a data frame to Board 2 as received acknowledgment. Error messages and information will be printed on the J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA Boards: EK-RA2A1, EK-RA2L1, FPB-RA6E1, FPB-RA4E1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, 
			     EK-RA6M3, EK-RA6M3G, EK-RA6M4, RSSK-RA6T1, EK-RA2L2.
	2 x Renesas RA boards.
	2 x Micro USB cables.
	2 x CAN transceiver boards (Waveshare SN65HVD230).
	10 x Connection wires: 8-Both sided Female, 2-Both sided Male (For RSSK-RA6T1: 8-Male to female, 2-Both sided Male).
	 
3. Hardware Connections:
	For EK-RA2A1:

		Board 1          CAN Transceiver (Board 1)        CAN Transceiver (Board 2)         Board 2
		------------------------------------------------------------------------------------------------
					       CANL   <---------------->  CANL
					       CANH   <---------------->  CANH
		3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
		GND        <---------------->  GND                        GND   <---------------->  GND      
		CRX0 P303  <---------------->  CRX                        CRX   <---------------->  CRX0 P303
		CTX0 P304  <---------------->  CTX                        CTX   <---------------->  CTX0 P304


	For EK-RA2L1, EK-RA4M2, FPB-RA6E1, FPB-RA4E1:

		Board 1          CAN Transceiver (Board 1)        CAN Transceiver (Board 2)         Board 2
		------------------------------------------------------------------------------------------------
					       CANL   <---------------->  CANL
					       CANH   <---------------->  CANH
		3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
		GND        <---------------->  GND                        GND   <---------------->  GND      
		CRX0 P102  <---------------->  CRX                        CRX   <---------------->  CRX0 P102
		CTX0 P103  <---------------->  CTX                        CTX   <---------------->  CTX0 P103


	For EK-RA4M1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, RSSK-RA6T1:
		Note: For RSSK-RA6T1, 3.3V (CN6:3), GND (CN6:4), CRX0 P402 (CN6:2) and CTX0 P401 (CN6:1).

		Board 1          CAN Transceiver (Board 1)        CAN Transceiver (Board 2)         Board 2
		------------------------------------------------------------------------------------------------
					       CANL   <---------------->  CANL
					       CANH   <---------------->  CANH
		3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
		GND        <---------------->  GND                        GND   <---------------->  GND      
		CRX0 P402  <---------------->  CRX                        CRX   <---------------->  CRX0 P402
		CTX0 P401  <---------------->  CTX                        CTX   <---------------->  CTX0 P401

	For EK-RA4M3:

		Board 1          CAN Transceiver (Board 1)        CAN Transceiver (Board 2)         Board 2
		------------------------------------------------------------------------------------------------
					       CANL   <---------------->  CANL
					       CANH   <---------------->  CANH
		3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
		GND        <---------------->  GND                        GND   <---------------->  GND      
		CRX1 P610  <---------------->  CRX                        CRX   <---------------->  CRX1 P610
		CTX1 P609  <---------------->  CTX                        CTX   <---------------->  CTX1 P609

	For EK-RA2L2:

		Board 1          CAN Transceiver (Board 1)        CAN Transceiver (Board 2)         Board 2
		------------------------------------------------------------------------------------------------
					       CANL   <---------------->  CANL
					       CANH   <---------------->  CANH
		3.3V       	   <-------->  3.3V                       3.3V  <---------------->  3.3V     
		GND        	   <-------->  GND                        GND   <---------------->  GND      
		CRX0 P103 (J2:36)  <-------->  CRX                        CRX   <---------------->  CRX0 P103 (J2:36)
		CTX0 P102 (J2:37)  <-------->  CTX                        CTX   <---------------->  CTX0 P102 (J2:37)

Note:   
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200048a4
   b. Keil:     Not Available 
   c. IAR:      Not Available
	 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).