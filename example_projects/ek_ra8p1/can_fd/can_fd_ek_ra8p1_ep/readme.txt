/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates CAN FD operations on Renesas RA MCUs using two RA boards. On pressing any key
 	on the J-Link RTT Viewer (connected to any one of the two boards), data is transmitted from Board 1 to Board 2.
	On data reception, Board 2 displays the received data on the J-Link RTT Viewer and transmits the updated data
	back to Board 1 as an acknowledgement. Board 1 on reception of data, changes mode from the CAN frame to a
	CAN FD frame and transmits data to Board 2. On reception, Board 2 transmits updated data back to Board 1 as an
	acknowledgement. On successful transmission and reception, both boards print the transmitted and received data
	on the J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA Boards: EK-RA6M5, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1, MCK-RA8T1,
			     FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA8P1.
	2 x Renesas RA boards.
	2 x Type-C USB cables for programming and debugging.
	1 x OM13099 CAN-FD shield if on-board CAN transceivers are not populated 
	    (e.g., https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/dual-can-fd-transceiver-shield:OM13099)
	Some jumper wires.

3. Hardware Connections:
	The boards used for testing can be of the same type (e.g., EK-RA8M1 <--> EK-RA8M1) or different types (e.g., EK-RA8M1 <--> MCK-RA6T3).
		+ In the case of same-type boards, if the on-board CAN transceivers are populated, they can be used for
		  testing by connecting CANH, CANL, and GND lines directly.

		+ If using same-type boards without populated on-board CAN transceivers, an external OM13099 board can
 		  be used. Connect the required wires as described in the readme.txt under the section Reference:
 		  External Hardware – OM13099 (CAN Transceiver Board).

		+ For testing on different-type boards (e.g., EK-RA8D1 <--> MCK-RA6T3) without on-board CAN transceivers,
 		  the OM13099 board can also be used externally. Connect the necessary wires as described in the optional
		  sections of the readme.txt for the respective boards.

	Additionally, users are required to check the connections and settings as noted in the "Note:" section. 
       	The CAN settings such as Clock and Bitrate need to match.

       	For EK-RA6M5:
       		Short E33, E34 & E35 in order to use onboard CAN Transceivers 

       		J33:1 of Board 1 to J33:1 of Board 2
       		J33:2 of Board 1 to J33:2 of Board 2
       		J33:3 of Board 1 to J33:3 of Board 2
       
       		[Optional] Use External Hardware: OM13099 (CAN Transceiver Board)
        	Board 1               	OM13099 (CAN Transceiver Board)		Board 2
                               
        	CTX P609 (J3:12)       	CAN0 TX (J3:4)                  	---
        	CRX P610 (J3:13)       	CAN0 RX (J3:2)                  	---
        	VCC (J18:4)            	3V3 (J3:12)                    		---
        	5V0 (J18:5)            	5V0 (J3:14)                    		---
        	GND (J18:6)            	GND (J3:16)                     	--- 
            	     ---              	CAN1 TX (J1:4)                  	CTX P609 (J3:12)
             	     ---               	CAN1 RX (J1:2)                 		CRX P610 (J3:13)
             	     ---               	P3:2 <-> P3:13                  	---
             	     ---               	P3:7 <-> P3:18                  	---
		Note: To use the external CAN transceiver (OM13099), the user must either cut E33, E34, and E35 to 
		      prevent conflicts with the onboard CAN transceiver or configure alternative CAN CTX and CRX 
		      pins instead of cutting E33, E34, and E35.

        For EK-RA4E2, EK-RA6E2:
        	Use onboard CAN Transceiver:
        	J32:1 of Board 1 to J32:1 of Board 2
        	J32:2 of Board 1 to J32:2 of Board 2
        	J32:3 of Board 1 to J32:3 of Board 2

        For MCK-RA4T1, MCK-RA6T3:
        	External Hardware: OM13099 (CAN Transceiver Board) 
        
        	Board 1            	OM13099 (CAN Transceiver Board)    	Board 2
                               
        	CTX P401 (CN6:2)     	CAN0 TX (J3:4)                       	---
        	CRX P402 (CN6:3)     	CAN0 RX (J3:2)                       	---
        	VCC (CN2:24)         	3V3 (J3:12)                          	---
        	5V0 (CN3:33)          	5V0 (J3:14)                          	---
        	GND (CN3:31)           	GND (J3:16)                          	--- 
         	    ---               	CAN1 TX (J1:4)                     	CTX P401 (CN6:2)
          	    ---               	CAN1 RX (J1:2)                     	CRX P402 (CN6:3)
         	    ---               	P3:2 <-> P3:13                        	---
       	  	    ---               	P3:7 <-> P3:18                        	---

	For EK-RA8M1:
        	Use onboard CAN Transceiver:
       		J62:1 of Board 1 to J62:1 of Board 2
       		J62:2 of Board 1 to J62:2 of Board 2
       		J62:3 of Board 1 to J62:3 of Board 2
        
       		[Optional] Use External Hardware: OM13099 (CAN Transceiver Board)
        	Board 1                	OM13099 (CAN Transceiver Board)    	Board 2
                               
        	CTX P312 (J51:12)      	CAN0 TX (J3:4)                       	---
        	CRX P311 (J51:8)       	CAN0 RX (J3:2)                       	---
        	VCC (J18:4)            	3V3 (J3:12)                          	---
        	5V0 (J18:5)            	5V0 (J3:14)                          	---
        	GND (J18:6)            	GND (J3:16)                          	--- 
         	    ---               	CAN1 TX (J1:4)                     	CTX P312 (J51:12)
         	    ---               	CAN1 RX (J1:2)                     	CRX P311 (J51:8)
         	    ---               	P3:2 <-> P3:13                        	---
         	    ---               	P3:7 <-> P3:18                        	---
		Note: To use the external CAN transceiver (OM13099), the user must either cut E48, E49, and E50 to
		      prevent conflicts with the onboard CAN transceiver or configure alternative CAN CTX and CRX pins 
		      instead of cutting E48, E49, and E50.

	For EK-RA8D1:
        	External Hardware: OM13099 (CAN Transceiver Board) 
        
        	Board 1                	OM13099 (CAN Transceiver Board)    	Board 2
                               
        	CTX P401 (J51:7)       	CAN0 TX (J3:4)                   	---
        	CRX P402 (J52:15)      	CAN0 RX (J3:2)                       	---
        	VCC (J18:4)            	3V3 (J3:12)                          	---
        	5V0 (J18:5)            	5V0 (J3:14)                          	---
        	GND (J18:6)            	GND (J3:16)                          	--- 
         	    ---               	CAN1 TX (J1:4)                     	CTX P401 (J51:7)
         	    ---               	CAN1 RX (J1:2)                     	CRX P402 (J52:15)
         	    ---               	P3:2 <-> P3:13                        	---
         	    ---               	P3:7 <-> P3:18                        	---

	For MCK-RA8T1:
        	External Hardware: OM13099 (CAN Transceiver Board) 

        	Board 1                	OM13099 (CAN Transceiver Board)    	Board 2

        	CTX P415 (CN5:2)       	CAN0 TX (J3:4)                       	---
        	CRX P414 (CN5:3)       	CAN0 RX (J3:2)                       	---
        	VCC (CN3:24)           	3V3 (J3:12)                          	---
        	5V0 (CN4:34)           	5V0 (J3:14)                          	---
        	GND (CN3:32)           	GND (J3:16)                          	--- 
        	     ---               	CAN1 TX (J1:4)                     	CTX P415 (CN5:2)
        	     ---               	CAN1 RX (J1:2)                     	CRX P414 (CN5:3)
        	     ---               	P3:2 <-> P3:13                       	---
        	     ---               	P3:7 <-> P3:18                       	---

	For FPB-RA8E1:
        	External Hardware: OM13099 (CAN Transceiver Board) 

        	Board 1                	OM13099 (CAN Transceiver Board)    	Board 2

        	CTX P103 (J1:46)      	CAN0 TX (J3:4)                       	---
        	CRX P102 (J1:43)      	CAN0 RX (J3:2)                       	---
        	VCC (J18:4)            	3V3 (J3:12)                          	---
        	5V0 (J18:5)            	5V0 (J3:14)                          	---
        	GND (J18:6)            	GND (J3:16)                          	--- 
        	     ---               	CAN1 TX (J1:4)                     	CTX P103 (J1:46)
        	     ---               	CAN1 RX (J1:2)                     	CRX P102 (J1:43)
        	     ---               	P3:2 <-> P3:13                       	---
        	     ---               	P3:7 <-> P3:18                       	---

	For EK-RA4L1:
		Use onboard CAN Transceiver:
		J33:1 of Board 1 to J33:1 of Board 2
       		J33:2 of Board 1 to J33:2 of Board 2
       		J33:3 of Board 1 to J33:3 of Board 2

	For EK-RA8E2:
		Use onboard CAN Transceiver:
		J7:1 of Board 1 to J7:1 of Board 2
       		J7:2 of Board 1 to J7:2 of Board 2
       		J7:3 of Board 1 to J7:3 of Board 2

	For EK-RA8P1:
        	External Hardware: OM13099 (CAN Transceiver Board) 

        	Board 1                	OM13099 (CAN Transceiver Board)    	Board 2

        	CTX P312 (J17:4)      	CAN0 TX (J3:4)                       	---
        	CRX P311 (J17:3)      	CAN0 RX (J3:2)                       	---
        	VCC (J1:7)            	3V3 (J3:12)                          	---
        	5V0 (J1:8)            	5V0 (J3:14)                          	---
        	GND (J1:40)            	GND (J3:16)                          	--- 
        	     ---               	CAN1 TX (J1:4)                     	CTX P312 (J17:4)
        	     ---               	CAN1 RX (J1:2)                     	CRX P311 (J17:3)
        	     ---               	P3:2 <-> P3:13                       	---
        	     ---               	P3:7 <-> P3:18                       	---

