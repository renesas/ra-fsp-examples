/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USB PHID + PHID Composite Device on Renesas RA MCUs based
	on Renesas FSP. The RA board is configured to function as either a Keyboard or a Mouse. On entering "k" from
	the terminal, the MCU will work as a Keyboard. The MCU sends characters a to z and numbers 0 to 9 to the host
	PC. On entering "m" from the terminal, the MCU will work as a Mouse. The MCU sends Mouse coordinates to the
	host PC, making the pointer move in a square pattern on the host PC screen.

2. Hardware Requirements:
	Supported RA boards: EK-RA2L2, EK-RA4L1, EK-RA6M2, EK-RA8D1, EK-RA8D2, EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1,
	                     MCK-RA8T1, MCK-RA8T2.
	1 x Renesas RA board.
	2 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Host PC with at least 2 USB ports (1 for debug and 1 for COM port).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA6M2:
		- Connect the RA board Full Speed port (J9) to the host PC using the appropriate USB cable for EP programming.

	For EK-RA8M1, EK-RA8D1:
		- Jumper J12: Connect pins 2-3.
		- Connect the jumper J15 pins.
		- Connect the RA board Full Speed port (J11) to the host PC using the appropriate USB cable for EP programming.

	For MCK-RA8T1:
		- Jumper JP9: Connect pins 2-3.
		- Connect the jumper JP10 pins.
		- Connect the RA board Full Speed port (CN14) to the host PC using the appropriate USB cable for EP programming.

	For EK-RA4L1:
		- Turn ON S4-4 to select USB device mode.
		- Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		- Connect the RA board Full Speed port (J11) to the host PC using the appropriate USB cable for EP programming.

	For EK-RA8E2, EK-RA8P1, EK-RA8M2, EK-RA8D2:
		- For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		  and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		- Connect the RA board Full Speed port (J11) to the host PC using the appropriate USB cable for EP programming.

	For EK-RA2L2:
    		- Connect the RA board Full Speed port (J11) to the host PC using the appropriate USB cable for EP programming.

	For MCK-RA8T2:
    		- Jumper JP6: Connect pins 2-3.
    		- Connect Jumper JP7 pins.
    		- Connect the RA board Full Speed port (CN18) to the host PC using the appropriate USB cable for EP programming.

Note:
1) Open Device Manager, and check "Human Interface Devices", to see "USB Input Device" has been detected.

2) Open Notepad on the PC, lowercase characters a - z, and numbers 0 - 9 will be shown on the Notepad in case "k" (Keyboard) is chosen.

3) On the PC, Mouse will move square shape on the screen in case "m" (Mouse) is chosen.

4) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
