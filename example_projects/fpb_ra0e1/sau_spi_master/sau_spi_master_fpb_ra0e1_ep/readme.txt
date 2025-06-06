/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the basic usage of SAU SPI driver in master configuration on Renesas RA MCUs.
	The project writes commands to the slave digital thermometer module, and reads the temperature values using SAU SPI
	bus configured as master. The temperature values are continuously printed on terminal. Status information and error
	messages will be printed on terminal during the execution of the project.

2. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1
	1 x Renesas RA board.
	1 x Type-C USB cable.
	1 x Digital thermometer module (PMOD MAX31723PMB1)
		Link Product: https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31723PMB1?qs=UmMSjoC1xtH8e742i4OoUA%3D%3D
		Manufacturer Product Number: MAX31723PMB1#
	1 x Host PC.
	
3. Hardware Connections:
	PMOD MAX31723PMB1:
	---------
	Select SPI communication protocol on the PMOD by configuring the following jumper configurations on JP1:
	Connect JP1-3 to JP1-5
	Connect JP1-4 to JP1-6
	Connect JP1-9 to JP1-10

	FPB-RA0E1:
	---------
	Connect RA board to Host PC by Type-C USB cable.
	Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port (PMOD1). 
	Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.
		
4. Software Requirements
	Renesas Flexible Software Package (FSP): Version 5.9.0
	e2 studio: Version 2025-04
	SEGGER J-Link RTT Viewer: Version 8.12f
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application
	
Note:
1. For the Serial Terminal application: The macro USE_VIRTUAL_COM is set to 1.
	1.1 Enable echo in Tera Term: Setup → Terminal… → check Local echo.
	1.2 Serial port settings:
	    - COM port: J-Link on-board
	    - Baud rate: 115200 bps
	    - Data length: 8 bits
	    - Parity: None
	    - Stop bit: 1
2. For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of sau_spi_master_notes.md.
