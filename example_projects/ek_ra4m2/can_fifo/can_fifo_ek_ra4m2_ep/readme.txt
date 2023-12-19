/***********************************************************************************************************************
* 
* Copyright [2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
	2 x CAN transceiver boards(Waveshare SN65HVD230).
	10 x Connection wires : 8-Both sided Female, 2-Both sided Male.
	 
2 . Hardware Connections:

       Supported RA Boards: EK-RA4M2
	   
	   EK-RA4M2(Board1)      CAN Transceiver(Board1)      CAN Transceiver(Board2)      EK-RA4M2(Board2) 
	   ----------------      ----------------------       ----------------------       ----------------
	                                  CANL   <---------------->  CANL
	                                  CANH   <---------------->  CANH
	   3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
	   GND        <---------------->  GND                        GND   <---------------->  GND      
	   CRX0-P102  <---------------->  CRX                        CRX   <---------------->  CRX0-P102
	   CTX0-P103  <---------------->  CTX                        CTX   <---------------->  CTX0-P103
	   
1) Segger RTT block address may need to be updated to observe the EP operation using a hex file with RTT-viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008ac
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
