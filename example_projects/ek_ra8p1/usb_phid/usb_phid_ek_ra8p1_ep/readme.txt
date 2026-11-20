/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionalities of USB PHID driver on Renesas RA MCUs based on
	Renesas FSP. The project configures RA board as a keyboard. On pressing any key in the terminal, the MCU
	sends characters 'a' to 'z' followed by '1' to '9' and '0' to the host PC.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA4M3,
	                     EK-RA4M2, EK-RA8M1, EK-RA8D1, EK-RA4E2, EK-RA6E2, MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1,
	                     EK-RA2L2, MCK-RA8T2, EK-RA8D2, EK-RA8M2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x USB cable for USB communication between the RA board and the host PC (USB cable type varies by board model).
	1 x Host PC with at least 2 available USB ports (1 for debugging and 1 for the COM port).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2 (Full-Speed):
		Connect the RA board USB Full-Speed port (J9) to the host PC using the appropriate USB cable.

	For EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA4M3, EK-RA4M2, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Jumper J12: Connect pins 2-3.
		Connect jumper J15 pins.
		Connect the RA board USB Full-Speed port (J11) to the host PC using the appropriate USB cable.

	For EK-RA4E2, EK-RA6E2 (Full-Speed):
		Connect the RA board USB Full-Speed port (J11) to the host PC using the appropriate USB cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9: Connect pins 2-3.
		Connect jumper JP10 pins.
		Connect the RA board USB Full-Speed port (CN14) to the host PC using the appropriate USB cable.

	For EK-RA4L1 (Full-Speed):
		Turn ON S4-4 to select USB device mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect the RA board USB Full-Speed port (J11) to the host PC using the appropriate USB cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2 (Full-Speed):
		Connect the RA board USB Full-Speed port (J11) to the host PC using the appropriate USB cable.

	For EK-RA2L2 (Full-Speed):
		Connect the RA board USB Full-Speed port (J11) to the host PC using the appropriate USB cable.

	For MCK-RA8T2 (Full-Speed):
		Jumper JP6: Connect pins 2-3.
		Connect jumper JP7 pins.
		Connect the RA board USB Full-Speed port (CN18) to the host PC using the appropriate USB cable.

	For EK-RA8M2 (Full-Speed):
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect the RA board USB Full-Speed port (J11) to the host PC using the appropriate USB cable.

Note:
1) Open "Device Manager", and check "Human Interface Devices", to see "USB Input Device" has been detected.

2) Lower case a - z and numbers 0 - 9 are sent from the MCU to the host PC.

3) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
