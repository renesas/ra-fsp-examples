/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
 
    This example project demonstrates the typical use of the I3C Driver on Renesas RA MCUs based on Renesas FSP.
    The I3C master on RA board demonstrates operations associated with an I3C slave running on another RA board.
	Once initialization is successful, I3C Master device assigns slave address to I3C slave device through DAA
	(dynamic address assignment) using I3C common command code(CCC). Once Dynamic Address Assignment is completed,y
	the Master EP performs write/read operations, and displays slave device information based on user input. If the
	on-board switch is pressed on the slave side, it will initiate an IBI transfer request. Error and info messages
	will be printed on J-Link RTT Viewer.
	
	Note: EK-RA8D1 supports I3C HDR-DDR mode. The Master EP can performs write/read operations both in the SDR mode and HDR-DDR mode.

2. Hardware Requirement:

	i. 2x Renesas RA boards.
       ii. 2x USB A to USB Micro B Cable or Type C USB Cable (For MCK-RA4T1, MCK-RA6T3). 
      iii. 5x Jumper wires to link RA boards.
       iv. 1x Pull up resistor of value 1k, 3.9k or 4.7k ohms.
      
3. Hardware Connections:
	Supported Boards: EK-RA2E2/EK-RA4E2/EK-RA6E2/MCK-RA4T1/MCK-RA6T3/EK-RA8M1/EK-RA8D1:

	* For EK-RA2E2/EK-RA4E2/EK-RA6E2/EK-RA8M1/EK-RA8D1:		
	   * Connect micro-USB cable between J10 on I3C master RA board and PC.
	   * Connect micro-USB cable between J10 on I3C slave RA board and PC.
	* For MCK-RA4T1/MCK-RA6T3:
	   * Connect Type C USB cable between CN9 on I3C master RA board and PC.
	   * Connect Type C USB cable between CN9 on I3C slave RA board and PC.
	   
	* External 3V3 pull-up using 1k, 3.9k or 4.7k ohms resistor should be connected on SDA line.
	   * Change external 3V3 pull-up using 1K ohms resistor for EK-RA8M1, EK-RA8D1.
	* The jumper wires are required to establish loop back connection along I3C lines across the board with pins as mentioned below.

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

	MCK-RA4T1/MCK-RA6T3:

		Master Board              Slave Board
		-------------             ------------
		SDA0 Pin P101   ---->     SDA0 Pin 101 (TP-16)
		SCL0 Pin P100   ---->     SCL0 Pin 100 (TP-17)
		Common GND

	EK-RA8M1:

		Master Board              Slave Board
		-------------             ------------
		SDA0 J27:2   	---->     SDA0 J27:2
		SCL0 J27:1   	---->	  SCL0 J27:1
		Common GND
		
	EK-RA8D1:

		Master Board              Slave Board
		-------------             ------------
		SDA0 P401   	---->     SDA0 P401
		SCL0 P400   	---->	  SCL0 P400
		Common GND

4. Hardware Configurations:

- For EK-RA8M1: Install jumper J61 to enable I3C

- For EK-RA8D1: Set the configuration switches (SW1) as below to avoid potential failures.
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
	|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     OFF    |     OFF     |     OFF   |
	+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

﻿
Note: 
1. On slave side user can perform IBI transfer request using onboard Push button(S2).
   For MCK-RA4T1/MCK-RA6T3: The user needs to use Toggle switch(S1) instead.
2. Readme is common for both i3c_master and i3c_slave EP.

4) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22001078
   b. Keil:	Not Available
   c. IAR:      Not Available
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
