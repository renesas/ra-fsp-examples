/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates CAN-FD operations on Renesas RA MCUs using 2 RA boards.
        On pressing any key on the J-Link RTT Viewer (connected to any one of the 2 boards), data is transmitted from Board1 to Board2.
        On data reception, Board2 displays the received data on the J-Link RTT Viewer and Board2 transmits updated data back to Board1 
	as an acknowledgment. 
        Board1 on reception of data, changes mode from CAN frame to CAN-FD frame and transmits data to board2.
        On reception, board2 transmits updated data back to board1 as an acknowledgment. On successful transmission and reception,
        Boards prints the transmitted & received data on to J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA Boards: EK-RA6M5, EK-RA4E2, EK-RA6E2, MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA4L1
	2 x Renesas RA boards.
	2 x Type-C USB cables for programming and debugging.
	1 x External OM13099 board if on-board CAN transceivers not populated.
	Some jumper wires.

3. Hardware Connections:
	The boards can be of the same type (E.g. EK-RA8M1 <--> EK-RA8M1) or different type (E.g. EK-RA8M1 <--> EK-RA6T3). 
       	In case of the same type of boards, the on-board CAN transceivers mounted (if populated) can be used for testing 
	by connecting the CANH, CANL and GND. 
       	
	In case of same type of boards (E.g. EK-RA8D1 <--> EK-RA8D1), (if on-board CAN transceivers not populated) 
	external OM13099 board can be used to test the application by connecting the required wires as described in 
	the readme.txt below - Reference: External Hardware: OM13099 (CAN Transceiver Board).
       	
	For testing the application on boards of different types (EK-RA8D1 <--> EK-RA6T3), which doesn't contain 
	on-board CAN transceivers, OM13099 can be used as externally to test the application by connecting the required 
	wires as described in the optional sections, for the particular boards in the readme.txt.
 
	Additionally, users are required to check the connections and settings as noted in the Note: section. 
       	The CAN settings such as Clock and Bitrate needs to match.
 
       	For EK-RA6M5:
       		Short E33, E34 & E35 inorder to use onboard CAN Transceivers 

       		J33:1 of board 1 to J33:1 of board 2
       		J33:2 of board 1 to J33:2 of board 2
       		J33:3 of board 1 to J33:3 of board 2
       
       		[Optional] Use External Hardware : OM13099 (CAN Transceiver Board)
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
        	J32:1 of board 1 to J32:1 of board 2
        	J32:2 of board 1 to J32:2 of board 2
        	J32:3 of board 1 to J32:3 of board 2

        For MCK-RA4T1, MCK-RA6T3:
        	External Hardware : OM13099 (CAN Transceiver Board) 
        
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
       		J62:1 of board 1 to J62:1 of board 2
       		J62:2 of board 1 to J62:2 of board 2
       		J62:3 of board 1 to J62:3 of board 2
        
       		[Optional] Use External Hardware : OM13099 (CAN Transceiver Board)
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
        	External Hardware : OM13099 (CAN Transceiver Board) 
        
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
        	External Hardware : OM13099 (CAN Transceiver Board) 

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
        	External Hardware : OM13099 (CAN Transceiver Board) 

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
		J33:1 of board 1 to J33:1 of board 2
       		J33:2 of board 1 to J33:2 of board 2
       		J33:3 of board 1 to J33:3 of board 2

4. Hardware Configuration:

	For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |      OFF   |      OFF    |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

	CAUTION: Do not enable SW1-4 and SW1-5 together

Note:
1) In this example project, the same code runs on both boards. The board from which the user initiates the transmission becomes the board1
   and board which receives the data becomes board2.
   
2) LED behavior as below:
   For Boards have 3 LEDs:
   a. LED1 will be turned ON when CAN transmission operation is in progress.
   b. LED2 will be turned ON when CAN transmission operation is successful.
   c. LED3 will be turned ON when an error occurs.
   
   For Boards have 2 LEDs:
   a. LED1 will be turned ON when CAN transmission operation is in progress.
   b. LED2 will be turned ON when CAN transmission operation is successful.
   c. LED1 & LED2 will be turned ON when an error occurs.
      
3) The user is expected to enter data of size not exceeding above 15 bytes.
      
4) For OM13099 (CAN Transceiver Board):
   a. Connect P3 loopback connection on CAN Transceiver Board with jumper cables instead of a db9 serial cable.
   b. Connect jumpers J3, J4, J5 and J6 on OM13099 (CAN Transceiver Board) to establish connection to CAN termination resistors.
      
5) For using different hardware in transmission and reception (Ex. Transmission is EK-RA8D1 board, reception is MCK-RA6T3 board), 
   if EP didn't work, please recheck actual data bitrate information (in hal_data.c) and ensure they are matched between transmission and reception. 
   Recommend to set CANFDCLK is 40MHz/80MHz to have same data bitrate between boards.       

6) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000d44
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
7) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
