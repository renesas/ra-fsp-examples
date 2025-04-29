/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the SAU I2C master HAL module APIs. The project initializes 
	the SAU I2C master module with the standard rate of 100 kHz and communicates with the PmodACL™ Board for ADXL345.
	On power-up, after initializing the SAU I2C master module and configuring the ADXL345 sensor, the RA MCU will
	read the axis acceleration data from the sensor and display it on the RTT Viewer every time new data is detected 
	by an external interrupt.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.9.0
	e2 studio: Version 2025-04
	SEGGER J-Link RTT Viewer: Version 8.12f
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware Requirements:
	Supported RA boards: FPB-RA0E1.
	1 x Renesas RA board.
	1 x USB type C cable for programming and debugging.
	1 x PmodACL™ Board (Part Number: 410-097) as a slave device.
	2 x Resistor (from 1k to 4.7k ohms) used to pull up the I2C bus.
	1 x Breadboard.
	8 x Jumper cables (6 x Male-Female, 1 x Male-Male, 1 x Female-Female) used to connect the RA board to the PmodACL™ and pull up the I2C bus.

4. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a USB type C cable.
	Connect the PmodACL™ Board to the RA board according to the instructions below for each board type:
	
	PMOD ACL
	-------- 
	J2 is used for I2C interface.
	J1 is used for SPI interface and interrupt pins.

	Pull-up Resistors
	----------------
	SDA pin and SCL pin must be connected to external pull-up resistors.

	FPB_RA0E1
	--------
	IRQ4/P109  (J2 - Pin 18) ----> INT1 (on PmodACL J1 - Pin 8)
	SCL20/P112 (J2 - Pin 20) ----> SCL  (on PmodACL J2 - Pin 1/5)
	SDA20/P110 (J2 - Pin 19) ----> SDA  (on PmodACL J2 - Pin 2/6)
	GND pin                  ----> GND  (on PmodACL J2 - Pin 3/7)
	VCC pin (3V3)            ----> VCC  (on PmodACL J2 - Pin 4/8)

Note:
For the functioning of the SAU I2C Master:
    1. SDA pin and SCL pin must configure output type as n-ch open drain.
    2. The value of the external pull-up resistor depends on the I2C bus speed. In this EP project, the I2C bus is configured at standard speed of 100 kHz, so the recommended external pull-up resistor value is from 1k to 4.7k ohms.

For Segger RTT block address:
1) The Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex files committed in repository are as follows:
   a. e2studio: 0x2000465c
   b. Keil:     Not Available
   c. IAR:      Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release)
