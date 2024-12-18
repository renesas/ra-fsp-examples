/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the SAU UART HAL module APIs. The project initializes the SAU UART module 
	with 115200 bps baud rate, 8-bits data length, no parity check, and one-bit stop.
	
	On power-up, after initializing the SAU UART module, a message requesting the user to open the Tera Term application on the PC along 
	with the configuration parameters for the Tera Term application will be displayed on the RTT Viewer. The configuration parameters are 
	as follows: The COM port is facilitated by the PmodUSBUART, operating at a baud rate of 115200 bps, with a data length of 8 bits,
	no parity check, one-bit stop, and without any flow control.
	
	After opening and configuring the Tera Term application, the user can enter any ASCII character string with a length less than 256 characters into 
	the Tera Term application, then press enter-key to send it to the RA board. The RA Board will echo any data received back to the Tera Term 
	application on the host PC.
	
	The PmodUSBUART facilitates the conversion of UART signals to USB signals and vice versa, enabling communication between the RA board and the Tera Term application.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.7.0
	e2 studio: Version 2024-10
	SEGGER J-Link RTT Viewer: Version 8.10f
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application 

3. Hardware Requirements:
	1 x Renesas FPB-RA0E1 board
	1 x USB type C cable for programming and debugging
	1 x PmodUSBUART(Part number: 410-212) module for converting data from the USB interface to the UART interface and vice versa.
	1 x Micro USB cable for connecting the PmodUSBUART to the host PC.

4. Hardware Connections:
	Plug in the PmodUSBUART Board (J2) to the PMOD1 connector on the MCU evaluation kit, and be careful to align Pin 1 on the module to Pin 1 on the MCU kit.
	Connect the PmodUSBUART Board (J1) to the host PC via a micro USB cable.
	Connect the USB Debug port on the RA board to the host PC via a USB type C cable.

For Segger RTT block address:
1) The Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block addresses for hex files committed in repository are as follows:
   a. e2studio: 0x20004514
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please locate the block address (pertaining to the variable named _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release)
