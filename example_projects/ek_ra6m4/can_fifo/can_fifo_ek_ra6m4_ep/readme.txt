/***********************************************************************************************************************
* 
* Copyright [2023-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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

1. Project Overview:
	 
	This project demonstrates transmit and receive FIFO operations in the context of CAN (Controller Area Network)
	on Renesas RA MCUs using 2 RA boards. On Board 1 enter any key on RTT viewer to send a remote frame to Board 2 
	to request data. When receiving a remote frame, Board 2 will send four messages back-to-back to Board 1. 
	Board 1 receives data and displays it on the RTT viewer. On successful comparison of data, Board 1 will transmit
	a data frame to Board 2 as received acknowledgment. Error messages and information will be printed on the J-link RTT Viewer.

2. Hardware Requirements:

	2 x Renesas RA boards.
	2 x Micro USB cable.
	2 x CAN transceiver boards(Waveshare SN65HVD230).
	10 x Connection wires : 8-Both sided Female, 2-Both sided Male (For RSSK-RA6T1: 8-Male to female, 2-Both sided Male)
	 
3. Hardware Connections:

       Supported RA Boards: EK-RA2A1,EK-RA2L1,FPB-RA6E1,FPB-RA4E1,EK-RA4M1,EK-RA4M2,EK-RA4M3,EK-RA6M1,EK-RA6M2,EK-RA6M3,EK-RA6M3G,
	   EK-RA6M4,RSSK-RA6T1.
	   
	   EK-RA2A1:
	   
	        Board1           CAN Transceiver(Board1)      CAN Transceiver(Board2)         Board2
	   ------------------------------------------------------------------------------------------------
	                                  CANL   <---------------->  CANL
	                                  CANH   <---------------->  CANH
	   3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
	   GND        <---------------->  GND                        GND   <---------------->  GND      
	   CRX0-P303  <---------------->  CRX                        CRX   <---------------->  CRX0-P303
	   CTX0-P304  <---------------->  CTX                        CTX   <---------------->  CTX0-P304
	   
	   
	   EK-RA2L1,EK-RA4M2,FPB-RA6E1,FPB-RA4E1:
	   
	        Board1           CAN Transceiver(Board1)      CAN Transceiver(Board2)         Board2
	   ------------------------------------------------------------------------------------------------
	                                  CANL   <---------------->  CANL
	                                  CANH   <---------------->  CANH
	   3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
	   GND        <---------------->  GND                        GND   <---------------->  GND      
	   CRX0-P102  <---------------->  CRX                        CRX   <---------------->  CRX0-P102
	   CTX0-P103  <---------------->  CTX                        CTX   <---------------->  CTX0-P103


	   EK-RA4M1,EK-RA6M1,EK-RA6M2,EK-RA6M3,EK-RA6M3G,EK-RA6M4,RSSK-RA6T1:
	   Note: For RSSK-RA6T1, 3.3V(CN6-Pin3), GND(CN6-Pin4), CRX0-P402(CN6-Pin2) and CTX0-P401(CN6-Pin1).
	   
	        Board1           CAN Transceiver(Board1)      CAN Transceiver(Board2)           Board2
	   ------------------------------------------------------------------------------------------------
	                                  CANL   <---------------->  CANL
	                                  CANH   <---------------->  CANH
	   3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
	   GND        <---------------->  GND                        GND   <---------------->  GND      
	   CRX0-P402  <---------------->  CRX                        CRX   <---------------->  CRX0-P402
	   CTX0-P401  <---------------->  CTX                        CTX   <---------------->  CTX0-P401
	   
	  
	   EK-RA4M3:

	        Board1           CAN Transceiver(Board1)      CAN Transceiver(Board2)           Board2
	   ------------------------------------------------------------------------------------------------
	                                  CANL   <---------------->  CANL
	                                  CANH   <---------------->  CANH
	   3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
	   GND        <---------------->  GND                        GND   <---------------->  GND      
	   CRX1-P610  <---------------->  CRX                        CRX   <---------------->  CRX1-P610
	   CTX1-P609  <---------------->  CTX                        CTX   <---------------->  CTX1-P609

Note:   
	1) Segger RTT block address may need to be updated to observe the EP operation using a hex file with RTT-viewer.
	   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008ac
	   b. Keil: Not Available 
	   c. IAR: Not Available
	 
	2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
	   in .map file generated in the build configuration folder (Debug/Release).
