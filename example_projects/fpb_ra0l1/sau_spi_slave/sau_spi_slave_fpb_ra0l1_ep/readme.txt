/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic usage of the SAU SPI driver in slave configuration on Renesas RA
	MCUs using Renesas FSP. The project follows a command protocol similar to that of the MAX31723 sensor. During
	the communication, the MCU#1, which is running the sau_spi_slave_ep project, uses an IRQ to detect the slave
	select pin signal of MCU#2, which is running the sau_spi_master_ep project, and MCU#1 reads commands and data
	sent from MCU#2 when the signal transitions from LOW to HIGH. When MCU#2 sends a "read data" command, MCU#1
	reads the internal die temperature using the ADC and sends the temperature back to the master MCU#2. The
	temperature values are continuously printed to the terminal to verify correct data transmission. Additionally,
	status information and error messages are displayed on the terminal during execution.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3.
	2 x Renesas RA boards.
	2 x USB cables for programming and debugging (USB cable type varies by board model).
	4 x Jumper wires with both ends male.
	1 x Host PC.

4. Hardware Connections:
	Connect the RA boards' USB debug ports to the host PC using the appropriate USB cables for EP programming and
	debugging.
	Connect 2 RA boards together as shown below:

	For FPB-RA0E1:
	---------
		The soldering bridge E4 must be closed.
		Populate R10 with a resistor ranging from 0 to 150 ohms.

		|   Board 1 (Master)  |   Board 2 (Slave)   |
		|---------------------+---------------------|
		| SO00  P101 (J2:23)  | SI00  P100 (J2:24)  |
		| SI00  P100 (J2:24)  | SO00  P101 (J2:23)  |
		| SCK00 P102 (J2:22)  | SCK00 P102 (J2:22)  |
		| P103 (J2:21)        | IRQ5  P103 (J2:21)  |

	For FPB-RA0E2:
	---------
		The user must close E46, E47 to use J-Link OB VCOM for Serial Terminal (UART).

		|   Board 1 (Master)  |   Board 2 (Slave)   |
		|---------------------|---------------------|
		| SO01  P206 (J1:22)  | SI01  P205 (J1:23)  |
		| SI01  P205 (J1:23)  | SO01  P206 (J1:22)  |
		| SCK01 P204 (J1:24)  | SCK01 P204 (J1:24)  |
		| P207 (J1:21)        | IRQ5  P207 (J1:21)  |

	For FPB-RA0L1:
	---------
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
		The user must close E28, E29 to use J-Link OB VCOM for Serial Terminal (UART).

		|   Board 1 (Master)  |   Board 2 (Slave)   |
		|---------------------+---------------------|
		| SO00  P101 (J2:11)  | SI00  P100 (J2:12)  |
		| SI00  P100 (J2:12)  | SO00  P101 (J2:11)  |
		| SCK00 P102 (J2:10)  | SCK00 P102 (J2:10)  |
		| P103 (J2:9)         | IRQ5  P103 (J2:9)   |

	For FPB-RA0E3:
	---------
		The user must turn OFF S3 to use the on-board debug functionality.

		|   Board 1 (Master)  |   Board 2 (Slave)   |
		|---------------------+---------------------|
		| SO11  P213 (J1:4)   | SI11  P212 (J1:5)   |
		| SI11  P212 (J1:5)   | SO11  P213 (J1:4)   |
		| SCK11 P201 (J1:8)   | SCK11 P201 (J1:8)   |
		| P112 (J2:4)         | IRQ2  P112 (J2:4)   |

Note:
1) Slave and Master temperature values may differ slightly due to conversion of Slave readings to the MAX31723
   register format.

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

	d. To show the degree Celsius symbol (°C), the user needs to select the "Consolas" font through:
		[Setup] -> [Font...] -> [Font:]

3) For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of
   sau_spi_slave_notes.md.
