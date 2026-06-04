/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic usage of SAU SPI driver in master configuration on Renesas RA MCUs.
	The project writes commands to the slave digital thermometer module, and reads the temperature values using SAU
	SPI bus configured as master. The temperature values are continuously printed on terminal. Status information
	and error messages will be printed on terminal during the execution of the project.

	Note:
	- Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via
	  J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to
	  the SEGGER J-Link RTT Viewer. If supported, it defaults to the serial terminal (UART).
	- To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please refer to the instructions provided
	  in the "Special Topic" section in sau_spi_master_notes.md file.
	- RA boards supported for J-Link OB VCOM: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1.
	- In addition to communicating with the digital thermometer module, the example project can also communicate
	  with the sau_spi_slave EP. For more information, please refer to the document of the sau_spi_slave EP.

2. Software Requirements
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	SEGGER J-Link RTT Viewer: Version 9.42
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3
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
		The soldering bridge E4 must be closed.
		Populate R10 with a resistor ranging from 0 to 150 ohms.

	For FPB-RA0E2:
	---------
		Connect the RA board to the host PC using a Type-C USB cable.
		Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port.
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.
		The user must close E46, E47 to use J-Link OB VCOM for Serial Terminal (UART).

	For FPB-RA0L1:
	---------
		Connect the RA board to the host PC using a Type-C USB cable.
		Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port (J19).
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
		The user must close E28, E29 to use J-Link OB VCOM for Serial Terminal (UART).

	For FPB-RA0E3:
	---------
		The user must turn OFF S3 to use the on-board debug functionality.
		Connect PMOD MAX31723PMB1 (J1) to the RA board as below:
			Temperature sensor (J1:1) to P112 (J2:4)
			Temperature sensor (J1:2) to P213 (J1:4)
			Temperature sensor (J1:3) to P212 (J1:5)
			Temperature sensor (J1:4) to P201 (J1:8)
			Temperature sensor (J1:5) to GND  (J1:3)
			Temperature sensor (J1:6) to VCC  (J1:6)

Note:
1) FPB-RA0E3 limitations:
   a. Due to the limited RAM size (2 KB), Serial Terminal is not supported on FPB-RA0E3.
   b. Enabling printing floats to the RTT Viewer is not required and should be disabled to avoid RAM overflow.

2) For using the Serial Terminal (UART):
   a. To echo back what was typed in Tera Term, the user needs to enable it through:
	  [Setup] -> [Terminal...] -> Check [Local echo].
   b. The serial terminal setting parameters are as follows:
	  Select the COM port provided by the J-Link on-board.
	  Speed: 115200 bps
	  Data: 8 bit
	  Parity: none
	  Stop bits: 1 bit
	  Flow control: none
   c. To show the degree Celsius symbol (°C), the user needs to select the "Consolas" font through:
	  [Setup] -> [Font...] -> [Font:]

3) For using the J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
   
4) For GCC ARM Embedded Toolchain (except for FPB-RA0E3 board): To enable printing floats to the RTT Viewer, edit the
   project settings and ensure that "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf

5) For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of
   sau_spi_master_notes.md.
