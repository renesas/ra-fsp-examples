/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the basic usage of TAU driver on Renesas RA MCUs based on Renesas FSP. 
	It consists of 7 independent operations coordinated together to create 3 sequences. They include setting the period
	for LED on and off times, counting external events, and measuring low-level pulse width. The user will input the 
	required parameters for each sequence to configure the corresponding independent channel operation function of TAU.
	The EP information and error messages will be printed to the Host PC.

	NOTE1: To display information, users can select between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART) with J-Link OB VCOM. 
	       By default, EP information is printed to the host PC via the Serial Terminal.
	       To utilize the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please follow the instructions in tau_notes.md file.
	
2. Hardware Requirements:

	Supported RA Boards: FPB-RA0E1

	1 x Renesas RA board.
	1 x USB Type C cable.
	5 x Jumper cable (both ends female).
	1 x Host PC.
	
3. Hardware connections:

    FPB-RA0E1:     
	---------
	1. Connect RA board to Host PC by USB Type C cable.
  	2. For using the Serial Terminal (UART):     
       		The soldering bridge E4 must be closed.
		Populate R10 with a resistor ranging from 0 to 150 ohms.
	3. Pin connection:
		P008-LED1 (J2-Pin 30) <--------> P112-TI03 (J2-Pin 20)
		P110-T001 (J2-Pin 19) <--------> P213-TI00 (J1-Pin 5)
		P212-TO00 (J1-Pin 6)  <--------> P100-TI04 (J2-Pin 24)
		P009-LED2 (J2-Pin 29) <--------> P103-TI05 (J2-Pin 21)
		P102-T006 (J2-Pin 22) <--------> P101-TI07 (J2-Pin 23)

NOTE2:
    For using the Serial Terminal (UART):
      1) To echo back what was typed in Tera Term, the user needs to enable it through [Setup] -> [Terminal...] -> Check [Local echo].
      2) The serial terminal setting parameters are as follows:
      	Select the COM port provided by the J-Link on-board.
      	Baud rate: 115200 bps
      	Data length: 8-bits  
      	Parity: none
      	Stop bit: 1-bit
      	Flow control: none