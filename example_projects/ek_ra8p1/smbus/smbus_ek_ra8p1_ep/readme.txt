/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/
1. Project Overview:
	This example project demonstrates the basic usage of I2C communication conforming to the SMBus Specification
	(version 2.0) on Renesas RA MCUs using the Renesas Flexible Software Package (FSP). The project involves
	writing commands to the MAX31875 sensor and reading the resulting temperature values. The SMBus transmission
	time is monitored by the GPT module to ensure the MCU acting as a master device complies with the SMBus timeout
	standard. The Packet Error Check (PEC) byte is calculated in CRC-8 format at both transmission and reception.
	These temperature values are continuously printed to a terminal. Additionally, status information and error
	messages are displayed on the host PC. The error LED will turn on if an error occurs.

	Note:
	- To display information, users can choose between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART)
	  with J-Link OB VCOM. It is important to note that the user should only operate a single terminal application 
	  at a time to avoid conflicts or data inconsistencies.
	- For instructions on how to switch between these options, please refer to the "Verifying Operation" section in
	  smbus_notes.md file.
	- By default, EP information is printed to the host PC using the Serial Terminal for boards that support J-Link
	  OB VCOM. For the RA boards that do not support J-Link OB VCOM, EP uses the SEGGER J-Link RTT Viewer by default
	  instead.
	- RA boards supported for J-Link OB VCOM: EK-RA8M1, EK-RA2A2, EK-RA4E2, EK-RA6E2, EK-RA8D1, FPB-RA2E3, MCK-RA4T1,
 	  MCK-RA6T3, MCK-RA8T1, FPB-RA8E1, EK-RA8P1.

2. Hardware Requirements:
	Supported RA boards: EK-RA8M1, CK-RA6M5, EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M1,
			     EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4,
			     EK-RA6M5, EK-RA8D1, FPB-RA2E3, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3,
			     MCK-RA8T1, RSSK-RA6T1, FPB-RA8E1, EK-RA8P1.
	1 x Renesas RA board.
	1 x USB cable for programing and debugging.
		- 1 x Type-C USB cable: CK-RA6M5, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1 FPB-RA8E1, EK-RA8P1
		- 1 x Mini USB cable: RSSK-RA6T1
		- 1 x Micro USB cable: All other boards
	1 x Evaluation Kit for the MAX31875 (https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31875EVKIT?qs=BZBei1rCqCDTW6E3jJcKjw%3D%3D&_gl=1)
	    Manufacturer Product Number: MAX31875EVKIT#
	1 x Host PC.

	- RA boards that do not require external pull-up resistors: 
	  For EK-RA8M1, CK-RA6M5, EK-RA2E1, EK-RA2L1, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, FPB-RA4E1,
 	      FPB-RA6E1, FPB-RA8E1, EK-RA8P1:
	  	+ 4 x Jumper wires (4 - both ends female).

	  For EK-RA2A2, FPB-RA2E3, MCK-RA4T1, MCK-RA6T3, MCK-RA8T1: 
		+ No additional jumper wires required.	

    	- RA boards that require external resistors for pull-up: EK-RA2A1, EK-RA2E2, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4W1,
	  EK-RA6E2, EK-RA6M1, EK-RA6M2, MCK-RA6T2, RSSK-RA6T1.
		2 x Resistors (3.3K ohm or 4.7K ohm).
		1 x Breadboard.
		7 x Jumper wires.
	    		+ 6 - male to female, 1 - both ends female.
	    		+ 4 - male to female, 3 - both ends male (For RSSK-RA6T1).

