/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/
1. Project Overview:
	This example project demonstrates the basic usage of I2C communication conforming to the SMBus Specification (version 2.0) on Renesas
	RA MCUs using the Renesas Flexible Software Package (FSP). The project involves writing commands to the MAX31875 sensor and reading
	the resulting temperature values. The SMBus transmission time is monitored by the GPT module to ensure the MCU acting as a master device
	complies with the SMBus timeout standard. The Packet Error Check (PEC) byte is calculated in CRC-8 format at both transmission and reception.
	These temperature values are continuously printed to a terminal. Additionally, status information and error messages are displayed on the host PC.
	The error LED will turn on if an error occurs.

	NOTE1:
	- To display information, users can choose between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART) with J-Link OB VCOM.
	  It is important to note that the user should only operate a single terminal application at a time to avoid conflicts or data inconsistencies. 
	- For instructions on how to switch between these options, please refer to the "Verifying Operation" section in smbus_notes.md file.
	- By default, EP information is printed to the host PC using the Serial Terminal for boards that support J-Link OB VCOM.
	For the RA boards that do not support J-Link OB VCOM, EP uses the SEGGER J-Link RTT Viewer by default instead.
	- RA boards supported for J-Link OB VCOM: EK-RA8M1, EK-RA2A2, EK-RA4E2, EK-RA6E2, EK-RA8D1, FPB-RA2E3, MCK-RA4T1, MCK-RA6T3, MCK-RA8T1, FPB-RA8E1.

2. Hardware Requirements:

	Supported RA Boards: EK-RA8M1, CK-RA6M5, EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1,
			     EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, FPB-RA2E3, FPB-RA4E1, FPB-RA6E1,
			     MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, RSSK-RA6T1, FPB-RA8E1.
	1 x Host PC.
	1 x Renesas RA board.
	1 x Micro USB cable.
	1 x Evaluation Kit for the MAX31875
	    Link Product: https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31875EVKIT?qs=BZBei1rCqCDTW6E3jJcKjw%3D%3D&_gl=1
	    Manufacturer Product Number: MAX31875EVKIT#

    - RA boards that do not require external resistors for pull-up:
    + For EK-RA8M1, CK-RA6M5, EK-RA2E1, EK-RA2L1, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1:
	4 x Jumper cables (4 - both ends female).
    + For EK-RA2A2, FPB-RA2E3, MCK-RA4T1, MCK-RA6T3, MCK-RA8T1:
	No additional jumper cables required.

    - RA boards that require external resistors for pull-up: EK-RA2A1, EK-RA2E2, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4W1, EK-RA6E2, EK-RA6M1,
				 			     EK-RA6M2, MCK-RA6T2, RSSK-RA6T1.
	2 x Resistors (3.3K ohm or 4.7K ohm).
	1 x breadboard.
	7 x Jumper cables
	    + 6 - male to female, 1 - both ends female.
	    + 4 - male to female, 3 - both ends male ( For RSSK-RA6T1).

