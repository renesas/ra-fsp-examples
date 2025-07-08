/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
 
    This example project demonstrates the typical use of the I3C Driver on Renesas RA2E2/RA4E2/RA6E2/RA4T1 MCUs based on
    Renesas FSP.The I3C master on EK-RA2E2/EK-RA4E2/EK-RA6E2/MCK-RA4T1 demonstrates operations associated with an I3C slave
    running on another EK-RA2E2/EK-RA4E2/EK-RA6E2/MCK-RA4T1 board.Once initialization is successful, I3C Master device assigns
    slave address to I3C slave device through DAA (dynamic address assignment) using I3C common command code (CCC). Once
    Dynamic Address Assignment is completed, the Master EP performs write/read operations, and displays slave device
    information based on user input. If the on-board switch is pressed on the slave side, it will initiate an IBI transfer
    request. Error and info messages will be printed on Jlink RTTViewer.

2. Hardware Requirement:

	i. 2x Renesas EK-RA2E2/EK-RA4E2/EK-RA6E2/MCK-RA4T1 kit.
       ii. 2x USB A to USB Micro B Cable or Type C USB Cable (For MCK-RA4T1).  
      iii. 5x Jumper wires to link EK-RA2E2/EK-RA4E2/EK-RA6E2/MCK-RA4T1 boards.
       iv. 1x Pull up resistor of value 3.9 or 4.7k ohms.
      
3. Hardware Connections:
 	Supported Board EK-RA2E2/EK-RA4E2/EK-RA6E2/MCK-RA4T1:		
	* For board EK-RA2E2/EK-RA4E2/EK-RA6E2:		
		+ Connect micro-USB cable between J10 on I3C master board EK-RA2E2/EK-RA4E2/EK-RA6E2 and PC.
      		+ Connect micro-USB cable between J10 on I3C slave board EK-RA2E2/EK-RA4E2/EK-RA6E2 and PC.
	* For board MCK-RA4T1:
		+ Connect Type C USB cable between CN9 on I3C master board MCK-RA4T1 and PC.
        	+ Connect Type C USB cable between CN9 on I3C slave board MCK-RA4T1 and PC.
        * External 3V3 pull-up using 3.9 or 4.7k ohms resistor should be connected on SDA line.
        * Three jumper wires are required to establish loop back connection along I3C lines across the board with pins as mentioned below.
        
	EK-RA2E2:

        Master Board              Slave Board
        -------------             ------------
	SDA0 Pin P401   ---->     SDA0 Pin 401
        SCL0 Pin P400   ---->     SCL0 Pin 400
        Common GND

	EK-RA4E2/EK-RA6E2:

        Master Board              Slave Board
        -------------             ------------
	SDA0 Pin P101   ---->     SDA0 Pin 101
        SCL0 Pin P100   ---->     SCL0 Pin 100
        Common GND

	MCK-RA4T1:

        Master Board              Slave Board
        -------------             ------------
	SDA0 Pin P101   ---->     SDA0 Pin 101 (TP-16)
        SCL0 Pin P100   ---->     SCL0 Pin 100 (TP-17)
        Common GND

Note: 
1. On slave side user can perform IBI transfer request using onboard Push button(S2).
   For MCK-RA4T1: The user needs to use Toggle switch(S1) instead.
2. readme is common for both i3c_master and i3c_slave EP.
﻿
3) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001464
   b. Keil:	Not Available
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).