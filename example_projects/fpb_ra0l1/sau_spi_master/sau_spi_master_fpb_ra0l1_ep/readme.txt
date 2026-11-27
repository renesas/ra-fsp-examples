/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic usage of SAU SPI driver in master configuration on Renesas RA MCUs.
	The project writes commands to the slave digital thermometer module, and reads the temperature values using SAU
	SPI bus configured as master. The temperature values are continuously printed on the terminal. Additionally,
	status information and error messages will be printed on the terminal during the execution of the project.

	Note:
	- In addition to communicating with the digital thermometer module, the example project can also communicate
	  with the sau_spi_slave EP. For more information, please refer to the document of the sau_spi_slave EP.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Digital thermometer module (PMOD MAX31723PMB1).
		Link Product: https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31723PMB1?qs=UmMSjoC1xtH8e742i4OoUA%3D%3D
		Manufacturer Product Number: MAX31723PMB1#.
	1 x Host PC.

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For PMOD MAX31723PMB1:
		Select SPI communication protocol on the PMOD by configuring the following jumper configurations on JP1:
			Connect JP1-3 to JP1-5.
			Connect JP1-4 to JP1-6.
			Connect JP1-9 to JP1-10.

	For FPB-RA0E1:
		- Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port. Please carefully align pin 1
		  on the Temperature sensor with pin 1 on the PMOD1.
		- The soldering bridge E4 must be closed.
		- Populate R10 with a resistor ranging from 0 to 150 ohms.

	For FPB-RA0E2:
		- Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port. Please carefully align pin 1
		  on the Temperature sensor with pin 1 on the PMOD1.
		- The user must close E46, E47 to use J-Link OB VCOM for serial terminal (UART).

	For FPB-RA0L1:
		- Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port (J19). Please carefully align pin 1
		  on the Temperature sensor with pin 1 on the PMOD1.
		- The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
		- The user must close E28, E29 to use J-Link OB VCOM for serial terminal (UART).

	For FPB-RA0E3:
		- The user must turn OFF S3 to use the on-board debug functionality.
		- Connect PMOD MAX31723PMB1 (J1) to the RA board as shown below:
			Temperature sensor (J1:1) to P112 (J2:4).
			Temperature sensor (J1:2) to P213 (J1:4).
			Temperature sensor (J1:3) to P212 (J1:5).
			Temperature sensor (J1:4) to P201 (J1:8).
			Temperature sensor (J1:5) to GND  (J1:3).
			Temperature sensor (J1:6) to VCC  (J1:6).

Note:
1) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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

   	d. To show the degree Celsius symbol (°C), the user needs to select the "Consolas" font through:
		[Setup] -> [Font...] -> [Font:]

2) For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of
   sau_spi_master_notes.md.
