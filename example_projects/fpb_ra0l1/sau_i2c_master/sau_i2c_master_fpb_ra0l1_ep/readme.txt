/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionality of the SAU I2C driver on Renesas RA MCUs
	based on FSP. The project initializes the SAU I2C Master module with the standard rate of 100 kHz
	and communicates with the Pmod ACL Board for ADXL345. On power-up, after initializing the SAU I2C
	Master module and configuring the ADXL345 sensor, the RA MCU will read the axis acceleration data
	from the sensor and display it on the terminal every time new data is detected by an external interrupt.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Pmod ACL (e.g., https://digilent.com/reference/pmod/pmodacl/start).
	2 x Resistors (From 1k to 4.7k ohms) used to pull up the I2C bus.
	1 x Breadboard.
	Some jumper wires used to connect the RA board to the Pmod ACL and pull up the I2C bus.

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.
	Connect the Pmod ACL to the RA board according to the instructions below for each board type.
	
	Pmod ACL:
	--------
		J2 is used for I2C interface.
		J1 is used for SPI interface and interrupt pins.

	Pull-up Resistors:
	--------
		SDA pin and SCL pin must be connected to external pull-up resistors.

	For FPB-RA0E1:
	--------
		IRQ4  P109 (J2:18) ----> INT1 (on Pmod ACL J1:8)
		SCL20 P112 (J2:20) ----> SCL  (on Pmod ACL J2:1/5)
		SDA20 P110 (J2:19) ----> SDA  (on Pmod ACL J2:2/6)
		GND pin            ----> GND  (on Pmod ACL J2:3/7)
		VCC pin (3V3)      ----> VCC  (on Pmod ACL J2:4/8)

	For FPB-RA0E2:
	--------
		IRQ4  P109 (J2:2)  ----> INT1 (on Pmod ACL J1:8)
		SCL20 P112 (J2:5)  ----> SCL  (on Pmod ACL J2:1/5)
		SDA20 P110 (J2:3)  ----> SDA  (on Pmod ACL J2:2/6)
		GND                ----> GND  (on Pmod ACL J2:3/7)
		VCC pin (3V3)      ----> VCC  (on Pmod ACL J2:4/8)

	For FPB-RA0L1:
	--------
		IRQ4  P109 (J2:2)  ----> INT1 (on Pmod ACL J1:8)
		SCL00 P102 (J2:10) ----> SCL  (on Pmod ACL J2:1/5)
		SDA00 P100 (J2:12) ----> SDA  (on Pmod ACL J2:2/6)
		GND                ----> GND  (on Pmod ACL J2:3/7)
		VCC pin (3V3)      ----> VCC  (on Pmod ACL J2:4/8)
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

	For FPB-RA0E3:
	--------
		IRQ4  P109 (J2:2)  ----> INT1 (on Pmod ACL J1:8)
		SCL00 P102 (J2:5)  ----> SCL  (on Pmod ACL J2:1/5)
		SDA00 P100 (J2:7)  ----> SDA  (on Pmod ACL J2:2/6)
		GND                ----> GND  (on Pmod ACL J2:3/7)
		VCC pin (3V3)      ----> VCC  (on Pmod ACL J2:4/8)
		The user must turn OFF S3 to use the on-board debug functionality.

Note:
1) For the functioning of the SAU I2C Master:
   a. SDA pin and SCL pin must be configured output type as n-ch open drain.
   b. The value of the external pull-up resistor depends on the I2C bus speed. In this EP, the I2C bus
      is configured at standard speed of 100 kHz, so the recommended external pull-up resistor value is from 1k to 4.7k ohms.

2) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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

3) For GCC ARM Embedded Toolchain (Except for FPB-RA0E3): To enable printing floats via printf, edit the project
   settings and ensure that "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf
