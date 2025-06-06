/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic usage of the SAU SPI driver in slave configuration on Renesas RA MCUs
	using Renesas FSP. The project follows a command protocol similar to that of the MAX31723 sensor. During the
	communication, the MCU#1, which is running the sau_spi_slave_ep project uses an IRQ to detect the slave select pin
	signal of MCU#2, which is running the sau_spi_master_ep, and MCU#1 reads commands and data sent from MCU #2 when the
	signal transitions from LOW to HIGH. When MCU #2 sends a "read data" command, MCU #1 reads the internal die temperature
	using the ADC and sends the temperature back to the master MCU #2. The temperature values are continuously printed to
	the terminal to verify correct data transmission. Additionally, status information and error messages are displayed on
	the terminal during execution.

	Note:  
	- Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, 
	  depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer;
	  if supported, it defaults to the serial terminal (UART).
	- To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please refer to the instructions provided in 
	  the 'Special Topic' section of sau_spi_slave_notes.md.

2. Hardware Requirements:
    Supported RA Boards: FPB-RA0E1
	2 x Renesas RA boards.
	2 x Type-C USB cables.
	4 x Jumper wires with both ends male.
	1 x Host PC.
	
3. Hardware Connections:
	FPB-RA0E1:
	---------
	Connect 2 RA boards to Host PC by 2 Type-C USB cables.
	Connect 2 RA boards together following below:
	|   Board 1 (Master)  |   Board 2 (Slave)   |
	|---------------------+---------------------|
	| P101 SO (J2-Pin23)  | P100 SI (J2-Pin24)  |
	| P100 SI (J2-Pin24)  | P101 SO (J2-Pin23)  |
	| P102 SCK (J2-Pin22) | P102 SCK (J2-Pin22) |
	| P103 (J2-Pin21)     | P103 IRQ (J2-Pin21) |
	
4. Software Requirements:
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
2. For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of sau_spi_slave_notes.md.

