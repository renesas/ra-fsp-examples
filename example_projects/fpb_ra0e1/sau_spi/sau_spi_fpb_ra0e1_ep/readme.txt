/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This Example Project demonstrates the basic usage of SAU SPI driver. The project writes commands to the slave digital thermometer module,
	and reads the temperature values using SAU SPI bus configured as master. The temperature values are continuously printed on
	J-Link RTT Viewer. Status information and error messages will be printed on J-link RTT Viewer during the execution of the project.

2. Hardware Requirements:

	Supported RA Boards: FPB-RA0E1

	1 x Renesas RA board.
	1 x USB Type C cable.
	1 x Digital thermometer module (PMOD MAX31723PMB1)
		Link Product: https://www.mouser.vn/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31723PMB1?qs=UmMSjoC1xtH8e742i4OoUA%3D%3D.
		Manufacturer Product Number: MAX31723PMB1#
	6 x Jumper cables (4 - both ends female, and 2 - male to female ).
	1 x Host PC.
	
3. Hardware settings for the project:

        Hardware connections:
	PMOD MAX31723PMB1:
	---------
	Select SPI communication protocol on the PMOD by configuring the following jumper configurations on JP1:
	Connect JP1-3 to JP1-5
	Connect JP1-4 to JP1-6
	Connect JP1-9 to JP1-10

        FPB-RA0E1:     
	---------
	Connect RA board to Host PC by USB Type C cable.

     	RA board                  PMOD MAX31723PMB1
	-------------------------------------------
	P109 (J2-Pin18)	<--------> SS_N  (J1-Pin1)
	P101 (J2-Pin23) <--------> MOSI  (J1-Pin2)
	P100 (J2-Pin24) <--------> MISO  (J1-Pin3)
	P102 (J2-Pin22) <--------> SCK   (J1-Pin4)
	GND  (J3-Pin6)  <--------> GND   (J1-Pin5)
	VCC  (J3-Pin4)	<--------> VDD   (J1-Pin6)		
		
NOTE:
1) The segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004a0c
   b. Keil:	Not Available
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release). 

