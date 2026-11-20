/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the RTC HAL module APIs. The project initializes the RTC
	module and allows the user to set RTC calendar time and calendar alarm. The user can enable periodic interrupt
	and can also view the current RTC calendar time. On enabling periodic interrupt, on-board LED toggles every 1
	second and on occurrence of calendar alarm, LED is turned ON.

2. Hardware Requirements:
	Supported RA boards: CK-RA6M5, EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2L1, EK-RA2L2, EK-RA4C1, EK-RA4E2, EK-RA4L1,
	                     EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G,
	                     EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8D2, EK-RA8E2, EK-RA8M1, EK-RA8M2, EK-RA8P1, FPB-RA0E1,
	                     FPB-RA0E2, FPB-RA0L1, FPB-RA2E3, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, MCK-RA8T2, FPB-RA4E2, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the PC using a USB cable for EP programming and debugging.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For FPB-RA0L1:
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2,
		3-4, 5-6, 7-8 to use the on-board debug functionality.

	For FPB-RA2E3:
		The user must close E25, E28, E46, E47 to use J-Link OB VCOM for serial terminal (UART).

	For FPB-RA8E1:
		The user must open jumpers J6 and J9 to use the on-board debug functionality.

Note:
1) The user is expected to enter data not exceeding 15 bytes in size.

2) Operation is not guaranteed for any user input value other than integer, char (e.g., float, special char)
   or incorrect format through the terminal.

3) As per the HW manual, the Year value ranges from 0 to 99, and the RTC supports a 100-year calendar
   from 2000 to 2099.

4) The user is expected to set RTC calendar Date and Time (option 1) before other operations.

5) The user is expected to enter Time values to set calendar date and time (Option 1) as shown below.
	Sample input: 23:10:2019 16:14:55 (Format: DD:MM:YYYY HH:MM:SS)

6) For RTC: The user is expected to enter Second values to set calendar alarm date and time (Option 2) as shown below.
	Sample input: 35 (Format: SS).
   For RTC_C: The user is expected to enter Hour and Minute values to set calendar alarm date and time (Option 2) as
   shown below.
	Sample input: 15:24 (Format: HH:MM).

7) For RTC_C: Alarm and periodic interrupts are retained after pressing the reset pin.

8) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
