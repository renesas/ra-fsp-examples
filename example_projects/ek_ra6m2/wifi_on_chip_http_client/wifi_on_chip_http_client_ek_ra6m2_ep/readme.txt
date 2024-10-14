/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the functionalities of the HTTP Client within the DA16200 module, employing the FSP
	Integrated Onchip DA16XXX Middleware driver, and leveraging FreeRTOS on Renesas RA MCUs. EP uses the US159-DA16200EVZ
	Ultra Low Power Wi-Fi Pmod™ module to connect to an access point and communicate with a web server using HTTP methods
	(GET, PUT, POST). After sending each request, EP waits for the HTTP response and prints it to the RTT Viewer.
	Additionally, status and failure messages are displayed on the RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.5.0
	e2 studio: Version 2024-07
	SEGGER J-Link RTT Viewer: Version 7.98b
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware Requirements:
	Board supported: EK-RA6M5, EK-RA2A2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2,
	EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA8D1, EK-RA8M1, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1
	1 x Renesas board
	1 x Micro USB cable for programming, debugging and observing EP operations on RTT viewer (For MCK-RA4T1, MCK-RA6T2,
	MCK-RA6T3, MCK-RA8T1: Type C USB cable).
	1 x US159-DA16200EVZ Ultra Low Power Wi-Fi Pmod™ (Part Number: US159-DA16200MEVZ).
	1 x Wi-Fi router Access Point(AP) with internet access. Note: WiFi router AP's credentials should be known.

4. Hardware Connections:
	EK-RA4M1/EK-RA6M1/EK-RA6M2
	---------
	Plug in the US159-DA16200MEVZ to the Pmod A connector on the MCU evaluation kit and be careful to align Pin 1 on the module
	to Pin 1 on the MCU kit.
	Connect the USB Debug port on the RA board to the PC using a micro USB cable.

	EK-RA4M2/EK-RA4M3/EK-RA6M3/EK-RA6M3G
	---------
	Plug in the US159-DA16200MEVZ to the Pmod1 connector on the MCU evaluation kit and be careful to align Pin 1 on the module
	to Pin 1 on the MCU kit.
	Connect the USB Debug port on the RA board to the PC using a micro USB cable.

	EK-RA6M5/EK-RA2L1/EK-RA4E2/EK-RA6E2/EK-RA6M4/EK-RA8D1/EK-RA8M1/FPB-RA4E1/FPB-RA6E1/MCK-RA4T1/MCK-RA6T3
	---------
	Plug in the US159-DA16200MEVZ to the Pmod2 connector on the MCU evaluation kit and be careful to align Pin 1 on the module
	to Pin 1 on the MCU kit.
	Connect the USB Debug port on the RA board to the PC using a micro USB cable.
	(For MCK-RA4T1/MCK-RA6T3: using a type C USB cable).

	EK-RA2A2
	---------
	Connect
	1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P501/TXD3 (J1:P1)
	2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P500/RXD3 (J4:P19)
	3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (J18:P6)
	4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (J4:P1)
	5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P203/RESET (J3:P9)
	Note: If using Pmod2 on EK-RA2A2, it is necessary to solder jumper Ex3.
	Connect the USB Debug port on EK-RA2A2 to the PC using a micro USB cable.

	MCK-RA6T2
	---------
	Connect
	1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> PD05/TXD9 (CN10:P3)
	2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> PD06/RXD9 (CN10:P2)
	3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (CN6:P32)
	4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (CN6:P34)
	5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> PA12/RESET (CN6:P12)
	Connect the USB Debug port on MCK-RA6T2 to the PC using a type C USB cable.
	
	MCK-RA8T1
	---------
	Connect
	1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P112/TXD0 (CN2:P1)
	2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P113/RXD0 (CN1:P33)
	3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (CN1:P30)
	4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (CN1:P23)
	5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P114/RESET (CN1:P31)
	Connect the USB Debug port on MCK-RA8T1 to the PC using a type C USB cable.

Note :	  
1) The segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x1ffe0a74
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).


