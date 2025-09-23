/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates SCI_I2C Master operation through loop-back with IIC Slave driver. 6 bytes of
	data will be transmitted and received continuously on successful initialization. The transmitted data is
	compared with the received data. If the data matches, on-board LEDs start blinking. On a data mismatch, the
	LEDs stay ON. Failure messages and status are displayed on RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	2 x Jumper wires.

3. Hardware Connections:
	Jumper wires are required to establish loopback connection along IIC lines on the board with pins as
	mentioned below:

	For EK-RA6M3, EK-RA6M3G:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P411 (J3:36) ----> SDA0 P401 (J3:09)
			SCL0 P410 (J3:35) ----> SCL0 P408 (J3:37)

	For EK-RA6M2:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P411 (J2:20) ----> SDA0 P401 (J4:11)
			SCL0 P410 (J2:02) ----> SCL0 P400 (J4:13)

	For EK-RA6M1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P411 (J2:20) ----> SDA0 P401 (J1:03)
			SCL0 P410 (J4:14) ----> SCL0 P400 (J1:01)

	For EK-RA6M4:
	--------
		Channel 7 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA7 P613 (J3:16) ----> SDA1 P511 (J2:38)
			SCL7 P614 (J3:17) ----> SCL1 P512 (J2:37)

	For RSSK-RA6T1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P101 (CN8:2) ----> SDA0 P401 (CN6:1)
			SCL0 P100 (CN8:3) ----> SCL0 P400 (CN9:1)

	For EK-RA4W1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
			SCI_I2C Master pins      IIC Slave pins
			SDA0 P101 (CN7:26) ----> SDA0 P407 (CN7:01)
			SCL0 P100 (CN7:27) ----> SCL0 P204 (CN7:09)

	For EK-RA4M1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P411 (J2:04) ----> SDA0 P401 (J2:08)
			SCL0 P410 (J2:02) ----> SCL0 P400 (J2:37)

	For EK-RA2A1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P302 (J2:04) ----> SDA0 P401 (J1:03)
			SCL0 P301 (J2:02) ----> SCL0 P000 (J1:01)
	
	For EK-RA2L1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P205 (J1:15) ----> SDA0 P401 (J4:08)
			SCL0 P206 (J1:14) ----> SCL0 P400 (J4:07)

	For EK-RA4M3:
	--------
		Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA9 P602 (J3:28) ----> SDA1 P511 (J2:38)
			SCL9 P601 (J3:29) ----> SCL1 P512 (J2:37)

	For EK-RA4M2:
	---------
		Channel 9 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA9 P602 (J3:28) ----> SDA0 P401 (J1:03)
			SCL9 P601 (J3:29) ----> SCL0 P400 (J1:04)

	For EK-RA2E1:
	---------
		Channel 2 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins      IIC Slave pins
			SDA2 P302 (J27:02) ----> SDA0 P407 (J2:09)
			SCL2 P301 (J27:01) ----> SCL0 P408 (J2:08)

	For EK-RA6M5:
	---------
		Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA9 P602 (J3:29) ----> SDA1 P511 (J4:39) 
			SCL9 P601 (J3:30) ----> SCL1 P512 (J4:38)

	For FPB-RA6E1:
	----------
		Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA9 P602 (J4:15) ----> SDA1 P206 (J3:31)
			SCL9 P601 (J4:16) ----> SCL1 P205 (J3:32)

	For FPB-RA4E1:
	-----------
		Channel 4 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA4 P206 (J3:31) ----> SDA0 P400 (J3:1)
			SCL4 P205 (J3:32) ----> SCL0 P401 (J3:2)

	For MCK-RA6T2:
	-----------
		Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA1 PB08 (CN5:3) ----> SDA0 PB06 (CN4:33)
			SCL1 PB09 (CN5:1) ----> SCL0 PB07 (CN4:31)

	For EK-RA8M1:
	-----------
		Remove Jumper J61 to disable I3C, use pin P401, P400 for SCI_I2C.
		Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins      IIC Slave pins
			SDA1 P401 (J56:32) ----> SDA0 P410 (J51:46)
			SCL1 P400 (J56:36) ----> SCL0 P409 (J51:49)

	For FPB-RA2E3:
	-----------
		Channel 0 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA1 P100 (J4:12) ----> SDA0 P400 (J3:1)
			SCL1 P101 (J4:11) ----> SCL0 P401 (J3:2)

	For EK-RA8D1:
	-----------
		Set the configuration switches (SW1) as below to avoid potential failures.
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |      OFF    |      OFF     |     OFF    |     OFF    |      OFF   |      OFF    |     OFF   |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+	

		CAUTION: Do not enable SW1-3, SW1-5 and SW1-8, use pin P401, P400 for SCI_I2C.

		Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins      IIC Slave pins
			SDA1 P401 (J51:7)  ----> SDA0 P410 (J52:11)
			SCL1 P400 (J51:10) ----> SCL0 P409 (J52:16)

	For MCK-RA8T1:
	-----------
		Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA9 P101 (CN2:6) ----> SDA1 P205 (CN2:25)
			SCL9 P102 (CN2:5) ----> SCL1 P206 (CN2:24)
	
	For EK-RA2A2:
	-----------
		Channel 0 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P101 (J4:05) ----> SDA0 P411 (J2:16)
			SCL0 P100 (J4:04) ----> SCL0 P410 (J2:15)

	For FPB-RA8E1:
	-----------
		Channel 0 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P113 (J3:17) ----> SDA1 P512 (J4:3)
			SCL0 P112 (J3:18) ----> SCL1 P511 (J4:4)

	For EK-RA4L1:
	-----------
		Place J15 jumper to use P100, P101 for IIC channel 0.
		Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA1 P608 (J2:37) ----> SDA0 P101 (J3:4)
			SCL1 P115 (J2:35) ----> SCL0 P100 (J3:2)

	For EK-RA8E2:
	-----------
		Channel 1 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
			SCI_I2C Master pins    IIC Slave pins
			SDA1 P401 (J2:3) ----> SDA1 P512 (J4:5)
			SCL1 P400 (J2:2) ----> SCL1 P511 (J4:3)

	For EK-RA8P1:
	-----------
		Channel 0 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
			SCI_I2C Master pins    IIC Slave pins
			SDA0 P603 (J4:4) ----> SDA1 P511 (J1:2)
			SCL0 P602 (J4:8) ----> SCL1 P512 (J1:4)

	For FPB-RA2T1:
	-----------
		The user must Close E8, E15 and Cut E10 to use P213 for LED1.
		The user must Close E7 to use P914 for LED2.
		The user must Close E6 and Cut E5 to pull up the SDA0 P401 and SCL0 P400 pins
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
			SCI_I2C Master pins     IIC Slave pins
			SDA0 P101 (J4:11) ----> SDA1 P401 (J3:2)
			SCL0 P100 (J4:12) ----> SCL1 P400 (J3:1)

	For EK-RA4C1:
	-----------
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
			SCI_I2C Master pins    IIC Slave pins
			SDA1 P400 (J1:3) ----> SDA0 P301 (J22:3)
			SCL1 P401 (J1:4) ----> SCL0 P302 (J22:4)

	For MCK-RA8T2:
	-----------
		Channel 0 has been used by SCI_I2C Master and Channel 2 has been used by IIC Slave.
			SCI_I2C Master pins      IIC Slave pins
			SDA0 P603 (CN1:33) ----> SDA2 P514 (CN2:24)
			SCL0 P602 (CN1:31) ----> SCL2 P515 (CN2:25)

Note:
1) For the functioning of SCI_I2C Master and IIC Slave on all the boards except for EK-RA6M3, EK-RA6M3G, FPB-RA6E1,
   FPB-RA4E1, FPB-RA2E3, MCK-RA8T1, EK-RA2A2, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA8P1, FPB-RA2T1, EK-RA4C1, MCK-RA8T2,
   external pull up resistors of value 3.9 or 4.7k ohms are required to be connected on I2C (SDA/SCL) lines.
   The SDA pin and the SCL pin must configure output type as n-ch open drain.

2) For EK-RA8M1, EK-RA8D1 value of external pull up resistors is 1.0k ohms.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000450
   b. Keil:	Not Available
   c. IAR: 	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
