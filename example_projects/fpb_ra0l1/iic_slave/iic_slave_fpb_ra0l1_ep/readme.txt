/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates typical use of the IIC Slave HAL module APIs. The project initializes IIC
	Slave and IIC Master modules with standard rate and is made interfaced with loop-back mechanism. It performs
	Slave read and write operations continuously once initialization is successful. On successful I2C transaction
	(6 bytes), the transmitted and received data are compared. LED blinks on data match; otherwise, it is turned
	ON as a sign of failure. Output message for both corresponding Slave operations is displayed on RTT Viewer.
	Any API/event failure message is also displayed.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2A2, EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6M1,
			     EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, FPB-RA6E1,
			     FPB-RA8E1, MCK-RA6T2, RSSK-RA6T1, FPB-RA0E2, EK-RA8E2, EK-RA8P1, EK-RA4C1, FPB-RA0L1
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	Some jumper wires.
	2 x Pull-up resistors of value 3.9k or 4.7k ohms.

3. Hardware Connections:
	Establish loopback connection along IIC lines within the board with pins as mentioned below.
	For EK-RA6M3, EK-RA6M3G:
	--------
		Channel 2 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA2 P511         ----> SDA0 P401
			SCL2 P512         ----> SCL0 P408

	For EK-RA6M2:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P206         ----> SDA0 P401
			SCL1 P100         ----> SCL0 P400

	For EK-RA6M1:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P101         ----> SDA0 P401
			SCL1 P100         ----> SCL0 P400

	For EK-RA4M1:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P206         ----> SDA0 P401
			SCL1 P100         ----> SCL0 P400

	For EK-RA2A1:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P400         ----> SDA0 P401
			SCL1 P109         ----> SCL0 P000

	For EK-RA4W1:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P206         ----> SDA0 P407
			SCL1 P205         ----> SCL0 P204

	For EK-RA6M4:
	--------
		Channel 0 has been used by IIC Master and channel 1 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA0 P401         ----> SDA1 P206
			SCL0 P408         ----> SCL1 P205

	For RSSK-RA6T1:
	--------
		Channel 0 has been used by IIC Master and channel 1 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA0 P401         ----> SDA1 P101
			SCL0 P400         ----> SCL1 P100

	For EK-RA2L1:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P206         ----> SDA0 P407
			SCL1 P205         ----> SCL0 P408

	For EK-RA4M3:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P511         ----> SDA0 P401
			SCL1 P512         ----> SCL0 P408

	For EK-RA4M2:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P206         ----> SDA0 P401
			SCL1 P205         ----> SCL0 P400

	For EK-RA6M5:
	--------
		Channel 1 has been used by IIC Master and channel 2 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P511 (J4:39) ----> SDA2 P414 (J1:32)
			SCL1 P512 (J4:38) ----> SCL2 P415 (J1:31)

	For FPB-RA6E1:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P206 (J3:31) ----> SDA0 P401 (J3:2)
			SCL1 P205 (J3:32) ----> SCL0 P400 (J3:1)

	For MCK-RA6T2:
	---------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 PB09 (CN5:1) ----> SDA0 PB07 (CN4:31)
			SCL1 PB08 (CN5:3) ----> SCL0 PB06 (CN4:33)

	For EK-RA8M1, EK-RA8D1:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P511         ----> SDA0 P409
			SCL1 P512         ----> SCL0 P410

	For EK-RA2A2:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P013         ----> SDA0 P410
			SCL1 P012         ----> SCL0 P411

	For FPB-RA8E1:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P511 (J4:4)  ----> SDA0 P409 (J2:11)
			SCL1 P512 (J4:3)  ----> SCL0 P410 (J2:12)

	For FPB-RA0E2:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IICA Master pins        IICA Slave pins
			SDA1 P401 (J1:2)  ----> SDA0 P913 (J1:19)
			SCL1 P400 (J1:1)  ----> SCL0 P914 (J1:18)

	For EK-RA8E2:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P511 (J4:3)  ----> SDA0 P409 (J2:28)
			SCL1 P512 (J4:5)  ----> SCL0 P410 (J2:26)  

	For EK-RA8P1:
	--------
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P511 (J1:2)  ----> SDA0 P409 (J17:13)
			SCL1 P512 (J1:4)  ----> SCL0 P410 (J17:16)

	For EK-RA4C1:
	--------
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins         IIC Slave pins
			SDA1 P401 (J1:4)  ----> SDA0 P301 (J22:3)
			SCL1 P400 (J1:3)  ----> SCL0 P302 (J22:4)

	For FPB-RA0L1:
	--------
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
		Channel 1 has been used by IIC Master and channel 0 has been used by IIC Slave.
			IIC Master pins           IIC Slave pins
			SDAA1 P407 (J1:12)  ----> SDAA0 P913 (J1:15)
			SCLA1 P408 (J1:11)  ----> SCLA0 P914 (J1:14)

Note:
1) DTC transfer support is configurable and is enabled in the configurator. DTC is helpful for minimizing interrupts
   during large transactions.
   Note: On FPB-RA0E2 and FPB-RA0L1 boards, DTC does not support the IICA module.

2) For the functioning of IIC Slave on all the boards except for EK-RA6M3, EK-RA6M3G, FPB-RA6E1, EK-RA2L1, EK-RA8D1,
   EK-RA8M1, EK-RA2A2, FPB-RA8E1, EK-RA8E2, EK-RA8P1, EK-RA4C1 external pull-up resistors of value 3.9 or 4.7k ohms
   are required to be connected on I2C (SDA/SCL) lines.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x20004c48
   b. Keil:	0x20004840
   c. IAR:	0x2000496c

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