4. Hardware Configuration:

	For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |      OFF   |      OFF    |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

	CAUTION: Do not enable SW1-4 and SW1-5 together

Note:
1) In this example project, the same code runs on both boards. The board from which the user initiates the transmission
   becomes the Board 1 and the board which receives the data becomes the Board 2.

2) LED behavior as below:
   For boards have 3 LEDs:
   a. LED1 will be turned ON when CAN transmission operation is in progress.
   b. LED2 will be turned ON when CAN transmission operation is successful.
   c. LED3 will be turned ON when an error occurs.
   
   For boards have 2 LEDs:
   a. LED1 will be turned ON when CAN transmission operation is in progress.
   b. LED2 will be turned ON when CAN transmission operation is successful.
   c. LED1 and LED2 will be turned ON when an error occurs.
      
3) The user is expected to enter data not exceeding 15 bytes in size.
      
4) For OM13099 (CAN Transceiver Board):
   a. Connect the P3 loopback connection on CAN Transceiver Board using jumper cables instead of a DB9 serial cable.
   b. Connect jumpers JP3, JP4, JP5, and JP6 on OM13099 board to establish connection to CAN termination resistors.
      
5) For using different hardware in transmission and reception (e.g., transmission is EK-RA8D1 board, reception is
   MCK-RA6T3 board), if EP didn't work, please recheck actual data bitrate information (in hal_data.c) and ensure
   they are matched between transmission and reception. Recommend to set CANFDCLK is 40MHz/80MHz to have same data
   bitrate between boards.       

6) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2200050c
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
7) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
