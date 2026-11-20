/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates basic functionalities of Flash HP driver. This example project initializes
	the Flash HP and performs Code/Data flash operations (erase, blank check, write, read) on predefined block
	based on the user input. The information and error messages are displayed on the terminal.

2. Hardware Requirements:
	Supported RA boards: CK-RA6M5, EK-RA4E2, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3,
	                     EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1,
	                     MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, RSSK-RA6T1, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA4C1,
	                     FPB-RA4E2, FPB-RA6E2.
 	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For FPB-RA8E1:
		The user must open jumpers J6 and J9 to use the on-board debug functionality.

Note:   
1) The user is expected to enter data not exceeding 15 bytes in size.

2) Operation is not guaranteed for any user input value, other than integer (i.e. float, char, special char)
   through the terminal.

3) Entering any input after EXIT option will result in error trap. Not recommended.

4) The user should refer to the "RASC User Guide for MDK and IAR" section for instructions on using RASC for
   IAR and Keil via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-IAR-user-guide

5) After the example project is built successfully, the user should follow these steps in sequence to launch
   the debug session:
   - Open Renesas Device Partition Manager (In e2studio or RASC -> Run -> Renesas Debug Tools -> Renesas Device
     Partition Manager), then choose "Connection Type" as SWD.
   - Initialize device. The user can refer to "Setting up Hardware" section of R11AN0467 application note via link:
     https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33 to perform the action.
   - Launch the Debug session from the example project.
 
6) EK-RA4L1, EK-RA4C1 boards must have HOCO running and stable in order to program and erase flash.

7) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
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
