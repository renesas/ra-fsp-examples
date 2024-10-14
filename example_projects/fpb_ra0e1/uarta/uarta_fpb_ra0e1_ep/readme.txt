/***********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
***********************************************************************************************************************/

1. Project Overview:
	The Example Project demonstrates the basic functionality of the UARTA driver on Renesas RA MCUs based on FSP, 
	the Virtual COM is used as example usage of the UARTA. 
	The user can select UARTA baud rate from the menu on J-Link RTT Viewer (e.g., 9600 bps, 14400 bps, 19200 bps,...). 
	After the UARTA is initialized successfully, a message prompts the user to open and configure parameters for Tera Term on PC. 
	Then the user can enter and send an ASCII string (up to 256 characters) from Tera Term to the RA board,
	which will also be displayed on J-Link RTT Viewer and echoed back to Tera Term by the RA board.
	Additionally, the EP information and any error messages will be displayed on J-Link RTT Viewer.
	
2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.5.0
	e2 studio: Version 2024-07
	SEGGER J-Link RTT Viewer: Version 7.98b
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application 

3. Hardware Requirements:
	Supported RA board: FPB-RA0E1
	1 x RA board
	1 x USB type C cable for programming and debugging

4. Hardware Connections:
	To use J-Link OB VCOM:
		1) Short the E4 trace.
	 	2) Replace R10 by 150ohm resistor. 
	Connect the USB Debug port on the RA board to the host PC via a USB type C cable.

5. Verifying Operation:
	1. Import, generate, and build the example project.
	2. Connect the RA board to the host PC via USB cable.
	3. Flash or debug the example project.
	4. Open J-Link RTT Viewer on the host PC.
	5. After the main menu is displayed on J-Link RTT Viewer, the user can select baud rate option as desired.
		Type '1' to select baud rate = 9600 bps.
		Type '2' to select baud rate = 14400 bps.
		Type '3' to select baud rate = 19200 bps.
		Type '4' to select baud rate = 38400 bps.
		Type '5' to select baud rate = 57600 bps.
		Type '6' to select baud rate = 115200 bps.
	6. Open the Tera Term application on the host PC and configure parameters as message was displayed on J-Link RTT Viewer.
		a) To echo back characters typed in Tera Term, the user needs to enable it through [Setup] -> [Terminal...] -> Check [Local echo].
		b) To make sure the received data shows properly, the user needs to set [Setup] -> [Terminal...] -> [Receive: CR + LF]
		c) The configuration parameters of the serial port on the terminal application are as follows:
			- COM port is provided by the J-Link on-board.
			- Baud rate: Base on the user's selection.
			- Data length: 8-bits  
			- Parity: none
			- Stop bit: 1-bit
			- Flow control: none
	7. Type any ASCII string (up to 256 characters) into the Tera Term, then press enter-key to send.
	8. View the output log on J-Link RTT Viewer and Tera Term.

Note:
1) The user can refer to the Hardware User's Manual and the FSP User's Manual on Baud rate section to check the baud rate calculation.

2) The Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block addresses for hex files committed in repository are as follows:
   a. e2studio: 0x20004964
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please locate the block address (pertaining to the variable named _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release)
