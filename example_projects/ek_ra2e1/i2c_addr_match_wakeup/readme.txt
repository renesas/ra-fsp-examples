/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the I2C address match wakeup function on Renesas RA MCUs based on FSP.
	On the Master board, the user can select an I2C operation from the main menu, and then select the Slave address
        from the Slave address menu. If the user selects the correct Slave address, the I2C operation can be performed.
	Otherwise, it cannot be performed. If there is no I2C operation, the Slave board will enter to Software Standby
        mode with I2C wakeup enable after 5 seconds. Then, the user can select an I2C operation and the Slave address
        on the Master board to send I2C packet to the Slave board. If the Slave address matches, the Slave board will 
        wake up and the I2C operation can continue. Additionally, the user can press S1 on the Slave board to transition
        from Software Standby mode to Normal mode. The information and error messages will be printed on the terminal
        application during the execution of the project.

	In Normal mode, the board state LED (LED1) is turned ON. In Software Standby mode, the board state LED (LED1)
        is turned OFF. If any error had occurred, the board state LED (LED1) is turned OFF and the error LED (LED2)
        is turned ON (If the board supports at least 2 USER LEDs).

	For Terminal application:
	- To display information, the user can select between the SEGGER J-Link RTT Viewer and the Serial terminal
          (UART). It is important to note that the user should only operate a single terminal application at a time
	  to avoid conflicts or data inconsistencies.
	- By default, the EP information is printed to the host PC using the Serial terminal.
	- To utilize the SEGGER J-Link RTT Viewer instead of the Serial terminal, please refer to the first step in the
	  "Verifying Operation" section.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.9.0
	e2 studio: Version 2025-04
	SEGGER J-Link RTT Viewer: Version 8.12f
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA2E1
 	2 x Renesas RA boards.
	1 x Host PC.
	2 x Micro USB cables for programming and debugging.
	Some jumper wires.
	For RA boards that do not support J-Link OB VCOM: EK-RA2E1
		2 x USB to UART Converters (e.g., Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/).
		2 x Micro USB cables used to connect the Pmod USBUART to the RA board.

4. Hardware Connections:
	- Connect the USB debug port on 2 RA boards to the host PC via 2 Micro USB cables for programming and debugging.
	- Connect 2 Pmod USBUART modules to the host PC via 2 Micro USB cables.
	- For Pmod USBUART: Jumper JP1 placement is LCL-VCC pins (The attached system board is powered independently
          from the PMOD USBUART).
	- Connect 2 Pmod USBUART modules to 2 RA boards as shown below:
		For EK-RA2E1:
			RA board		  Pmod USBUART
			RXD0 P206 (J2:15)    ---> TXD Pin (J2:3)
			TXD0 P205 (J2:16)    ---> RXD Pin (J2:2)
			GND (J2:40)	     ---> GND Pin (J2:5)		

	- Connect the Master board to the Slave board as shown below:
		For EK-RA2E1:
			Master board                Slave board
			SDA0 P407 (J24:9)    --->   SDA0 P407 (J24:9)
			SCL0 P408 (J24:10)   --->   SCL0 P408 (J24:10)

5. Verifying Operation:
	a. Import, generate, and build the i2c_master_addr_match_wakeup and i2c_slave_addr_match_wakeup projects.
	- By default, the EP supports Serial terminal
		Define USE_SERIAL_TERM=1 macro in:
                Project Properties -> C/C++ Build -> Settings -> Tool Settings -> GNU ARM Cross C Compiler -> Preprocessor
	- To use SEGGER J-Link RTT Viewer, please follow the instructions as shown below:
		Define USE_SERIAL_TERM=0 macro in:
	        Project Properties -> C/C++ Build -> Settings -> Tool Settings -> GNU ARM Cross C Compiler -> Preprocessor

	b. Set up the hardware properly according to the instructions mentioned in the "Hardware Connections" section.

	c. Download the i2c_master_addr_match_wakeup EP to Board1 to run in Master mode.
 	   Download the i2c_slave_addr_match_wakeup EP to Board2 to run in Slave mode.
	   Note: After finished downloading the EP using the debug option in e2studio, the user should terminate the session
                 before connecting to terminal application. The EP should be run in Standalone mode.

	d. Open Serial terminal application (default) or J-Link RTT Viewer on the host PC for 2 boards. After connecting successfully, 
           power cycle both boards to view the output status.

	e. On the Board1 (Master mode), after the main menu is displayed on the terminal application, the user selects option
           to perform I2C operation as desired.
		- Type '1' and enter to read message from the Slave board. The Slave address menu is displayed.
			+ Type '1' and enter to select Slave address = 0x4A. The I2C operation can be performed.
			+ Type '2' and enter to select Slave address = 0x4B. The I2C operation cannot be performed
                          due to the incorrect selected Slave address.
		- Type '2' and enter to write message to the Slave board. The Slave address menu is displayed.
			+ Type '1' and enter to select Slave address = 0x4A. The I2C operation can be performed.
			+ Type '2' and enter to select Slave address = 0x4B. The I2C operation cannot be performed
                          due to the incorrect selected Slave address.

        f. The Board2 (Slave mode) enters Software Standby mode with I2C wakeup enable after 5 seconds of I2C bus idle state.
	   The board state message is displayed on the terminal application. The user can wake up the Board2 by:
 		- Perform read or write operations on the Board1 after selecting correct Slave address (0x4A) to wake up by
                  I2C address match wakeup. If the user selects the incorrect Slave address (0x4B), the I2C operation cannot be
                  performed and the Board2 cannot wake up.
		- Press S1 on the Board2 to wake up by IRQ.

Note:
	1) It is important to note that the user should connect to either the terminal application or the debug session in e2studio
           at a time to avoid potential errors.
	
	2) For Serial terminal application:
	   a) To echo back what was typed in Tera Term, the user needs to enable it through:
	      [Setup] -> [Terminal...] -> Check [Local echo].
	   b) To ensure a new line is added after each input, the user needs to set transmit option through:
	      [Setup] -> [Terminal...] -> New-line -> Transmit: CR+LF.
	   C) The configuration parameters of the serial port on the terminal application are as follows:
		- COM port is provided by the Pmod USBUART or J-Link OB VCOM
		- Baud rate: 115200 bps
		- Data length: 8-bits
		- Parity: none
		- Stop bit: 1-bit
		- Flow control: none

	3) For SEGGER J-Link RTT Viewer:
	   a) The RTT console comes in handy for viewing the application messages while running or debugging the application.
	      To use the RTT console, the debugger script for the LPM must be selected as shown below:
	      Open J-Link RTT Viewer console -> File -> Connect -> Script file (optional) -> Select the CM_low_power_debug.JLinkScript,
	      which is attached as part of the i2c_slave_addr_match_wakeup project.
	   b) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
              in .map file generated in the build configuration folder (Debug/Release).
