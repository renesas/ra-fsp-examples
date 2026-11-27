/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the functionalities of the HTTP Client within the DA16200 module, employing
	the FSP Integrated Onchip DA16XXX Middleware driver, and leveraging FreeRTOS on Renesas RA MCUs. EP uses the
	US159-DA16200EVZ Ultra Low Power Wi-Fi Pmod™ module to connect to an access point and communicate with a web
	server using HTTP methods (GET, PUT, POST). After sending each request, EP waits for the HTTP response and
	prints it to the terminal. Additionally, status and failure messages are displayed on the terminal.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA6M5, EK-RA2A2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2,
			     EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA8D1, EK-RA8M1, FPB-RA4E1,
			     FPB-RA6E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, EK-RA8P1, EK-RA4C1, MCK-RA8T2,
			     EK-RA8M2, EK-RA8D2, FPB-RA0L1, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Wi-Fi router Access Point (AP) with internet access.
		Note: Please ensure the Wi-Fi Access Point credentials are available before running the project.
	1 x US159-DA16200EVZ Ultra Low Power Wi-Fi Pmod™ (Part Number: US159-DA16200MEVZ) (SDK v3.2.9.2).
		Note: Please ensure the Pmod™ SDK is up-to-date before executing the project.
		      For SDK update instructions, please refer to Section 5, 6, and 7 in the provided link:
		      https://www.renesas.com/en/document/apn/da16200da16600-sdk-update-guide

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA4M1, EK-RA6M1, EK-RA6M2:
	---------
		Plug in the US159-DA16200MEVZ to the PMODA connector on the MCU evaluation kit and be careful to
		align Pin 1 on the module to Pin 1 on the MCU kit.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, FPB-RA6E2:
	---------
		For FPB-RA6E2:
			The user must close E6 and open E5 to use PMOD1 for UART.
		Plug in the US159-DA16200MEVZ to the PMOD1 connector on the MCU evaluation kit and be careful to
		align Pin 1 on the module to Pin 1 on the MCU kit.

	For EK-RA6M5, EK-RA2L1, EK-RA4E2, EK-RA6E2, EK-RA6M4, EK-RA8D1, EK-RA8M1, FPB-RA4E1, FPB-RA6E1,
	    MCK-RA4T1, MCK-RA6T3, EK-RA8P1, EK-RA8M2, EK-RA8D2:
	---------
		For EK-RA8M2:
			The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3,
			and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
		Plug in the US159-DA16200MEVZ to the PMOD2 connector on the MCU evaluation kit and be careful to
		align Pin 1 on the module to Pin 1 on the MCU kit.

	For MCK-RA8T2:
	---------
		Plug in the US159-DA16200MEVZ to the PMOD Type3A/6A (CN6) connector on the MCU evaluation kit and
		be careful to align Pin 1 on the module to Pin 1 on the MCU kit.

	For FPB-RA0L1:
	---------
		The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
		The user must close E34, E36, E35 and open E40 to use P101, P100, P013 on PMOD1.
		Plug in the US159-DA16200MEVZ to the PMOD1 (J19) connector on the MCU evaluation kit and be careful to
		align Pin 1 on the module to Pin 1 on the MCU kit.

	For EK-RA2A2:
	---------
		Connect
		1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P501/TXD3 (J1:1)
		2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P500/RXD3 (J4:19)
		3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (J18:6)
		4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (J4:1)
		5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P203/RESET (J3:9)
		Note: If using Pmod2 on EK-RA2A2, it is necessary to solder jumper Ex3.

	For MCK-RA6T2:
	---------
		Connect
		1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> PD05/TXD9 (CN10:3)
		2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> PD06/RXD9 (CN10:2)
		3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (CN6:32)
		4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (CN6:34)
		5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> PA12/RESET (CN6:12)

	For MCK-RA8T1:
	---------
		Connect
		1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P112/TXD0 (CN2:1)
		2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P113/RXD0 (CN1:33)
		3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (CN1:30)
		4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (CN1:23)
		5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P114/RESET (CN1:31)

	For EK-RA4C1:
	---------
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		The user must close E52 to use P305 for RST pin of PMOD2.
		Connect
		1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P115/TXD1 (J2:35)
		2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P608/RXD1 (J2:37)
		3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (J2:1)
		4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (J1:1)
		5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P305/RESET (J2:38)

Note:
1) Ensure that the Wi-Fi Access Point credentials are available before running the project.

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
