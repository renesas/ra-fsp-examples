/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionalities of the USB Host Communication Device Class (HCDC)
	driver on Renesas RA MCUs using two RA boards. Board 1, running the USB HCDC example project, communicates
	with Board 2, running the USB PCDC example project. Board 1 initiates the communication by sending commands
	to Board 2, which responds with the requested data. Board 1 then prints the received data on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA4L1, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1,
	                     EK-RA8D2, EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, MCK-RA8T1, MCK-RA8T2.
	2 x Renesas RA boards.
		1 x RA board running the USB HCDC example project (Board 1).
		1 x RA board running the USB PCDC example project (Board 2).
	3 x USB cables (USB cable type varies by board model).
		2 x USB cables for programming and debugging.
		1 x USB cable for USB communication between Board 1 and Board 2.
	1 x OTG cable used with the USB communication cable between Board 1 and Board 2 (OTG cable type varies by
        board model).

3. Configuration Settings:
	To select USB Speed in configuration:
	i. Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1,
			    EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8D2, EK-RA8M2)
		USB Speed         :    Full Speed
		USB Module Number :    USB_IP0 Port

	ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8D2, EK-RA8M2)
		USB Speed         :    Hi Speed
		USB Module Number :    USB_IP1 Port

4. Hardware Connections:
	Connect USB debug ports of the two RA boards to USB ports of the host PC using two appropriate USB cables for
	EP programming and debugging.

	Connect Board 1, running the USB HCDC example project, to Board 2, running the USB PCDC example project,
	using the appropriate OTG cable and USB communication cable.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Jumper J12: Connect pins 1-2.
		Remove jumper J15 pins.
		Connect the Full-Speed port (J11) of Board 1 to Board 2 using the appropriate OTG cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9: Connect pins 1-2.
		Remove jumper JP10 pins.
		Connect the Full-Speed port (CN14) of Board 1 to Board 2 using the appropriate OTG cable.

	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB host mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect the Full-Speed port (J11) of Board 1 to Board 2 using the appropriate OTG cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2 (Full-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect the Full-Speed port (J11) of Board 1 to Board 2 using the appropriate OTG cable.

	For MCK-RA8T2 (Full-Speed):
		Jumper JP6: Connect pins 1-2.
		Remove jumper JP7 pins.
		Connect the Full-Speed port (cn18) of Board 1 to Board 2 using the appropriate OTG cable.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Jumper J7: Connect pins 1-2.
		Remove jumper J17 pins.
		Connect the High-Speed port (J6) of Board 1 to Board 2 using the appropriate OTG cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
		Jumper J7: Connect pins 1-2.
		Remove jumper J17 pins.
		Connect the High-Speed port (J31) of Board 1 to Board 2 using the appropriate OTG cable.

	For EK-RA8P1, EK-RA8D2, EK-RA8M2 (High-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect the High-Speed port (J7) of Board 1 to Board 2 using the appropriate OTG cable.

Note:
1) By default, the USB HCDC example project (e.g., Board 1) runs in Full-Speed mode.

2) Both USB HCDC and USB PCDC need to be running at Full-Speed or High-Speed for the EP functionality
   to work correctly.

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
