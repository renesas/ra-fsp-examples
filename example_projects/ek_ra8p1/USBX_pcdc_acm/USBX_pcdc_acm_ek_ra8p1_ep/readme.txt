/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the basic functionality of USBX Device CDC ACM on Renesas RA MCUs based on
	Renesas FSP using Azure RTOS. The RA board acts as a CDC device which will be enumerated as a COM port when
	connected to the host PC. Once the board is connected, the user can provide input from the USBX PCDC ACM
	terminal. The user input will be echoed on the USBX PCDC ACM terminal. The application status messages will
	be displayed on the debug terminal.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1,
	                     MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8M2, EK-RA8D2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x USB cable for USB communication between the RA board and the host PC (USB cable type varies by board model).
	1 x Host PC with at least 2 USB ports (1 for debug, 1 for COM port).

4. Configuration Settings:
	To select USB Speed in configuration:
		i.  Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, MCK-RA8T1, EK-RA4L1, EK-RA8E2,
		                     EK-RA8P1, MCK-RA8T2, EK-RA8M2, EK-RA8D2)
			USB Speed        : Full Speed
			USB Module Number: USB_IP0 Port

		ii. High-Speed: (For EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8M2, EK-RA8D2)
			USB Speed         : Hi Speed
			USB Module Number : USB_IP1 Port

5. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming
	and debugging.

	For EK-RA6M1, EK-RA6M2 (Full-Speed):
		1. Connect the RA board USB FS port (J9) to the PC using the appropriate USB cable.

	For EK-RA6M4, EK-RA4M2, EK-RA4M3 (Full-Speed):
		1. Jumper J12 placement is pins 2-3.
		2. Connect jumper J15 pins.
		3. Connect the RA board USB FS port (J11) to the PC using the appropriate USB cable.

	For MCK-RA8T1 (Full-Speed):
		1. Jumper JP9: Connect pins 2-3.
		2. Connect jumper JP10 pins.
		3. Connect the RA board USB FS port (CN14) to the PC using the appropriate USB cable.

	For EK-RA4L1 (Full-Speed):
		1. Turn ON S4-4 to select USB device mode.
		2. Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		3. Connect the RA board USB FS port (J11) to the PC using the appropriate USB cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2 (Full-Speed):
		Note: For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		1. Connect the RA board USB FS port (J11) to the PC using the appropriate USB cable.

	For MCK-RA8T2 (Full-Speed):
		1. Jumper JP6: Connect pins 2-3.
		2. Connect jumper JP7 pins.
		3. Connect the RA board USB FS port (CN18) to the PC using the appropriate USB cable.

	For EK-RA6M3 (High-Speed):
		1. Jumper J7: Connect pins 2-3.
		2. Connect jumper J17 pins.
		3. Connect the RA board USB HS port (J6) to the PC using the appropriate USB cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
		Note: For EK-RA8D1, the user must turn OFF SW1-6 to use USBHS.
		1. Jumper J7: Connect pins 2-3.
		2. Connect jumper J17 pins.
		3. Connect the RA board USB HS port (J31) to the PC using the appropriate USB cable.

	For EK-RA8P1, EK-RA8D2, EK-RA8M2 (High-Speed):
		Note: For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		1. Connect the RA board USB HS port (J7) to the PC using the appropriate USB cable.

6. Verifying Operation:
	a) Import, generate, and build the example project (EP).
	b) Set up the hardware connections as mentioned in the Hardware Connections section.
	c) Download the EP to the RA board.
	d) To view the output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
		- To make sure the received data shows properly, the user needs to set:
			+ [Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR].

		- To echo back characters typed in Tera Term, the user needs to enable it through:
			+ [Setup] -> [Terminal...] -> Check [Local echo].

		- The configuration parameters of the serial port on the serial terminal application:
			+ Port: The port is provided by the J-Link On-Board.
			+ Speed: 115200
			+ Data: 8 bit
			+ Parity: none
			+ Stop bits: 1 bit
			+ Flow control: none
	e) On the host PC, open "Device Manager". Expand "Ports (COM & LPT)", locate "USB Serial Device (COMxx)",
	   and note the COM port number for use in the next step.
	f) Open another serial terminal application (e.g., Tera Term) for USBX PCDC ACM. When prompted, select the serial option
	   and select the COM port noted in the previous step.
		- To echo back characters typed in Tera Term, enable:
			+ [Setup] -> [Terminal...] -> Check [Local echo].

Note:
1) To change the USB speed, the user needs to update the source code in rm_usbx_pcdc_descriptor.c as shown below:
	For High-Speed:
	- Comment out USB_DT_CONFIGURATION and uncomment USB_DT_OTHER_SPEED_CONF in
	  g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED].
	- Uncomment USB_DT_CONFIGURATION and comment out USB_DT_OTHER_SPEED_CONF in
	  g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED].

	For Full-Speed:
	- Uncomment USB_DT_CONFIGURATION and comment out USB_DT_OTHER_SPEED_CONF in
	  g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED].
	- Comment out USB_DT_CONFIGURATION and uncomment USB_DT_OTHER_SPEED_CONF in
	  g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED].