3. Hardware Connections:

    EK-RA8M1:     
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
     	RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P512 (J56-Pin37) <--------> SCL  (J1-Pin3)
	SDA1 P511 (J57-Pin38) <--------> SDA  (J1-Pin4)
	GND  	  (J57-Pin49) <--------> GND  (J1-Pin5)
	VCC  	  (J56-Pin40) <--------> VDD  (J1-Pin6)

    CK-RA6M5:
    ---------
	Connect RA board to Host PC by TypeC USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P512 (J3-Pin10) <--------> SCL  (J1-Pin3)
	SDA1 P511 (J3-Pin9)  <--------> SDA  (J1-Pin4)
	GND  	  (J6-Pin6)  <--------> GND  (J1-Pin5)
	VCC  	  (J6-Pin4)  <--------> VDD  (J1-Pin6)

    EK-RA2A1:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P109 (J1-Pin37) <--------> SCL  (J1-Pin3)
	SDA1 P110 (J1-Pin35) <--------> SDA  (J1-Pin4)
	GND  	  (J1-Pin17) <--------> GND  (J1-Pin5)
	VCC  	  (J1-Pin15) <--------> VDD  (J1-Pin6)

    EK-RA2A2, FPB-RA2E3:
    -------------------
	Connect RA board to Host PC by Micro USB cable.
	Connect MAX31875PMB1 (JU4:1) to RA board via the PMOD2 Port (PMOD2:1)

    EK-RA2E1:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 P408 (J2-Pin08) <--------> SCL  (J1-Pin3)
	SDA0 P407 (J2-Pin09) <--------> SDA  (J1-Pin4)
	GND  	  (J2-Pin40) <--------> GND  (J1-Pin5)
	VCC  	  (J2-Pin01) <--------> VDD  (J1-Pin6)

    EK-RA2E2:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 P400 (J1-Pin02) <--------> SCL  (J1-Pin3)
	SDA0 P401 (J1-Pin03) <--------> SDA  (J1-Pin4)
	GND  	  (J1-Pin13) <--------> GND  (J1-Pin5)
	VCC  	  (J1-Pin01) <--------> VDD  (J1-Pin6)

    EK-RA2L1:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 P408 (J1-Pin10) <--------> SCL  (J1-Pin3)
	SDA0 P407 (J1-Pin11) <--------> SDA  (J1-Pin4)
	GND  	  (J1-Pin40) <--------> GND  (J1-Pin5)
	VCC  	  (J1-Pin01) <--------> VDD  (J1-Pin6)

    EK-RA4E2, EK-RA6E2:
    -------------------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 P205 (J2-Pin16) <--------> SCL  (J1-Pin3)
	SDA0 P206 (J2-Pin15) <--------> SDA  (J1-Pin4)
	GND  	  (J2-Pin40) <--------> GND  (J1-Pin5)
	VCC  	  (J2-Pin01) <--------> VDD  (J1-Pin6)

    EK-RA4M1:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 P400 (J2-Pin37) <--------> SCL  (J1-Pin3)
	SDA0 P401 (J2-Pin08) <--------> SDA  (J1-Pin4)
	GND  	  (J2-Pin12) <--------> GND  (J1-Pin5)
	VCC  	  (J2-Pin10) <--------> VDD  (J1-Pin6)

    EK-RA4M2:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 P400 (J1-Pin04) <--------> SCL  (J1-Pin3)
	SDA0 P401 (J1-Pin03) <--------> SDA  (J1-Pin4)
	GND  	  (J1-Pin27) <--------> GND  (J1-Pin5)
	VCC  	  (J1-Pin01) <--------> VDD  (J1-Pin6)

    EK-RA4M3, EK-RA6M4:
    -------------------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P512 (J2-Pin37) <--------> SCL  (J1-Pin3)
	SDA1 P511 (J2-Pin38) <--------> SDA  (J1-Pin4)
	GND  	  (J2-Pin39) <--------> GND  (J1-Pin5)
	VCC  	  (J2-Pin40) <--------> VDD  (J1-Pin6)

    EK-RA4W1:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P205 (CN7-Pin08) <--------> SCL  (J1-Pin3)
	SDA1 P206 (CN7-Pin07) <--------> SDA  (J1-Pin4)
	GND  	  (CN7-Pin19) <--------> GND  (J1-Pin5)
	VCC  	  (CN7-Pin18) <--------> VDD  (J1-Pin6)

    EK-RA6M1:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 P400 (J1-Pin01) <--------> SCL  (J1-Pin3)
	SDA0 P401 (J1-Pin03) <--------> SDA  (J1-Pin4)
	GND  	  (J1-Pin17) <--------> GND  (J1-Pin5)
	VCC  	  (J1-Pin15) <--------> VDD  (J1-Pin6)

    EK-RA6M2:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 P400 (J4-Pin13) <--------> SCL  (J1-Pin3)
	SDA0 P401 (J4-Pin11) <--------> SDA  (J1-Pin4)
	GND  	  (J4-Pin36) <--------> GND  (J1-Pin5)
	VCC  	  (J4-Pin37) <--------> VDD  (J1-Pin6)

    EK-RA6M3, EK-RA6M3G:
    -------------------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL2 P512 (J1-Pin05) <--------> SCL  (J1-Pin3)
	SDA2 P511 (J1-Pin21) <--------> SDA  (J1-Pin4)
	GND  	  (J1-Pin30) <--------> GND  (J1-Pin5)
	VCC  	  (J1-Pin36) <--------> VDD  (J1-Pin6)

    EK-RA6M5:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P512 (J4-Pin38) <--------> SCL  (J1-Pin3)
	SDA1 P511 (J4-Pin39) <--------> SDA  (J1-Pin4)
	GND  	  (J4-Pin40) <--------> GND  (J1-Pin5)
	VCC  	  (J4-Pin01) <--------> VDD  (J1-Pin6)

    EK-RA8D1:
    ---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P512 (J57-Pin04) <--------> SCL  (J1-Pin3)
	SDA1 P511 (J57-Pin02) <--------> SDA  (J1-Pin4)
	GND  	  (J57-Pin39) <--------> GND  (J1-Pin5)
	VCC  	  (J57-Pin05) <--------> VDD  (J1-Pin6)

    FPB-RA4E1, FPB-RA6E1:
    ---------------------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 P400 (J3-Pin01) <--------> SCL  (J1-Pin3)
	SDA0 P401 (J3-Pin02) <--------> SDA  (J1-Pin4)
	GND  	  (J3-Pin12) <--------> GND  (J1-Pin5)
	VCC  	  (J3-Pin15) <--------> VDD  (J1-Pin6)

    FPB-RA8E1:
    ---------
	Connect RA board to Host PC by TypeC USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P512 (J24-Pin10) <--------> SCL  (J1-Pin3)
	SDA1 P511 (J24-Pin9)  <--------> SDA  (J1-Pin4)
	GND  	  (J18-Pin6)  <--------> GND  (J1-Pin5)
	VCC  	  (J18-Pin4)  <--------> VDD  (J1-Pin6)

    MCK-RA4T1, MCK-RA6T3:
    ---------------------
	Connect RA board to Host PC by Type C USB cable.
	Connect MAX31875PMB1 (JU4:1) to RA board via the PMOD1 Port (PMOD1:1)
	Jumper J10 short pin2-3 to select SCL Pin
	Jumper J11 short pin1-2 to select SDA Pin

    MCK-RA6T2:
    ---------
	Connect RA board to Host PC by Type C USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL0 PB06 (CN4-Pin33) <--------> SCL  (J1-Pin3)
	SDA0 PB07 (CN4-Pin31) <--------> SDA  (J1-Pin4)
	GND  	  (CN4-Pin25) <--------> GND  (J1-Pin5)
	VCC  	  (CN4-Pin23) <--------> VDD  (J1-Pin6)

    MCK-RA8T1:
    ---------
	Connect RA board to Host PC by Type C USB cable.
	Connect MAX31875PMB1 (JU4:1) to RA board via the PMOD6A Port (PMOD6A:1)

    RSSK-RA6T1:
    -----------
	Connect RA board to Host PC by Mini USB cable.
	Connect RA board to MAX31875PMB1 as below:
		  RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P101 (CN8-Pin02)  <--------> SCL  (J1-Pin3)
	SDA1 P100 (CN8-Pin03)  <--------> SDA  (J1-Pin4)
	GND  	  (CN8-Pin05)  <--------> GND  (J1-Pin5)
	VCC  	  (CN8-Pin06)  <--------> VDD  (J1-Pin6)


NOTE2:
    For using the J-Link RTT Viewer:
      1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
	RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x1ffe0a2c
	b. Keil:	Not Available
	c. IAR: 	Not Available
 
      2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
	in .map file generated in the build configuration folder (Debug/Release).
