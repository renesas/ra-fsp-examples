/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates SCI_I2C Master operation through loop-back with IIC Slave driver.
	6 bytes of data will be transmitted and received continuously on successful initialization. 
	The transmitted data is compared with the received data. If the data matches, on-board LED starts blinking.
	On a data mismatch, LED stays ON. Failure messages and status is displayed on RTT Viewer.

2. Hardware Connections:
	Jumper wires are required to establish loop back connection along IIC lines within the board with pins as
	mentioned below:

	For EK-RA6M3, EK-RA6M3G:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P411 (J3:36)  ----> SDA
			SCI0 P410 (J3:35)  ----> SCL
		2) Slave IIC pins
			IIC0 P401 (J3:09)  ----> SDA
			IIC0 P408 (J3:37)  ----> SCL

	For EK-RA6M2:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P411 (J2:20)  ----> SDA
			SCI0 P410 (J2:02)  ----> SCL
		2) Slave IIC pins
			IIC0 P401 (J4:11)  ----> SDA
			IIC0 P400 (J4:13)  ----> SCL

	For EK-RA6M1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P411 (J2:20)  ----> SDA
			SCI0 P410 (J4:14)  ----> SCL
		2) Slave IIC pins
			IIC0 P401 (J1:03)  ----> SDA
			IIC0 P400 (J1:01)  ----> SCL

	For EK-RA6M4:
	--------
		Channel 7 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI7 P613 (J3:16)  ----> SDA
			SCI7 P614 (J3:17)  ----> SCL
		2) Slave IIC pins
			IIC1 P511 (J2:38)  ----> SDA
			IIC1 P512 (J2:37)  ----> SCL

	For RSSK-RA6T1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P101 (CN8:2)  ----> SDA
			SCI0 P100 (CN8:3)  ----> SCL
		2) Slave IIC pins
			IIC0 P401 (CN6:1)  ----> SDA
			IIC0 P400 (CN9:1)  ----> SCL

	For EK-RA4W1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P101 (CN7:26) ----> SDA
			SCI0 P100 (CN7:27) ----> SCL
		2) Slave IIC pins
			IIC0 P407 (CN7:01) ----> SDA
			IIC0 P204 (CN7:09) ----> SCL

	For EK-RA4M1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P411 (J2:04)  ----> SDA
			SCI0 P410 (J2:02)  ----> SCL
		2) Slave IIC pins
			IIC0 P401 (J2:08)  ----> SDA
			IIC0 P400 (J2:37)  ----> SCL

	For EK-RA2A1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P302 (J2:04)  ----> SDA
			SCI0 P301 (J2:02)  ----> SCL
		2) Slave IIC pins
			IIC0 P401 (J1:03)  ----> SDA
			IIC0 P000 (J1:01)  ----> SCL
	
	For EK-RA2L1:
	--------
		Channel 0 has been used by SCI_I2C Master and IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P205 (J1:15)  ----> SDA
			SCI0 P206 (J1:14)  ----> SCL
		2) Slave IIC pins
			IIC0 P401 (J4:08)  ----> SDA
			IIC0 P400 (J4:07)  ----> SCL

	For EK-RA4M3:
	--------
		Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI9 P602 (J3:28)  ----> SDA
			SCI9 P601 (J3:29)  ----> SCL
		2) Slave IIC pins
			IIC1 P511 (J2:38)  ----> SDA
			IIC1 P512 (J2:37)  ----> SCL

	For EK-RA4M2:
	---------
		Channel 9 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI9 P602 (J3:28)  ----> SDA
			SCI9 P601 (J3:29)  ----> SCL 
		2) Slave IIC pins
			IIC0 P401 (J1:03)  ----> SDA
			IIC0 P400 (J1:04)  ----> SCL

	For EK-RA2E1:
	---------
		Channel 2 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI2 P302 (J27:02) ----> SDA
			SCI2 P301 (J27:01) ----> SCL 
		2) Slave IIC pins
			IIC0 P407 (J2:09)  ----> SDA
			IIC0 P408 (J2:08)  ----> SCL

	For EK-RA6M5:
	---------
		Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI9 P602 (J3:29)  ----> SDA
			SCI9 P601 (J3:30)  ----> SCL 
		2) Slave IIC pins
			IIC1 P511  (J4:39)  ----> SDA
			IIC1 P512  (J4:38)  ----> SCL

	For FPB-RA6E1:
	----------
		Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI9 P602 (J4:15)  ----> SDA
			SCI9 P601 (J4:16)  ----> SCL
		2) Slave IIC pins
			IIC1 P206 (J3:31)  ----> SDA
			IIC1 P205 (J3:32)  ----> SCL

	For FPB-RA4E1:
	-----------
		Channel 4 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI4 P206 (J3:31)  ----> SCL
			SCI4 P205 (J3:32)  ----> SDA
		2) Slave IIC pins
			IIC0 - P400 (J3:1) ----> SCL 
			IIC0 - P401 (J3:2) ----> SDA

	For MCK-RA6T2:
	-----------
		Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI1 PB08 (CN5:3)  ----> SCL
			SCI1 PB09 (CN5:1)  ----> SDA
		2) Slave IIC pins
			IIC0 PB06 (CN4:33) ----> SCL 
			IIC0 PB07 (CN4:31) ----> SDA

	For EK-RA8M1:
	-----------
		Remove Jumper J61 to disable I3C, use pin P401, P400 for SCI_I2C.
		Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI1 P401 (J56:32) ----> SCL
			SCI1 P400 (J56:36) ----> SDA
		2) Slave IIC pins
			IIC0 P410 (J51:46) ----> SCL 
			IIC0 P409 (J51:49) ----> SDA

	For FPB-RA2E3:
	-----------
		Channel 0 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI1 P100 (J4:12)  ----> SCL
			SCI1 P101 (J4:11)  ----> SDA
		2) Slave IIC pins
			IIC0 P400 (J3:1)   ----> SCL 
			IIC0 P401 (J3:2)   ----> SDA

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
		1) SCI_I2C Master pins
			SCI1 P401 (J51:7)  ----> SCL
			SCI1 P400 (J51:10) ----> SDA
		2) Slave IIC pins
			IIC0 P410 (J52:11) ----> SCL 
			IIC0 P409 (J52:16) ----> SDA

	For MCK-RA8T1:
	-----------
		Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI9 P101 (CN2:6)  ----> SCL
			SCI9 P102 (CN2:5)  ----> SDA
		2) Slave IIC pins
			IIC1 P205 (CN2:25) ----> SCL 
			IIC1 P206 (CN2:24) ----> SDA
	
	For EK-RA2A2:
	-----------
		Channel 0 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P101 (J4:05)  ----> SCL
			SCI0 P100 (J4:04)  ----> SDA
		2) Slave IIC pins
			IIC0 P411 (J2:16)  ----> SCL 
			IIC0 P410 (J2:15)  ----> SDA

	For FPB-RA8E1:
	-----------
		Channel 0 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI0 P113 (J3:17)  ----> SCL
			SCI0 P112 (J3:18)  ----> SDA
		2) Slave IIC pins
			IIC1 P512 (J4:3)   ----> SCL 
			IIC1 P511 (J4:4)   ----> SDA

	For EK-RA4L1:
	-----------
		Place J15 jumper to use P100, P101 for IIC channel 0.
		Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI1 P608 (J2:37)  ----> SCL
			SCI1 P115 (J2:35)  ----> SDA
		2) Slave IIC pins
			IIC0 P101 (J3:4)   ----> SCL 
			IIC0 P100 (J3:2)   ----> SDA
	For EK-RA8E2:
	-----------
		Channel 1 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
		1) SCI_I2C Master pins
			SCI1 P401 (J2:3)   ----> SCL
			SCI1 P400 (J2:2)   ----> SDA
		2) Slave IIC pins
			IIC1 P512 (J4:5)   ----> SCL 
			IIC1 P511 (J4:3)   ----> SDA

Note:
1) For the functioning of SCI_I2C Master and IIC Slave on all the boards except for EK-RA6M3, EK-RA6M3G, FPB-RA6E1,
   FPB-RA4E1, FPB-RA2E3, MCK-RA8T1, EK-RA2A2, FPB-RA8E1, EK-RA4L1, EK-RA8E2 external pull up resistors of value 3.9 or 4.7k ohms are 
   required to be connected on I2C (SDA/SCL) lines.

2) For EK-RA8M1, EK-RA8D1 value of external pull up resistors is 1.0k ohms.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2204044c
   b. Keil: 	Not Available 
   c. IAR: 	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
