/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project showcases how the TML driver operates on Renesas RA MCUs. The demonstration includes 
	showcasing the TML timer with counter operation, capture operation in normal mode, and counter operation 
	in low power mode. In TML counter operation (normal mode), three TML instances (which are configured as 8-bit,
	16-bit, and 32-bit counter modes) operate for the input duration by the user. In TML 16-bit capture operation,
	user can trigger an ELC software event to get the raw counts value of 16-bit timer. In TML counter operation
	(low power modes), the 32-bit timer is configured as counter mode with input period time. Once the 32-bit timer
	has expired, the RA board is woken up from LPM to normal mode. The EP information and error messages will be 
	printed to the Host PC.

	Note: - To display information, users can select between the SEGGER J-Link RTT Viewer and
	        the Serial Terminal (UART) with J-Link OB VCOM.
	      - By default, EP information is printed to the host PC via the Serial Terminal.
	      - To utilize the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please follow the instructions
 	        in tml_notes.md file.

2. Hardware Requirements:
	Supported RA Boards: FPB-RA0E1, FPB-RA0E2
	1 x Renesas RA board.
	1 x Type-C USB cable.
	1 x Host PC.

3. Hardware Connections:
	Connect the RA board to the host PC via a Type-C USB cable.

    	For FPB-RA0E1:
	---------
		To use J-Link OB VCOM for Serial Terminal (UART):
       			- The soldering bridge E4 must be closed.
			- Populate R10 with a resistor ranging from 0 to 150 ohms.

	For FPB-RA0E2:
	---------
		To use J-Link OB VCOM for Serial Terminal (UART), the user must close E46, E47.

Note:
	For using the Serial Terminal (UART):
      	1) To echo back what was typed in Tera Term, the user needs to enable it through:
	   [Setup] -> [Terminal...] -> Check [Local echo].

      	2) The serial terminal setting parameters are as follows:
      		Select the COM port provided by the J-Link on-board.
      		Baud rate: 115200 bps
      		Data length: 8-bits
      		Parity: none
      		Stop bit: 1-bit
      		Flow control: none
