/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionalities of the 2D Drawing Engine (DRW) device driver on
	Renesas RA MCUs based on Renesas FSP. On successful initialization of the DRW and GLCDC modules, the EP
	draws and fills various shapes with animation and alpha blending effects on an external LCD connected to
	the RA MCU. Additionally, the EP information and any failure messages will be displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M3G, EK-RA8D1, EK-RA8E2, EK-RA8P1, EK-RA8D2.
	1 x Renesas RA Board.
	1 x Graphics Expansion Board:
		For EK-RA6M3G: Graphics Expansion Board (P/N: RTK7EK6M3B00001BU).
		For EK-RA8D1: MIPI Graphics Expansion Board.
		For EK-RA8E2: Graphics Expansion Board (P/N: RTKAPPLCDPS02001BE).
		For EK-RA8P1, EK-RA8D2: Graphics Expansion Board (P/N: RTKLCDPAR1S00001BE).
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA6M3G:
		- Connect Graphics Expansion Board to the RA board.

	For EK-RA8D1:
		- Connect the MIPI Graphics Expansion Board to J58 of the RA board.
		- Set the configuration switches (SW1) on the EK-RA8D1 board as below:
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |      OFF     |     OFF    |      OFF   |     OFF    |      ON     |    OFF    |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

	For EK-RA8E2:
		- Connect J1 on Graphic Expansion Board to J1 of the RA board. Please carefully align pin 2 on the Graphic
		  Expansion Board with pin 1 on J1 of the RA board.

	For EK-RA8P1, EK-RA8D2:
		- Connect J1 on Graphics Expansion Board to J1 of the RA board. Please carefully align pin 1 on the Graphic
		  Expansion Board with pin 1 on J1 of the RA board.

Note: 
1) The user can also view the "Raw Image" in memory monitor of the e2studio IDE using the address of g_framebuffer
   and specifying the resolution. For detailed information, please refer to the "Verifying Operation" section in
   drw_notes.md.

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
