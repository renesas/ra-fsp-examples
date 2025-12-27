/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic usage of the SAU SPI driver in slave configuration on Renesas RA MCUs
	using Renesas FSP. The project follows a command protocol similar to that of the MAX31723 sensor. During the
	communication, the MCU#1, which is running the sau_spi_slave_ep project, uses an IRQ to detect the slave select pin
	signal of MCU#2, which is running the sau_spi_master_ep, and MCU#1 reads commands and data sent from MCU#2 when the
	signal transitions from LOW to HIGH. When MCU#2 sends a "read data" command, MCU#1 reads the internal die temperature
	using the ADC and sends the temperature back to the master MCU#2. The temperature values are continuously printed to
	the terminal to verify correct data transmission. Additionally, status information and error messages are displayed on
	the terminal during execution.

	Note:  
	- Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, 
	  depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer;
	  if supported, it defaults to the serial terminal (UART).
	- To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please refer to the instructions provided in 
	  the "Special Topic" section of sau_spi_slave_notes.md.
	- RA boards support J-Link OB VCOM: FPB-RA0E1, FPB-RA0L1.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	SEGGER J-Link RTT Viewer: Version 8.92
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1, FPB-RA0L1
	2 x Renesas RA boards.
	2 x Type-C USB cables.
	4 x Jumper wires with both ends male.
	1 x Host PC.

4. Hardware Connections:
	Connect 2 RA boards to Host PC by 2 Type-C USB cables.
	Connect 2 RA boards together as shown below:

	For FPB-RA0E1:
	---------
		|   Board 1 (Master)  |   Board 2 (Slave)   |
		|---------------------+---------------------|
		| SO00  P101 (J2:23)  | SI00  P100 (J2:24)  |
		| SI00  P100 (J2:24)  | SO00  P101 (J2:23)  |
		| SCK00 P102 (J2:22)  | SCK00 P102 (J2:22)  |
		| P103 (J2:21)        | IRQ5  P103 (J2:21)  |

	For FPB-RA0L1:
	---------
		|   Board 1 (Master)  |   Board 2 (Slave)   |
		|---------------------+---------------------|
		| SO00  P101 (J2:11)  | SI00  P100 (J2:12)  |
		| SI00  P100 (J2:12)  | SO00  P101 (J2:11)  |
		| SCK00 P102 (J2:10)  | SCK00 P102 (J2:10)  |
		| P103 (J2:9)         | IRQ5  P103 (J2:9)   |
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
		The user must close E28, E29 to use J-Link OB VCOM for Serial Terminal (UART).

Note:
1) Slave and Master temperature values may differ slightly due to conversion of Slave readings to the MAX31723 register format.

2) For the Serial Terminal application: The macro USE_VIRTUAL_COM is set to 1.
	1.1 Enable echo in Tera Term: Setup → Terminal… → check Local echo.
	1.2 Serial port settings:
		- COM port: J-Link on-board
		- Baud rate: 115200 bps
		- Data length: 8 bits
		- Parity: None
		- Stop bit: 1
	1.3 To show the degree Celsius symbol (°C), the user needs to select the "Consolas" font through:
            [Setup] -> [Font...] -> [Font:]

3) For using the J-Link RTT Viewer:
   If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

4) For GCC ARM Embedded Toolchain: To enable printing floats to the RTT Viewer, edit the project settings and ensure that
   "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf

5) For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of sau_spi_slave_notes.md.
