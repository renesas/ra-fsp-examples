/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
   	The project demonstrates the basic functionalities of USB Host Vendor class driver on Renesas RA MCUs based
	on Renesas FSP. In this example, the application will configure the MCU as a Vendor Host device. This host
	device will be connected to the USB Peripheral Vendor device which is another RA board. After the enumeration
	is completed, the Vendor Host board will write 20 bytes of data to the Vendor Peripheral board and read back
	the same data from the Vendor Peripheral board. The Vendor Host and Vendor Peripheral applications are designed
 	to continuously transfer data between both the boards. The user will be able to see all the operation sequence
	and status on the serial terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1,
	                     EK-RA4L1, EK-RA8E2, EK-RA8P1, EK-RA8M2, EK-RA8D2, MCK-RA8T2.
	2 x Renesas RA boards.
		1 x RA board running the USB HVND example project (Board 1).
		1 x RA board running the USB PVND example project (Board 2).
	3 x USB cables (USB cable type varies by board model).
		2 x USB cables for programming and debugging.
		1 x USB cable for USB communication between Board 1 and Board 2.
	1 x OTG cable used with the USB communication cable between Board 1 and Board 2 (OTG cable type varies by
	  board model).

3. Configuration Settings:
	To select USB Speed in configuration:
		i.  Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2,
		                     EK-RA8P1, EK-RA8M2, EK-RA8D2, MCK-RA8T2)
		    USB Speed                     :    Full Speed
		    USB Module Number             :    USB_IP0 Port

		ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA8P1, EK-RA8M2, EK-RA8D2)
		    USB Speed                     :    Hi Speed
		    USB Module Number             :    USB_IP1 Port

4. Hardware Connections:
	Connect the USB debug ports of Board 1 and Board 2 to the host PC using the appropriate USB cables for EP
	programming and debugging.

	Connect Board 1, running the USB HVND example project, to Board 2, running the USB PVND example project,
	using the appropriate OTG cable and USB communication cable.

	The jumper and switch settings below are for Board 1 (USB HVND), the host board.

	For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1 (Full-Speed):
		Jumper J12: Connect pins 1-2.
		Remove jumper J15 pins.
		Connect the Full-Speed port (J11) of Board 1 to Board 2 using the appropriate OTG cable.

	For MCK-RA8T1 (Full-Speed):
		Jumper JP9: Connect pins 1-2.
		Remove jumper JP10 pins.
		Connect the Full-Speed port (CN14) of Board 1 to Board 2 using the appropriate OTG cable.

	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB host mode.
		Set J17 jumper to pins 2-3, set J7 jumper to use P407 for USBFS VBUS.
		Connect the Full-Speed port (J11) of Board 1 to Board 2 using the appropriate OTG cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8M2, EK-RA8D2 (Full-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect the Full-Speed port (J11) of Board 1 to Board 2 using the appropriate OTG cable.

	For MCK-RA8T2 (Full-Speed):
		Jumper JP6: Connect pins 1-2.
		Remove jumper JP7 pins.
		Connect the Full-Speed port (CN18) of Board 1 to Board 2 using the appropriate OTG cable.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Jumper J7: Connect pins 1-2.
		Remove jumper J17 pins.
		Connect the High-Speed port (J6) of Board 1 to Board 2 using the appropriate OTG cable.

	For EK-RA8P1, EK-RA8M2, EK-RA8D2 (High-Speed):
		For EK-RA8M2: The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Connect the High-Speed port (J7) of Board 1 to Board 2 using the appropriate OTG cable.

Note:
1) By default, the USB HVND example project runs in High-Speed mode if the RA board supports it, except for
   the EK-RA8D1 and EK-RA8M1.

2) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below.
   Each board has its own COM port, so open one terminal for each board:
	a. To make sure the received data shows properly, the user needs to set:
		[Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR].

	b. The configuration parameters of the serial port on the serial terminal application:
		Port: The port is provided by the J-Link On-Board.
		Speed: 115200
		Data: 8 bit
		Parity: none
		Stop bits: 1 bit
		Flow control: none
