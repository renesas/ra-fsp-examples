/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the SCI I2C master HAL module APIs.
	The project initializes SCI I2C master module with the standard rate and interfaces with PmodACL™ Board for ADXL345.

	On power up, after establishing connection of the sensor with the RA board, the RA MCU will read axis acceleration
	from the sensor and display it on the RTT Viewer every time new data is detected by an external interrupt.

2. Hardware Requirements:
	1 x Renesas RA board
	1 x PmodACL™ Board
	1 x Micro USB cable
	2 x Resistor (from 1k to 4.7k ohms)
	Some jumper cables

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a micro USB cable.
	Connect the PmodACL™ Board to the RA board according to the instructions below for each board type:
	
	PMOD ACL
	-------- 
	J2 is used for I2C communication.
	J1 is used for SPI communication and interrupt pins.

	Pull-up Resistors
	----------------
	SDA pin and SCL pin must be connected to external pull-up resistors.

	RA6M5_CK
	--------
	IRQ11/P006 (J8 - Pin 3) ----> INT1 (on PmodACL J1 - Pin 8)
	SCL3/P706  (J8 - Pin 1) ----> SCL  (on PmodACL J2 - Pin 1/5)
	SDA3/P707  (J8 - Pin 2) ----> SDA  (on PmodACL J2 - Pin 2/6)
	GND pin                 ----> GND  (on PmodACL J2 - Pin 3/7)
	VCC pin (3V3)           ----> VCC  (on PmodACL J2 - Pin 4/8)

	RA2E2_EK
	--------
	IRQ4/P102  ----> INT1 (on PmodACL J1 - Pin 8)
	SCL9/P103  ----> SCL  (on PmodACL J2 - Pin 1/5)
	SDA9/P101  ----> SDA  (on PmodACL J2 - Pin 2/6)
	GND pin    ----> GND  (on PmodACL J2 - Pin 3/7)
	VCC pin (3V3)    ----> VCC  (on PmodACL J2 - Pin 4/8)

	RA4E2_EK & RA6E2_EK
	--------
	IRQ6/P409  ----> INT1 (on PmodACL J1 - Pin 8)
	SCL0/P410  ----> SCL  (on PmodACL J2 - Pin 1/5)
	SDA0/P411  ----> SDA  (on PmodACL J2 - Pin 2/6)
	GND pin    ----> GND  (on PmodACL J2 - Pin 3/7)
	VCC pin    ----> VCC  (on PmodACL J2 - Pin 4/8)

	RA4T1_MCK & RA6T3_MCK
	--------
	IRQ6/P301 (CN5 - Pin 4)	----> INT1 (on PmodACL J1 - Pin 8)
	SCL9/P110 (CN5 - Pin 3)	----> SCL  (on PmodACL J2 - Pin 1/5)
	SDA9/P109 (CN5 - Pin 2)	----> SDA  (on PmodACL J2 - Pin 2/6)
	GND pin	  (CN5 - Pin 5)	----> GND  (on PmodACL J2 - Pin 3/7)
	VCC pin   (CN5 - Pin 6)	----> VCC  (on PmodACL J2 - Pin 4/8)

Note:
For the functioning of the SCI I2C Master:
    1. SDA pin and SCL pin must configure output type as n-ch open drain.
    2. The value of the external pull-up resistor depends on the I2C bus speed.
    In this EP project, the I2C bus is configured at standard speed, so the recommended external pull-up resistor value is from 1k to 4.7k ohms.

For Segger RTT block address:
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000a44
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release)
