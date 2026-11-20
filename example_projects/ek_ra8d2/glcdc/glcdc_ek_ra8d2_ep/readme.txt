/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionalities of the GLCDC device driver on Renesas RA MCUs
	based on Renesas FSP. On successful initialization of the GLCDC module, the EP will read the display resolution
	and frame buffer set in the GLCDC module and draw color bands to cover the entire screen of external LCD
	connected to RA MCU. The user can view raw image in e2studio IDE. Additionally, the EP information and any
	failure messages will be displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M3G, EK-RA8D1, EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8P1 V2.
	1 x Renesas RA Board.
	1 x Graphics Expansion Board:
		For EK-RA6M3G, EK-RA8D1: Graphics Expansion Board (P/N: RTK7EK6M3B00001BU).
		For EK-RA8E2: Graphics Expansion Board (P/N: RTKAPPLCDPS02001BE).
		For EK-RA8P1, EK-RA8D2, EK-RA8P1 V2: Graphics Expansion Board (P/N: RTKLCDPAR1S00001BE).
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	For EK-RA8D1: 1 x Renesas APP-LCD-CONV1 board or Jumper wires used to connect Graphics Expansion Board to the
	              RA board.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA6M3G:
		- Connect Graphics Expansion Board to J1 of the RA board. Please refer to section 8.1 in EK-RA6M3G - User Manual
		  https://www.renesas.com/us/en/document/mat/ek-ra6m3g-v1-users-manual?r=1168091

	For EK-RA8D1:
		- Set the configuration switches (SW1) as below to avoid potential failures.
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     ON     |     ON      |    OFF    |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

		- If the user uses Renesas APP-LCD-CONV1 board:
			Connect Graphics Expansion Board to APP-LCD-CONV1.
			Connect APP-LCD-CONV1 Board to J57.

		- If the user does not have Renesas APP-LCD-CONV1 board:
			The user can use Jumper wires to connect Graphic Expansion Board to EK-RA8D1 board manually,
			please refer "Hardware Connection" section in glcdc_notes.md to connect correctly.
	For EK-RA8E2:
		- Connect J1 on Graphics Expansion Board to J1 of the RA board. Please carefully align pin 2 on the Graphic
		  Expansion Board with pin 1 on J1 of the RA board.

	For EK-RA8P1, EK-RA8D2, EK-RA8P1 V2:
		- Connect J1 on Graphics Expansion Board to J1 of the RA board. Please carefully align pin 1 on the Graphic
		  Expansion Board with pin 1 on J1 of the RA board.
		- For EK-RA8P1 V2:
		  The user must set the configuration switches (SW6) as below to use the on-board
		  debug functionality.
		  +-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		  | SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
		  +-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+
		  |  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |   ON   |
		  +-------+-------+-------+-------+-------+-------+-------+-------+-------+--------+

Note:
1) The user can also view the "Raw Image" in memory monitor of the e2studio IDE using the address of g_framebuffer
   and specifying the resolution.
   For detailed information, please refer to the "Verifying Operation" section in glcdc_notes.md.

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
