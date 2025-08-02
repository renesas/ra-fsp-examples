/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of the SCI Smart Card Interface Device on Renesas RA MCUs
	based on Renesas FSP using SCI_SMCI module without RTOS. An ISO 7816-3 Smart Card is connected to the RA board,
	which is configured as SCI Smart Card Interface (please refer to the Table of Smart Card Connections). The
	application sets and configures baud rate for the smart card. This project also demonstrates the ATR proceeds
	in five steps: Initial character TS; Format bytes T0; Interface bytes TAi, TBi, TCi, TDi and variable number;
	Historical bytes, and the check byte TCK (optional) for SIM Card and Debit Card.

	For the SIM Card, the application proceeds with the activation process including Answer To Reset (ATR) stage
	and negotiation stage will show (completed PPS exchange).

	For the Debit Card, the user can follow the menu to do the test. The user can "Enter" value from Menu, and all
	information of the chip in the Smart Card connected to the RA board will be printed in the RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.0.0
	e2 studio: Version 2025-04.1
	SEGGER J-Link RTT Viewer: Version 8.44a
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x SIM Card (the standard SIM, Micro SIM or Nano SIM and) with wires connection soldered are ready on SIM or socket.
	1 x Debit Card and ISO 7816-3 Card (Should be used ISO 7816-3 AT88SC0104C) if the user would like to test for both.
	1 x Resistor (from 1k to 4.7k ohms).
	9 x Jumper wires.

4. Hardware Connections:
	For EK-RA6M5:
		Connect a Micro USB cable to PC and the other end to the debug port (J10) on the RA board.
		SIM or Debit Card connect to EK-RA6M5 pins:
				SIM or Debit Card	        EK-RA6M5 
			1. 	Pin 1 VCC		=>	P804
			2. 	Pin 2 RESET 		=>	P803
			3. 	Pin 3 SCK		=>	P612
			4.	Pin 4 GND		=>	GND
			5.	Pin 5 DETECT		=>	Pin P708 = MCU's Detect Pin (Open)
			6.	pin I/O			=>	P613 and P614
	
	For EK-RA4L1:
		Connect a Type-C USB cable to PC and the other end to the debug port (J10) on the RA board.
		SIM or Debit Card connect to EK-RA4L1 pins:
				SIM or Debit Card		EK-RA4L1
			1. 	Pin 1 VCC		=>	P700 (J2:36)
			2. 	Pin 2 RESET 		=>	P205 (J2:3)
			3. 	Pin 3 SCK		=>	P303 (J2:9)
			4.	Pin 4 GND		=>	GND
			5.	Pin 5 DETECT		=>	P105 (J3:12) = MCU's Detect Pin (Open)
			6.	Pin I/O			=>	P301 (J4:8) and P302 (J4:9) 

	For EK-RA2L2:
		Connect a Micro USB cable to PC and the other end to the debug port (J10) on the RA board.
		SIM or Debit Card connect to EK-RA2L2 pins:
				SIM or Debit Card		EK-RA2L2
			1. 	Pin 1 VCC		=>	P111 (J2:29)
			2. 	Pin 2 RESET 		=>	P104 (J2:35)
			3. 	Pin 3 SCK		=>	P102 (J2:37)
			4.	Pin 4 GND		=>	GND
			5.	Pin 5 DETECT		=>	P206 (J2:15) = MCU's Detect Pin (Open)
			6.	Pin I/O			=>	P410 (J2:6) and P411 (J2:5) 

	For FPB-RA2T1:
		Connect a Type-C USB cable to PC and the other end to the debug port (CN6) on the RA board.
		SIM or Debit Card connect to FPB-RA2T1 pins:
				SIM or Debit Card		FPB-RA2T1
			1. 	Pin 1 VCC		=>	P915 (J3:13)
			2. 	Pin 2 RESET 		=>	P914 (J3:14)
			3. 	Pin 3 SCK		=>	P102 (J4:10)
			4.	Pin 4 GND		=>	GND
			5.	Pin 5 DETECT		=>	P015 (J4:14) = MCU's Detect Pin (Open)
			6.	Pin I/O			=>	P100 (J4:12) and P101 (J4:11)

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		The user must close E46, E65 to use P301, P302 for SCI.
		Connect a Type-C USB cable to PC and the other end to the debug port (J10) on the RA board.
		SIM or Debit Card connect to EK-RA4C1 pins:
				SIM or Debit Card		EK-RA4C1
			1. 	Pin 1 VCC		=>	P601 (J2:32)
			2. 	Pin 2 RESET 		=>	P205 (J2:3)
			3. 	Pin 3 SCK		=>	P303 (J2:9)
			4.	Pin 4 GND		=>	GND
			5.	Pin 5 DETECT		=>	P513 (J3:12) = MCU's Detect Pin (Open)
			6.	Pin I/O			=>	P301 (J2:26) and P302 (J2:24) 	

Note:
1) Pins for Pin I/O must be tied together, pulled up to VCC via a resistor from 1k to 4.7k Ohms (using 1k Ohms for EK-RA2L2),
   and connected to the DATA line of the connected card.

2) If a Smart Card Socket is unavailable:
   a. Pin 5 (DETECT) of the card does not need to connect to the MCU's DETECT Pin. However, the MCU's Detect Pin must be
      pulled high to 3.3V (high = Smart Card inserted).
   b. To test option 3, the user must reconnect the MCU's DETECT Pin from 3.3V to GND (low = Smart Card removed). 
      Then reconnect this pin from GND to 3.3V (high = Smart Card inserted) so the MCU will detect the card.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000ca4
   b. Keil:     Not Available
   c. IAR:      Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).