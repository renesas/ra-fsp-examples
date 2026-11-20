/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionality of the MRAM driver on Renesas RA MCUs using the
	Renesas Flexible Software Package (FSP). The user can interact with the application via the terminal to execute
 	write, read, and erase operations, starting at a fixed MRAM address. The operation status and data content
	are displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA8P1, EK-RA8D2, EK-RA8M2, MCK-RA8T2.
 	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

Note:
1) Initialize device back to factory default by using "Renesas Device Partition Manager" before run the example project
   as below:
   - Open Renesas Device Partition Manager (RDPM): On "e2studio" -> Run -> Renesas Debug Tools -> Renesas Device
     Partition Manager, then choose "Connection Type" as SWD.
   - Initialize the MCU: On RDPM, select only "Initialize device" option, then click "Run".
   - The user can refer to section "5.3 Setting up Hardware" in the document via link:
     https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

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
