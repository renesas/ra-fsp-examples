/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the SAU UART HAL module APIs. The project initializes the
	SAU UART module with 115200 bps baud rate, 8-bits data length, no parity check, and one-bit stop.
	
	On power-up, after initializing the SAU UART module, a message requesting the user to open the Tera Term
	application on the PC along with the configuration parameters for the Tera Term application will be displayed
	on the RTT Viewer. The configuration parameters are as follows: The COM port is facilitated by the Pmod USBUART,
	operating at a baud rate of 115200 bps, with a data length of 8 bits, no parity check, one-bit stop, and without
	any flow control.
	
	After opening and configuring the Tera Term application, the user can enter any ASCII character string with
	a length less than 256 characters into the Tera Term application, then press enter-key to send it to the RA board.
	The RA Board will echo any data received back to the Tera Term application on the host PC.
	
	The Pmod USBUART facilitates the conversion of UART signals to USB signals and vice versa, enabling communication
	between the RA board and the Tera Term application.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.1.0
	e2 studio: Version 2025-07
	SEGGER J-Link RTT Viewer: Version 8.58
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Pmod USBUART (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/) module for
	    converting data from the USB interface to the UART interface and vice versa.
	1 x Micro USB cable for connecting the Pmod USBUART to the host PC.
	Some jumper wires.

4. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.
	Connect Pmod USBUART to the RA board and the host PC as shown below:
	For FPB-RA0E1:
	------- 
		- Plug in the Pmod USBUART Board (J2) to the PMOD1 connector on the MCU evaluation kit, and be careful
		  to align Pin 1 on the module to Pin 1 on the RA board.
		- Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.

	For FPB-RA0E2:
	------- 
   		- Connect TXD2 P402 (J1:3) to RXD (J2:2) of Pmod USBUART.
    		- Connect RXD2 P403 (J1:4) to TXD (J2:3) of Pmod USBUART.
		- Connect GND (J1:8) to GND (J2:5) of Pmod USBUART.
		- Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.

	For FPB-RA0L1:
	------- 
		- Plug in the Pmod USBUART Board (J2) to the PMOD1 (J19) connector on the MCU evaluation kit, and be careful
		  to align Pin 1 on the module to Pin 1 on the RA board.
		- Connect the Pmod USBUART Board (J1) to the host PC via a micro USB cable.
		- The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.

Note:
1) For Tera Term application:
   a. To echo back characters typed in Tera Term, the user needs to enable it through:
      [Setup] -> [Terminal...] -> Check "Local echo"
   b. To make sure the received data shows properly, the user needs to set it through:
      [Setup] -> [Terminal...] -> [Receive: CR + LF]

2) The Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block addresses for hex files committed in repository are as follows:
   a. e2studio: 0x200048d8
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
