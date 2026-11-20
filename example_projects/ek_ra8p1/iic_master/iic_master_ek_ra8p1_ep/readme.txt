/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the IIC Master HAL module APIs. The project initializes IIC
	Master module with standard rate and interfaces with Pmod ACL Board for ADXL345. On power up after establishing
	the connection of sensor with RA board, it displays accelerometer axis data on the terminal. Additionally, any
	error messages will be displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: CK-RA6M5, EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2L1, EK-RA4C1, EK-RA4L1, EK-RA4M1, EK-RA4M2,
			     EK-RA4M3, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1,
			     EK-RA8D2, EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, FPB-RA0E1, FPB-RA0E2, FPB-RA0L1,
			     FPB-RA2E3, FPB-RA2T1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, MCK-RA6T2, MCK-RA8T1, MCK-RA8T2,
			     RSSK-RA6T1, FPB-RA4E2, FPB-RA6E2.
	1 x Renesas RA board.
	1 x Pmod ACL (e.g., https://digilent.com/reference/pmod/pmodacl/start).
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	Some jumper wires used to connect the RA board to the Pmod ACL.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For CK-RA6M5, EK-RA8M1, EK-RA8D1:
		SDA1 P511  ----> SDA (on Pmod ACL)
		SCL1 P512  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For FPB-RA8E1:
		SDA1 P511 (J4:4)  ----> SDA (on Pmod ACL)
		SCL1 P512 (J4:3)  ----> SCL (on Pmod ACL)
		VCC pin   (J4:1)  ----> VCC (on Pmod ACL)
		GND pin   (J4:2)  ----> GND (on Pmod ACL)
		The user must open jumpers J6 and J9 to use the on-board debug functionality.

	For EK-RA6M3G, EK-RA6M3:
		SDA2 P511  ----> SDA (on Pmod ACL)
		SCL2 P512  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For EK-RA6M2, EK-RA4M1:
		SDA1 P206  ----> SDA (on Pmod ACL)
		SCL1 P100  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For EK-RA6M1, EK-RA6M4, EK-RA6M5, EK-RA2E1, FPB-RA6E1, FPB-RA4E1:
		SDA0 P401  ----> SDA (on Pmod ACL)
		SCL0 P400  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)


	For FPB-RA2E3:
		SDA0 P401 (J3:2) ----> SDA (on Pmod ACL)
		SCL0 P400 (J3:1) ----> SCL (on Pmod ACL)
		VCC pin   (J3:9) ----> VCC (on Pmod ACL)
		GND pin   (J3:6) ----> GND (on Pmod ACL)
		The user must close E25, E28, E46, E47 to use J-Link OB VCOM for serial terminal (UART).

	For EK-RA2A1:
		SDA0 P401  ----> SDA (on Pmod ACL)
		SCL0 P000  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For RSSK-RA6T1:
		SDA1 P101  ----> SDA (on Pmod ACL)
		SCL1 P100  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For EK-RA2L1:
		SDA0 P407  ----> SDA (on Pmod ACL)
		SCL0 P408  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For EK-RA4M3:
		SDA0 P401  ----> SDA (on Pmod ACL)
		SCL0 P408  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For EK-RA4M2:
		SDA0 P206  ----> SDA (on Pmod ACL)
		SCL0 P205  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For EK-RA4W1:
		SDA1 P407  ----> SDA (on Pmod ACL)
		SCL1 P204  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For MCK-RA6T2:
		SDA0 PB07  ----> SDA (on Pmod ACL)
		SCL0 PB06  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For MCK-RA8T1:
		SDA1 P206 (CN2:24)  ----> SDA (on Pmod ACL)
		SCL1 P205 (CN2:25)  ----> SCL (on Pmod ACL)
		VCC pin   (CN1:24)  ----> VCC (on Pmod ACL)
		GND pin   (CN1:34)  ----> GND (on Pmod ACL)

	For EK-RA2A2:
		SDA0 P410  ----> SDA (on Pmod ACL)
		SCL0 P411  ----> SCL (on Pmod ACL)
		VCC pin    ----> VCC (on Pmod ACL)
		GND pin    ----> GND (on Pmod ACL)

	For FPB-RA0E1:
		SDAA0 P101  ----> SDA (on Pmod ACL)
		SCLA0 P100  ----> SCL (on Pmod ACL)
		VCC pin     ----> VCC (on Pmod ACL)
		GND pin     ----> GND (on Pmod ACL)

	For EK-RA4L1:
		Place J15 jumper to use P100, P101 for I2C channel 0.
		SDA0 P100 (J3:2) ----> SDA (on Pmod ACL)
		SCL0 P101 (J3:4) ----> SCL (on Pmod ACL)
		VCC pin          ----> VCC (on Pmod ACL)
		GND pin          ----> GND (on Pmod ACL)

	For FPB-RA0E2:
		SDAA0 P913 (J1:19)  ----> SDA (on Pmod ACL)
		SCLA0 P914 (J1:18)  ----> SCL (on Pmod ACL)
		VCC pin    (J1:11)  ----> VCC (on Pmod ACL)
		GND pin    (J1:8)   ----> GND (on Pmod ACL)

	For EK-RA8E2:
		SDA0 P409 (J2:28) ----> SDA (on Pmod ACL)
		SCL0 P410 (J2:26) ----> SCL (on Pmod ACL)
		VCC pin   (J2:1)  ----> VCC (on Pmod ACL)
		GND pin   (J2:39) ----> GND (on Pmod ACL)

	For EK-RA8P1, EK-RA8D2:
		SDA1 P511 (J1:2)  ----> SDA (on Pmod ACL)
		SCL1 P512 (J1:4)  ----> SCL (on Pmod ACL)
		VCC pin   (J1:5)  ----> VCC (on Pmod ACL)
		GND pin   (J1:40) ----> GND (on Pmod ACL)

	For FPB-RA2T1:
		Close E6 and open E5 to pull up the SDA0 P401 and SCL0 P400 pins
		SDA0 P401 (J3:2)  ----> SDA (on Pmod ACL)
		SCL0 P400 (J3:1)  ----> SCL (on Pmod ACL)
		VCC pin   (J3:9)  ----> VCC (on Pmod ACL)
		GND pin   (J3:6)  ----> GND (on Pmod ACL)

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		SDA1 P401 (J1:4)  ----> SDA (on Pmod ACL)
		SCL1 P400 (J1:3)  ----> SCL (on Pmod ACL)
		VCC pin   (J4:1)  ----> VCC (on Pmod ACL)
		GND pin   (J4:5)  ----> GND (on Pmod ACL)

	For MCK-RA8T2:
		SDA2 P514 (CN2:24)  ----> SDA (on Pmod ACL)
		SCL2 P515 (CN2:25)  ----> SCL (on Pmod ACL)
		VCC pin   (CN3:23)  ----> VCC (on Pmod ACL)
		GND pin   (CN3:25)  ----> GND (on Pmod ACL)

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
		SDAA0 P913 (J1:15)  ----> SDA (on Pmod ACL)
		SCLA0 P914 (J1:14)  ----> SCL (on Pmod ACL)
		VCC pin    (J1:9)   ----> VCC (on Pmod ACL)
		GND pin    (J1:6)   ----> GND (on Pmod ACL)

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.
		SDA1 P511 (J4:14)  ----> SDA (on Pmod ACL)
		SCL1 P512 (J4:11)  ----> SCL (on Pmod ACL)
		VCC pin   (J4:1)   ----> VCC (on Pmod ACL)
		GND pin   (J4:19)  ----> GND (on Pmod ACL)

	For FPB-RA4E2:
	--------
		SDA P101 (J4:15)  ----> SDA (on Pmod ACL)
		SCL P100 (J4:16)  ----> SCL (on Pmod ACL)
		VCC pin  (J4:7)   ----> VCC (on Pmod ACL)
		GND pin  (J4:8)   ----> GND (on Pmod ACL)

	For FPB-RA6E2:
	--------
		SDA P101 (J4:15)  ----> SDA (on Pmod ACL)
		SCL P100 (J4:16)  ----> SCL (on Pmod ACL)
		VCC pin  (J4:7)   ----> VCC (on Pmod ACL)
		GND pin  (J4:8)   ----> GND (on Pmod ACL)

Note:
1) For the functioning of IIC Master on all the boards except for EK-RA6M3, EK-RA6M3G, FPB-RA6E1, FPB-RA4E1,
   EK-RA8M1, FPB-RA2E3, EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA8P1, FPB-RA2T1, EK-RA4C1,
   MCK-RA8T2, EK-RA8D2, EK-RA8M2, FPB-RA4E2 and FPB-RA6E2 external pull up resistors of value 3.9 or 4.7K ohms are required
   to be connected on I2C (SDA/SCL) lines.

2) The SDA pin and the SCL pin must configure output type as n-ch open drain.

3) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
	a. To make sure the received data shows properly, the user needs to set:
		[Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR].

	b. To echo back characters typed in Tera Term, the user needs to enable it through:
		[Setup] -> [Terminal...] -> Check [Local echo].

	c. The configuration parameters of the serial port on the serial terminal application:
		Port: The port is provided by the J-Link On-Board.
		Speed: 115200
		Data: 8 bit
		Parity: none
		Stop bits: 1 bit
		Flow control: none

4) For GCC ARM Embedded Toolchain: To enable printing floats to the terminal, edit the project settings and ensure
   that "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous >
   Use float with nano printf
