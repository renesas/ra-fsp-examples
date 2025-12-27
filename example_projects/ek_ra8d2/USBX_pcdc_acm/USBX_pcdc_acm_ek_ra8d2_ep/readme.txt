/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USBX Device CDC ACM module on Renesas RA MCUs based on
	Renesas FSP using AzureRTOS. The user will use an RA board as a CDC device which will be enumerated as a COM
	port when connected to the host PC. Once the board is connected, the user can provide input from the serial
	terminal. The user input will be echoed on the serial terminal. The application status messages will be
	displayed on the J-Link RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	SEGGER J-Link RTT Viewer: Version 8.92
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, 
	                     MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8M2, EK-RA8D2
	1 x Renesas RA board.
	2 x Type-C USB device cables.
	1 x PC with at least 2 USB ports (1 for debug and 1 for COM port).

4. Configuration Settings:
	To select USB Speed in configuration:
		i.  Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, MCK-RA8T1, EK-RA4L1, EK-RA8E2, 
		                 EK-RA8P1, MCK-RA8T2, EK-RA8M2, EK-RA8D2)
			USB Speed                     :    Full Speed
			USB Module Number             :    USB_IP0 Port

		ii. High-Speed: (For EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8M2, EK-RA8D2)
			USB Speed                     :    Hi Speed
			USB Module Number             :    USB_IP1 Port

5. Hardware Connections:
	For EK-RA6M1, EK-RA6M2 (Full Speed):
		1. Connect the RA board USB FS port (J9) to the PC via a Micro USB cable.
		2. Connect the RA board USB debug port (J11) to the PC via a Micro USB cable for EP debugging.

	For EK-RA6M4, EK-RA4M2, EK-RA4M3 (Full Speed):
		1. Jumper J12 placement is pins 2-3.
		2. Connect jumper J15 pins.
		3. Connect the RA board USB FS port (J11) to the PC via a Micro USB cable.

	For MCK-RA8T1 (Full Speed):
		1. Jumper JP9: Connect pins 2-3.
		2. Connect jumper JP10 pins.
		3. Connect the RA board USB FS port (CN14) to the PC via a Type-C USB cable.

	For EK-RA4L1 (Full Speed):
		1. Turn ON S4-4 to select USB device mode.
		2. Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		3. Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.
		4. Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2 (Full Speed):
		Note: For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		1. Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.
		2. Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

	For MCK-RA8T2 (Full Speed):
		1. Jumper JP6: Connect pins 2-3.
		2. Connect jumper JP7 pins.
		3. Connect the RA board USB FS port (CN18) to the PC via a Type-C USB cable.
		4. Connect the RA board USB debug port (CN13) to the PC via a Type-C USB cable for EP debugging.

	For EK-RA6M3 (High Speed):
		1. Jumper J7: Connect pins 2-3.
		2. Connect jumper J17 pins.
		3. Connect the RA board USB HS port (J6) to the PC via a Micro USB cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High Speed):
		Note: For EK-RA8D1, the user must turn OFF SW1-6 to use USBHS.
		1. Jumper J7: Connect pins 2-3.
		2. Connect jumper J17 pins.
		3. Connect the RA board USB HS port (J31) to the PC via a Micro USB cable.

	For EK-RA8P1, EK-RA8D2, EK-RA8M2 (High Speed):
		Note: For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
		and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		1. Connect the RA board USB HS port (J7) to the PC via a Type-C USB cable.
		2. Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

6. Verifying Operation: 
	To run this example project, the user needs to compile and download the example project to the evaluation kit,
	and then follow instructions below to observe the operation of the system.
	a) On the host PC, open "Device Manager". Expand "Ports (COM & LPT)", locate "USB Serial Device (COMxx)" 
	   and note down the COM port number for reference in the next step.
	b) Open the serial port of the device using Tera Term. When prompted, select the Serial option and choose the
	   COM port number noted in the above step.
		- To echo back characters typed in Tera Term, the user needs to enable it through:
		  	[Setup] -> [Terminal...] -> Check [Local echo].
		- The configuration parameters of the serial port on the terminal application are as follows:
			Baud rate: 115200 bps
			Data length: 8-bits
			Parity: none
			Stop bit: 1-bit
			Flow control: none
	c) Open the J-Link RTT Viewer to view the application status messages.

Note:
1) Need to update src code as below if the user changes USB Speed in rm_usbx_pcdc_descriptor.c file
	For High Speed:
	Comment USB_DT_CONFIGURATION, uncomment USB_DT_OTHER_SPEED_CONF in g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED]
	Uncomment USB_DT_CONFIGURATION, comment USB_DT_OTHER_SPEED_CONF in g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED]

	For Full Speed:
	Uncomment USB_DT_CONFIGURATION, comment USB_DT_OTHER_SPEED_CONF in g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED]
	Comment USB_DT_CONFIGURATION, uncomment USB_DT_OTHER_SPEED_CONF in g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED]

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000650
   b. Keil: 	Not Available
   c. IAR:  	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
