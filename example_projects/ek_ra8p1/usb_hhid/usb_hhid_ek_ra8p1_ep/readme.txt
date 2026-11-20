/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionalities of USB Host Human Interface Device Class (HHID)
	driver with FreeRTOS on Renesas RA MCUs based on Renesas FSP. The USB HHID driver configures keyboard as an
	HHID device. On pressing a key from the keyboard, the received keycode is decoded and displayed on the
	terminal. Additionally, error and info messages will also be printed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3, EK-RA6M3G, EK-RA8M1, EK-RA8D1,
	                     MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8D2, EK-RA8M2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x OTG cable used with the USB communication cable between the RA board and the wired USB keyboard (OTG cable
	type varies by board model).
	1 x Wired USB keyboard (e.g., HP PN: 803181-001).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3, EK-RA6M3G, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Jumper J12 placement is pins 1-2.
		Remove jumper J15 pins.
		Connect a wired USB keyboard to J11 connector using the appropriate OTG cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9 placement is pins 1-2.
		Remove jumper JP10 pins.
		Connect a wired USB keyboard to CN14 connector using the appropriate OTG cable.

	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB host mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect a wired USB keyboard to J11 connector using the appropriate OTG cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2 (Full-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29
		on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect a wired USB keyboard to J11 connector using the appropriate OTG cable.

	For MCK-RA8T2 (Full-Speed):
		Jumper JP6 placement is pins 1-2.
		Remove jumper JP7 pins.
		Connect a wired USB keyboard to CN18 connector using the appropriate OTG cable.

Note:
1) The user is requested to enter a key from a–z, A–Z, 0–9, and special characters (e.g., !, @, #, $, %, ^, &, *, (, ))
   on keyboard.

2) The EP has been confirmed to work properly with the HP brand keyboard (HP PN: 803181-001), while some other keyboard
   models may not be compatible.

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
