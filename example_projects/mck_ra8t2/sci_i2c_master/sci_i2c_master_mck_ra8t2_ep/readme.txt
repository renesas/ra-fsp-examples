/***********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
***********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the SCI I2C Master HAL module APIs. The project initializes
	SCI I2C Master module with the standard rate and interfaces with Pmod ACL for ADXL345. On power up, after
	establishing connection of the sensor with the RA board, the RA MCU will read axis acceleration from the sensor
	and display it on the RTT Viewer every time new data is detected by an external interrupt.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Pmod ACL (e.g., https://digilent.com/reference/pmod/pmodacl/start).
	1 x Type-C USB cable for programming and debugging.
	2 x Resistors (from 1k to 4.7k ohms).
	Some jumper wires.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.
	Connect the Pmod ACL to the RA board according to the instructions below for each board type:
	For Pmod ACL:
	-------- 
		J2 is used for I2C communication.
		J1 is used for SPI communication and interrupt pins.

	For CK-RA6M5:
	--------
		IRQ11 P006 ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL3 P706  ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA3 P707  ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin    ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin    ----> VCC  (on Pmod ACL J2 - Pin 4/8)
	
	For EK-RA2E2:
	--------
		IRQ4 P102  ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL9 P103  ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA9 P101  ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin    ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin    ----> VCC  (on Pmod ACL J2 - Pin 4/8)
	
	For EK-RA4E2, EK-RA6E2:
	--------
		IRQ6 P409  ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL0 P410  ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA0 P411  ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin    ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin    ----> VCC  (on Pmod ACL J2 - Pin 4/8)
	
	For MCK-RA4T1, MCK-RA6T3:
	--------
		IRQ6 P301  ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL9 P110  ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA9 P109  ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin    ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin    ----> VCC  (on Pmod ACL J2 - Pin 4/8)
	
	For MCK-RA8T1:
	--------
		IRQ2 P100 (CN2:7)   ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL9 P113 (CN1:33)  ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA9 P112 (CN2:1)   ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin   (CN1:34)  ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin   (CN1:24)  ----> VCC  (on Pmod ACL J2 - Pin 4/8)

	For FPB-RA8E1:
	--------
		IRQ9 P414 (J4:17)  ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL0 P602 (J3:5)   ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA0 P603 (J3:6)   ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin   (J3:20)  ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin   (J3:1)   ----> VCC  (on Pmod ACL J2 - Pin 4/8)

	For EK-RA4L1:
	--------
		IRQ0 P105 (J3:12)  ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL1 P608 (J2:37)  ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA1 P115 (J2:35)  ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin   (J3:25)  ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin   (J3:1)   ----> VCC  (on Pmod ACL J2 - Pin 4/8)

	For FPB-RA2L2:
	--------
		IRQ3 P004 (J1:3)   ----> INT1 (on Pmod ACL J1 - Pin 8) 
		SCL0 P100 (J2:39)  ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA0 P101 (J2:38)  ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin   (J2:40)  ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin   (J2:2)   ----> VCC  (on Pmod ACL J2 - Pin 4/8)

	For EK-RA8E2:
	--------
		IRQ9 P414 (J2:21)  ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL1 P401 (J2:3)   ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA1 P400 (J2:2)   ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin   (J2:39)  ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin   (J2:1)   ----> VCC  (on Pmod ACL J2 - Pin 4/8)

	For EK-RA8P1:
	--------
		IRQ15 P012 (J2:25) ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL0  P602 (J4:8)  ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA0  P603 (J4:4)  ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin    (J2:39) ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin    (J2:40) ----> VCC  (on Pmod ACL J2 - Pin 4/8)

	For FPB-RA2T1:
	--------
		IRQ4 P111 (J4:4)   ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL2 P301 (J3:23)  ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA2 P302 (J3:22)  ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin   (J2:6)   ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin   (J2:4)   ----> VCC  (on Pmod ACL J2 - Pin 4/8)

	For EK-RA4C1:
	--------
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		IRQ15 P513 (J3:12) ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL1 P608  (J2:37) ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA1 P115  (J2:35) ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin    (J2:1)  ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin    (J1:1)  ----> VCC  (on Pmod ACL J2 - Pin 4/8)

	For MCK-RA8T2:
	--------
		IRQ17 P710 (CN6:1)  ----> INT1 (on Pmod ACL J1 - Pin 8)
		SCL0  P602 (CN1:31) ----> SCL  (on Pmod ACL J2 - Pin 1/5)
		SDA0  P603 (CN1:33) ----> SDA  (on Pmod ACL J2 - Pin 2/6)
		GND pin    (CN1:25) ----> GND  (on Pmod ACL J2 - Pin 3/7)
		VCC pin    (CN1:23) ----> VCC  (on Pmod ACL J2 - Pin 4/8)

Note:
1) For the functioning of the SCI I2C Master:
   a. SDA pin and SCL pin must configure output type as n-ch open drain.
   b. SDA pin and SCL pin must be connected to external pull-up resistors.
   c. The value of the external pull-up resistor depends on the I2C bus speed. In this EP, the I2C bus
      is configured at standard speed, so the recommended external pull-up resistor value is from 1k to 4.7k ohms.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000470
   b. Keil: 	Not Available
   c. IAR: 	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

4) For GCC ARM Embedded Toolchain: To enable printing floats to the RTT Viewer, edit the project settings and ensure that
   "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf
