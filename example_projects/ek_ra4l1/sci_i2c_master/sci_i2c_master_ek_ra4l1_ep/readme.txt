/***********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
***********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the SCI I2C master HAL module APIs.
        The project initializes SCI I2C master module with the standard rate and interfaces with PmodACL™ Board for ADXL345.

	On power up, after establishing connection of the sensor with the RA board, the RA MCU will read axis acceleration
	from the sensor and display it on the RTT Viewer every time new data is detected by an external interrupt.

2. Hardware Requirements:
	1 x RA board
	1 x PmodACL board
	1 x Type-C USB cable for programming and debugging
	2 x Resistors (from 1k to 4.7k ohms)
	Some jumper wires

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.
	Connect the PmodACL™ Board to the RA board according to the instructions below for each board type:
	
	For PMOD ACL:
	-------- 
		J2 is used for I2C communication.
		J1 is used for SPI communication and interrupt pins.

	For CK-RA6M5:
	--------
		IRQ11/P006 ----> INT1 (on PmodACL J1 - Pin 8)
		SCL3/P706  ----> SCL  (on PmodACL J2 - Pin 1/5)
		SDA3/P707  ----> SDA  (on PmodACL J2 - Pin 2/6)
		GND pin    ----> GND  (on PmodACL J2 - Pin 3/7)
		VCC pin    ----> VCC  (on PmodACL J2 - Pin 4/8)
	
	For EK-RA2E2:
	--------
		IRQ4/P102  ----> INT1 (on PmodACL J1 - Pin 8)
		SCL9/P103  ----> SCL  (on PmodACL J2 - Pin 1/5)
		SDA9/P101  ----> SDA  (on PmodACL J2 - Pin 2/6)
		GND pin    ----> GND  (on PmodACL J2 - Pin 3/7)
		VCC pin    ----> VCC  (on PmodACL J2 - Pin 4/8)
	
	For EK-RA4E2, EK-RA6E2:
	--------
		IRQ6/P409  ----> INT1 (on PmodACL J1 - Pin 8)
		SCL0/P410  ----> SCL  (on PmodACL J2 - Pin 1/5)
		SDA0/P411  ----> SDA  (on PmodACL J2 - Pin 2/6)
		GND pin    ----> GND  (on PmodACL J2 - Pin 3/7)
		VCC pin    ----> VCC  (on PmodACL J2 - Pin 4/8)
	
	For MCK-RA4T1, MCK-RA6T3:
	--------
		IRQ6/P301  ----> INT1 (on PmodACL J1 - Pin 8)
		SCL9/P110  ----> SCL  (on PmodACL J2 - Pin 1/5)
		SDA9/P109  ----> SDA  (on PmodACL J2 - Pin 2/6)
		GND pin    ----> GND  (on PmodACL J2 - Pin 3/7)
		VCC pin    ----> VCC  (on PmodACL J2 - Pin 4/8)
	
	For MCK-RA8T1:
	--------
		IRQ2/P100 (CN2:P7)   ----> INT1 (on PmodACL J1 - Pin 8)
		SCL9/P113 (CN1:P33)  ----> SCL  (on PmodACL J2 - Pin 1/5)
		SDA9/P112 (CN2:P1)   ----> SDA  (on PmodACL J2 - Pin 2/6)
		GND pin   (CN1:P34)  ----> GND  (on PmodACL J2 - Pin 3/7)
		VCC pin   (CN1:P24)  ----> VCC  (on PmodACL J2 - Pin 4/8)

	For FPB-RA8E1:
	--------
		IRQ9/P414 (J4:P17)  ----> INT1 (on PmodACL J1 - Pin 8)
		SCL0/P602 (J3:P5)   ----> SCL  (on PmodACL J2 - Pin 1/5)
		SDA0/P603 (J3:P6)   ----> SDA  (on PmodACL J2 - Pin 2/6)
		GND pin   (J3:P20)  ----> GND  (on PmodACL J2 - Pin 3/7)
		VCC pin   (J3:P1)   ----> VCC  (on PmodACL J2 - Pin 4/8)

	For EK-RA4L1:
	--------
		IRQ0/P105 (J3:P12)  ----> INT1 (on PmodACL J1 - Pin 8)
		SCL1/P608 (J2:P37)  ----> SCL  (on PmodACL J2 - Pin 1/5)
		SDA1/P115 (J2:P35)  ----> SDA  (on PmodACL J2 - Pin 2/6)
		GND pin   (J3:P25)  ----> GND  (on PmodACL J2 - Pin 3/7)
		VCC pin   (J3:P1)   ----> VCC  (on PmodACL J2 - Pin 4/8)
Note:
1) For the functioning of the SCI I2C Master:
   a. SDA pin and SCL pin must configure output type as n-ch open drain.
   b. SDA pin and SCL pin must be connected to external pull-up resistors.
   c. The value of the external pull-up resistor depends on the I2C bus speed. In this EP project, the I2C bus
      is configured at standard speed, so the recommended external pull-up resistor value is from 1k to 4.7k ohms.

2) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000a3c
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the build configuration folder (Debug/Release).

4) For GCC ARM Embedded Toolchain: To enable printing floats to RTT Viewer, edit the project settings and make sure 
   use nano with printf is enabled. The setting can be found by
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Misc > Use float with nano printf
