/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about the Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        This project demonstrates the monitoring of USB Type-C connection status and VBUS electrical characteristics 
        on the Renesas EK-RA2L2 evaluation board. The application utilizes the RA2L2’s ADC to periodically sample 
        analog signals every 500 ms to calculate voltage (mV) and current (mA). It also retrieves USBC connection information.
        All data is sent to a PC via USB Full-Speed PCDC, and users can view the results using Tera Term or the QE for USB tool 
        in e² studio. The error LED will turn on if an error occurs.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
        e2 studio: Version 2025-12
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	- Supported RA board: EK-RA2L2
	- 1 × RA board
	- 1 × USB Type-A male to Micro-B male cable
        - 1 × USB Type-C male to Type-C male cable
        - 1 × E2 Lite emulator with mini USB and target cables (for programming and debugging)

4. Hardware Connections:
	- Connect the E2 Lite emulator to the target board using either the J20 or J13 connector.
        - Connect the J10 port to the host PC using a USB cable. This connection is dedicated to providing 5V power to the board.
        - Connect the USB Full-Speed (J11) port to the host PC using a USB Type-C cable for data communication and additional power.
        - To ensure correct operation, set the jumpers on the EK-RA2L2 board as follows: J16(2-3), J8(1-2), J9(2-3),
		  J29(1-2,3-4,5-6,7-8), J40(5-6), J41(56)

Note: For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of md file.
