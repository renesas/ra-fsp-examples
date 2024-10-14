/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of SCI Smart Card Interface Device on Renesas RA MCUs based on Renesas
	FSP using SCI_SMCI module without RTOS. ISO 7816-3 Smart Card will be connected to the RA board which is configured as
	SCI Smart Card Interface (Please looks at the Table of Smart Card Connections). The application sets and configure baud 
	rate for the smart. This project also demonstrates the ATR proceeds in five steps: Initial character TS; format bytes T0;
	Interface bytes TAi, TBi, TCi, TDi and vaiable number; Historical bytes, and the check byte TCK (optional) for SIM Card 
	and Debit Card.	For the SIM Card, the application proceeds the activation process including Answer To Reset stage and 
	negotiation stage will show(Completed PPS exchange). For the Debit Card, user can follow the the Menu to do the test. User
	can "Enter" value from Menu and all information of the chip in the card the Smart Card connected to the RA board will be 
	printed it the RTTViewer...

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.5.0
	e2 studio: Version 2024-07
	SEGGER J-Link RTT Viewer: Version 7.98b
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	An internal pack file of SCI SMCI driver (https://renesasgroup-my.sharepoint.com/:u:/g/personal/cheng-ping_chan_jc_renesas_com/EfBMYUOfm3lOo4JMbGgs5ZMBo7t5DvDNPs6u_-mrSdTToA?e=DpCCWx)

3. Hardware Requirements:

   Hardware : 
	1) 1x EK-RA6M5 board 
	2) 1x Micro USB cable for programming and debugging
	3) 1x SIM Card (the standard SIM, Micro SIM or Nano SIM and) with wires connection soldered are ready on SIM or socket.
	4) 1x Debit Card and 1x of ISO 7816-3 Card (Should be used ISO 7816-3 AT88SC0104C) If user would like to test for both.	 

4. Hardware settings for the project:
   EK-RA6M5 connection:
	1. Connect Micro USB cable to PC and the other end to J10 debuger on RA board.
	
   SIM or Debit Card connect to EK-RA6M5 pins:
	SIM or Debit Card			EK-RA6M5 
	1. 	pin 1 VCC = 3.3V	=>	pin VCC  = 3.3V
	2. 	pin 2 RESET 		=>	pin P803 = Reset
	3. 	pin 3 SCK		=>	pin P612 = Clock
	4.	pin 4 GND		=>	pin GND	 = Ground
	5.	pin 5 DETECT		=>	pin P708 = Detect (Open)
	6.	pin I/O			=>	pin P613 and P614 
	
	
Note:
1. Pin P613 and P614 tired together and needs 3.3V pull up using 4.7K.
2. User don't need to connect pin 5 DETECT (SIM or Debit Card) to pin P708 if user didn't have SIM socket. But User needs to connect pin P708 to 3.3V (RA6M5 MCU detect P708 hign = SIM Card inserted)
3. To test the option 3 user needs to reconnect pin P708 to GND (Pin P708 goes low = Card removed) before Enter "3". Disconnect pin P708 from GND (P708 goes high) then MCU will detect Card inserted. 
4. Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200004ac
