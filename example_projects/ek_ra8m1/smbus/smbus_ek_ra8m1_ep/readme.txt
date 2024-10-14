/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/
1. Project Overview:
	This example project demonstrates the basic usage of I2C communication conforming to the SMBus Specification (version 2.0) on Renesas
	RA MCUs using the Renesas Flexible Software Package (FSP). The project involves writing commands to the MAX31875 sensor and reading
	the resulting temperature values. The SMBus transmission time is monitored by the GPT module to ensure the MCU acting as a master device
	complies with the SMBus timeout standard. The Packet Error Check (PEC) byte is calculated in CRC-8 format at both transmission and reception.
	These temperature values are continuously printed to a terminal. Additionally, status information and error messages are displayed on the host PC.
	The error LED will turn on if an error occurs.

	NOTE1:
	- To display information, users can choose between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART) with J-Link OB VCOM.
	  It is important to note that the user should only operate a single terminal application at a time to avoid conflicts or data inconsistencies. 
	- For instructions on how to switch between these options, please refer to the "Verifying Operation" section in smbus_notes.md file.
	- By default, EP information is printed to the host PC using the Serial Terminal for boards that support J-Link OB VCOM.
	For the RA boards that do not support J-Link OB VCOM, EP uses the SEGGER J-Link RTT Viewer by default instead.
	- RA boards supported for J-Link OB VCOM: EK-RA8M1.

2. Hardware Requirements:

	Supported RA Boards: EK-RA8M1
	1 x Host PC.
	1 x Renesas RA board.
	1 x Micro USB cable.
	1 x Evaluation Kit for the MAX31875
		Link Product: https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31875EVKIT?qs=BZBei1rCqCDTW6E3jJcKjw%3D%3D&_gl=1
		Manufacturer Product Number: MAX31875EVKIT#
	- RA boards that do not require external resistors for pull-up:: EK-RA8M1
	4 x Jumper cables (4 - both ends female).

3. Hardware Connections:

    EK-RA8M1:     
	---------
	Connect RA board to Host PC by Micro USB cable.
	Connect RA board to MAX31875PMB1 as below:
     	RA board                       MAX31875PMB1
	-----------------------------------------------
	SCL1 P512 (J56-Pin37) <--------> SCL  (J1-Pin3)
	SDA1 P511 (J57-Pin38) <--------> SDA  (J1-Pin4)
	GND  	  (J57-Pin49) <--------> GND  (J1-Pin5)
	VCC  	  (J56-Pin40) <--------> VDD  (J1-Pin6)


NOTE2:
    For using the Serial Terminal (UART):
      1) The serial terminal setting parameters are as follows:
      	Select the COM port provided by the J-Link on-board.
      	Baud rate: 115200 bps
      	Data length: 8-bits  
      	Parity: none
      	Stop bit: 1-bit
      	Flow control: none
