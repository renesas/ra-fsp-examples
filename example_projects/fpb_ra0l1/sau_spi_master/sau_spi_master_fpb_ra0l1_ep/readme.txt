/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic usage of SAU SPI driver in master configuration on Renesas RA MCUs.
	The project writes commands to the slave digital thermometer module, and reads the temperature values using SAU SPI
	bus configured as master. The temperature values are continuously printed on terminal. Status information and error
	messages will be printed on terminal during the execution of the project.

2. Software Requirements
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1, FPB-RA0L1
	1 x Renesas RA board.
	1 x Type-C USB cable.
	1 x Digital thermometer module (PMOD MAX31723PMB1)
		Link Product: https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31723PMB1?qs=UmMSjoC1xtH8e742i4OoUA%3D%3D
		Manufacturer Product Number: MAX31723PMB1#
	1 x Host PC.
	
4. Hardware Connections:
	PMOD MAX31723PMB1:
	---------
	Select SPI communication protocol on the PMOD by configuring the following jumper configurations on JP1:
	Connect JP1-3 to JP1-5
	Connect JP1-4 to JP1-6
	Connect JP1-9 to JP1-10

	For FPB-RA0E1:
	---------
	Connect the RA board to the host PC using a Type-C USB cable.
	Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port.
	Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.

	For FPB-RA0L1:
	---------
	Connect the RA board to the host PC using a Type-C USB cable.
	Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port (J19).
	Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.
	The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
	The user must close E28, E29 to use J-Link OB VCOM for Serial Terminal (UART).

Note:
1. For the Serial Terminal application: The macro USE_VIRTUAL_COM is set to 1.
	1.1 Enable echo in Tera Term: Setup → Terminal… → check Local echo.
	1.2 Serial port settings:
	    - COM port: J-Link on-board
	    - Baud rate: 115200 bps
	    - Data length: 8 bits
	    - Parity: None
	    - Stop bit: 1
	1.3 To show the degree Celsius symbol (°C), the user needs to select the "Consolas" font through:
            [Setup] -> [Font...] -> [Font:]

2) For using the J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

3) For GCC ARM Embedded Toolchain: To enable printing floats to the RTT Viewer, edit the project settings and ensure that
   "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf

4) For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of sau_spi_master_notes.md.
