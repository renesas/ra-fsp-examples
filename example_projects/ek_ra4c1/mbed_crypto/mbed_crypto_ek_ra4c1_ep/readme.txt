/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of Mbed Crypto driver on Renesas RA MCUs based on
	Renesas FSP. The EP performs following Mbed Crypto operations:
	1. AES256 Key generation (for GCM Mode), Encryption and Decryption.
	2. SHA256 - setup and calculate hash, compare calculated hash value and length with their expected values.
	3. ECC - P256R1 Key pair generation, signing the hashed message with private key & verify hashed message with
	   public key.
	4. RSA 2048 Key pair generation, signing the hashed message with private key & verify hashed message with
	   public key.
	Error and info messages will be printed on the terminal.

	Note:
	1) EK-RA2L1, EK-RA4M1 and EK-RA4W1 boards do not support SHA, ECC, and RSA operations.
	2) EK-RA4L1, EK-RA4C1 boards do not support RSA operation.
	3) Some MCUs support hardware crypto operations, some MCUs support software crypto operations. 
	   Refer to configuration.xml -> Stacks -> MbedTLS (Crypto Only) -> Properties -> Settings -> Property ->
	   Common -> Hardware Acceleration configuration for the specific configurations for each example project.

2. Hardware Requirements:
	Supported RA boards: EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3,
	                     EK-RA6M3G, EK-RA6M4, EK-RA6M5, RSSK-RA6T1, EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1,
	                     EK-RA4L1, EK-RA8P1, EK-RA8E2, EK-RA4C1, EK-RA8M2, EK-RA8D2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

	For EK-RA8M2:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4,
		5-6, 7-8 to use the on-board debug functionality.

	For EK-RA8D2:
		The user must set the configuration switches (SW4) on the RA board as below:
		+------------------+------------------+------------+---------------+-----------+------------+------------------+------------------+
		| SW4-1 PMOD1_SEL0 | SW4-2 PMOD1_SEL1 | SW4-3 OSPI | SW4-4 ARDUINO | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
		+------------------+------------------+------------+---------------+-----------+------------+------------------+------------------+
		|       OFF        |       OFF        |     OFF    |      OFF      |    OFF    |     OFF    |       OFF        |       OFF        |
		+------------------+------------------+------------+---------------+-----------+------------+------------------+------------------+

Note:
1) After the example project is built successfully, the user should follow these steps in sequence to launch
   the debug session:
   - Start E2Studio -> Run -> Renesas Debug Tools -> Renesas Device Partition Manager (Available on e2studio),
     then choose "Connection Type" as SWD.
   - Initialize device. The user can refer to the "Setting up Hardware" section of R11AN0467 application note via link:
     https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33 to perform the action.
   - Launch the Debug session from the example project.

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