3. Hardware Connections:

	For EK-RA8M1:     
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		      MAX31875PMB1
		-----------------------------------------------
		SCL1 P512 (J56:37) <--------> SCL (J1:3)
		SDA1 P511 (J57:38) <--------> SDA (J1:4)
		GND  	  (J57:49) <--------> GND (J1:5)
		VCC  	  (J56:40) <--------> VDD (J1:6)

	For CK-RA6M5:
	---------
		Connect RA board to Host PC by Type-C USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL1 P512 (J3:10) <--------> SCL (J1:3)
		SDA1 P511 (J3:9)  <--------> SDA (J1:4)
		GND  	  (J6:6)  <--------> GND (J1:5)
		VCC  	  (J6:4)  <--------> VDD (J1:6)

	For EK-RA2A1:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL1 P109 (J1:37) <--------> SCL (J1:3)
		SDA1 P110 (J1:35) <--------> SDA (J1:4)
		GND 	  (J1:17) <--------> GND (J1:5)
		VCC  	  (J1:15) <--------> VDD (J1:6)

	For EK-RA2A2, FPB-RA2E3:
	-------------------
		Connect RA board to Host PC by Micro USB cable.
		Connect MAX31875PMB1 (JU4:1) to RA board via the PMOD2 (PMOD2:1)

	For EK-RA2E1:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL0 P408 (J2:08) <--------> SCL (J1:3)
		SDA0 P407 (J2:9)  <--------> SDA (J1:4)
		GND  	  (J2:40) <--------> GND (J1:5)
		VCC  	  (J2:1)  <--------> VDD (J1:6)

	For EK-RA2E2:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL0 P400 (J1:2)  <--------> SCL (J1:3)
		SDA0 P401 (J1:3)  <--------> SDA (J1:4)
		GND  	  (J1:13) <--------> GND (J1:5)
		VCC  	  (J1:1)  <--------> VDD (J1:6)

	For EK-RA2L1:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL0 P408 (J1:10) <--------> SCL (J1:3)
		SDA0 P407 (J1:11) <--------> SDA (J1:4)
		GND  	  (J1:40) <--------> GND (J1:5)
		VCC  	  (J1:1)  <--------> VDD (J1:6)

	For EK-RA4E2, EK-RA6E2:
	-------------------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL0 P205 (J2:16) <--------> SCL (J1:3)
		SDA0 P206 (J2:15) <--------> SDA (J1:4)
		GND  	  (J2:40) <--------> GND (J1:5)
		VCC  	  (J2:1)  <--------> VDD (J1:6)

	For EK-RA4M1:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL0 P400 (J2:37) <--------> SCL (J1:3)
		SDA0 P401 (J2:8)  <--------> SDA (J1:4)
		GND  	  (J2:12) <--------> GND (J1:5)
		VCC  	  (J2:10) <--------> VDD (J1:6)

	For EK-RA4M2:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL0 P400 (J1:04) <--------> SCL (J1:3)
		SDA0 P401 (J1:3)  <--------> SDA (J1:4)
		GND  	  (J1:27) <--------> GND (J1:5)
		VCC  	  (J1:1)  <--------> VDD (J1:6)

	For EK-RA4M3, EK-RA6M4:
	-------------------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL1 P512 (J2:37) <--------> SCL (J1:3)
		SDA1 P511 (J2:38) <--------> SDA (J1:4)
		GND  	  (J2:39) <--------> GND (J1:5)
		VCC  	  (J2:40) <--------> VDD (J1:6)

	For EK-RA4W1:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		      MAX31875PMB1
		-----------------------------------------------
		SCL1 P205 (CN7:8)  <--------> SCL (J1:3)
		SDA1 P206 (CN7:7)  <--------> SDA (J1:4)
		GND  	  (CN7:19) <--------> GND (J1:5)
		VCC  	  (CN7:18) <--------> VDD (J1:6)

	For EK-RA6M1:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL0 P400 (J1:1)  <--------> SCL (J1:3)
		SDA0 P401 (J1:3)  <--------> SDA (J1:4)
		GND  	  (J1:17) <--------> GND (J1:5)
		VCC  	  (J1:15) <--------> VDD (J1:6)

	For EK-RA6M2:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL0 P400 (J4:13) <--------> SCL (J1:3)
		SDA0 P401 (J4:11) <--------> SDA (J1:4)
		GND  	  (J4:36) <--------> GND (J1:5)
		VCC  	  (J4:37) <--------> VDD (J1:6)

	For EK-RA6M3, EK-RA6M3G:
	-------------------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL2 P512 (J1:5)  <--------> SCL (J1:3)
		SDA2 P511 (J1:21) <--------> SDA (J1:4)
		GND  	  (J1:30) <--------> GND (J1:5)
		VCC  	  (J1:36) <--------> VDD (J1:6)

	For EK-RA6M5:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL1 P512 (J4:38) <--------> SCL (J1:3)
		SDA1 P511 (J4:39) <--------> SDA (J1:4)
		GND  	  (J4:40) <--------> GND (J1:5)
		VCC  	  (J4:1)  <--------> VDD (J1:6)

	For EK-RA8D1:
	---------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		      MAX31875PMB1
		-----------------------------------------------
		SCL1 P512 (J57:4)  <--------> SCL (J1:3)
		SDA1 P511 (J57:2)  <--------> SDA (J1:4)
		GND  	  (J57:39) <--------> GND (J1:5)
		VCC  	  (J57:5)  <--------> VDD (J1:6)

	For FPB-RA4E1, FPB-RA6E1:
	---------------------
		Connect RA board to Host PC by Micro USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		     MAX31875PMB1
		-----------------------------------------------
		SCL0 P400 (J3:1)  <--------> SCL (J1:3)
		SDA0 P401 (J3:2)  <--------> SDA (J1:4)
		GND  	  (J3:12) <--------> GND (J1:5)
		VCC  	  (J3:15) <--------> VDD (J1:6)

	For FPB-RA8E1:
	---------
		Connect RA board to Host PC by Type-C USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		      MAX31875PMB1
		-----------------------------------------------
		SCL1 P512 (J24:10) <--------> SCL (J1:3)
		SDA1 P511 (J24:9)  <--------> SDA (J1:4)
		GND  	  (J18:6)  <--------> GND (J1:5)
		VCC  	  (J18:4)  <--------> VDD (J1:6)

	For MCK-RA4T1, MCK-RA6T3:
	---------------------
		Connect RA board to Host PC by Type-C USB cable.
		Connect MAX31875PMB1 (JU4:1) to RA board via the PMOD1 Port (PMOD1:1)
		Jumper J10 short pin2-3 to select SCL Pin
		Jumper J11 short pin1-2 to select SDA Pin

	For MCK-RA6T2:
	---------
		Connect RA board to Host PC by Type-C USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		      MAX31875PMB1
		-----------------------------------------------
		SCL0 PB06 (CN4:33) <--------> SCL (J1:3)
		SDA0 PB07 (CN4:31) <--------> SDA (J1:4)
		GND  	  (CN4:25) <--------> GND (J1:5)
		VCC  	  (CN4:23) <--------> VDD (J1:6)

	For MCK-RA8T1:
	---------
		Connect RA board to Host PC by Type-C USB cable.
		Connect MAX31875PMB1 (JU4:1) to RA board via the PMOD6A (PMOD6A:1)

	For RSSK-RA6T1:
	-----------
		Connect RA board to Host PC by Mini USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		      MAX31875PMB1
		-----------------------------------------------
		SCL1 P101 (CN8:2)  <--------> SCL (J1:3)
		SDA1 P100 (CN8:3)  <--------> SDA (J1:4)
		GND  	  (CN8:5)  <--------> GND (J1:5)
		VCC  	  (CN8:6)  <--------> VDD (J1:6)

	For EK-RA8P1:
	-----------
		Connect RA board to Host PC by Type-C USB cable.
		Connect RA board to MAX31875PMB1 as below:
		RA board		      MAX31875PMB1
		-----------------------------------------------
		SCL1 P512 (J1:4)  <--------> SCL (J1:3)
		SDA1 P511 (J1:2)  <--------> SDA (J1:4)
		GND  	  (J1:40) <--------> GND (J1:5)
		VCC  	  (J1:5)  <--------> VDD (J1:6)

Note:
    	For using the Serial Terminal (UART):
      	1) The serial terminal setting parameters are as follows:
      	   Select the COM port provided by the J-Link on-board.
      	   Baud rate: 115200 bps
      	   Data length: 8-bits  
      	   Parity: none
      	   Stop bit: 1-bit
      	   Flow control: none

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22001050
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
