/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/
1. Project Overview:
	The example project demonstrates the basic functionalities of SDRAM on Renesas RA MCUs based on Renesas FSP.
	This example project transfers data to the entire SDRAM using DMAC. It demonstrates SDRAM settings and data
	retention by enabling self-refresh in Low Power Mode (LPM). When the Ultra-Low-Power Timer (ULPT) expires,
	the MCU returns to normal mode and reads the SDRAM to ensure the data is still retained. The EP information
	and error messages are displayed on the terminal. LED1 is turned ON for 1 second before the MCU enters LPM,
	then turned OFF. Additionally, LED3 is turned ON if an error occurs.

2. Hardware Requirements:
	Supported RA boards: EK-RA8D1, EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8T2.
	1 x Host PC.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming
	and debugging.

   	For EK-RA8D1:
		Set the configuration switches (SW1) as below.
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     OFF    |     ON      |    OFF    |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

	For EK-RA8T2:
		The user must set the configuration switches (SW6 and SW4) as below to use the on-board debug functionality.
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
