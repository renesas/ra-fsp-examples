/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of OSPI_B driver on Renesas RA MCUs based on
	Renesas FSP. EP performs read/write/erase operation in SPI mode (1S-1S-1S) or DOPI mode (8D-8D-8D)
	based on the user input. The information and error messages are displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA8M1, EK-RA8D1, EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2, EK-RA8T2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

4. Hardware Configurations:
	- For EK-RA8P1: Set the configuration switches (SW4) as below to avoid potential failures.
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	| SW4-1 PMOD1_SEL0 | SW4-2 PMOD1_SEL1 |    SW4-3 OSPI   |   SW4-4 ARDUINO  |    SW4-5 I3C   |    SW4-6 MIPI  | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+
	|       OFF        |       OFF        |       OFF       |       OFF        |      OFF       |      OFF       |       OFF        |       OFF        |
	+------------------+------------------+-----------------+------------------+----------------+----------------+------------------+------------------+

	- For EK-RA8D2: Set the configuration switches (SW4) as below to avoid potential failures.
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
	| SW4-1 PMOD1 | SW4-2 PMOD1 | SW4-3 Octo-SPI | SW4-4 Arduino/mikroBUS | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS | SW4-8 USBHS |
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+
	|     OFF     |     OFF     |      OFF       |          OFF           |    OFF    |    OFF     |     OFF     |     OFF     |
	+-------------+-------------+----------------+------------------------+-----------+------------+-------------+-------------+

	- For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on
	                pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.

	- For EK-RA8T2: Set the configuration switches (SW6 and SW4) as below to use the on-board debug functionality.
	+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
	| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
	+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
	|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
	+-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+

	+-------+-------+-------+-------+-------+-------+-------+-------+
	| SW4-1 | SW4-2 | SW4-3 | SW4-4 | SW4-5 | SW4-6 | SW4-7 | SW4-8 |
	+-------+-------+-------+-------+-------+-------+-------+-------+
	|  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |
	+-------+-------+-------+-------+-------+-------+-------+-------+

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
