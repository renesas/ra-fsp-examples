/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates Audio Playback with PWM module's functionality. It uses ADPCM module to
	decode the PCM data. The Decoded PCM data is in signed 16-bit format. The application converts this signed
	PCM data into unsigned PCM data and provides the converted unsigned data to the audio playback with PWM module.
	The converted unsigned data is provided as input and output is generated as PWM signal. Error and info messages
	will be printed on the terminal.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3,
	                     EK-RA6M4, RSSK-RA6T1, EK-RA6M5, FPB-RA4E1, FPB-RA6E1, MCK-RA6T2, EK-RA4E2, EK-RA6E2,
	                     MCK-RA4T1, MCK-RA6T3, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA2A2, FPB-RA8E1, EK-RA4L1,
	                     EK-RA8E2, EK-RA8P1, EK-RA4C1, MCK-RA8T2, EK-RA8D2, EK-RA8M2, FPB-RA6E2.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).
	1 x Pmod AMP2 (e.g., https://digilent.com/reference/pmod/pmodamp2/).
	1 x Wired headset with 3.5mm audio jack.
	Some jumper wires.

3. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and
	debugging.

	Connect the RA board to Pmod AMP2 as shown below:

	EK-RA2A1                               Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P109            ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA2L1                               Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P111            ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA4M1, EK-RA6M1
	EK-RA6M5, RSSK-RA6T1
	FPB-RA4E1, FPB-RA6E1
	EK-RA4E2, EK-RA6E2
	MCK-RA4T1, MCK-RA6T3,
	FPB-RA6E2                              Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P500            ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA4M2, EK-RA6M3                     Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P107            ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA4W1                               Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P501            ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA4M3, EK-RA6M2
	EK-RA6M4, EK-RA8M1, EK-RA8D1           Pmod AMP2                         
	----------------------------------------------------------
	PWM_OUTPUT_PIN P713            ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	MCK-RA6T2                              Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN PB04 (CN4:29)   ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	MCK-RA8T1                              Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P411 (CN6:9)    ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA2A2                               Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P110 (J4:9)     ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	FPB-RA8E1                              Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P107 (J1:49)    ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA4L1                               Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P107 (J2:49)    ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA8E2                               Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P107 (J3:21)    ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA8P1, EK-RA8D2                     Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P713 (J1:35)    ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA4C1                               Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P107 (J2:47)    ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)
	The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
	the on-board debug functionality.

	MCK-RA8T2                              Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P800 (CN4:11)   ---->   AIN       (Pin 1)
	VCC -- (via 22k ohms resistor) ---->   ~SHUTDOWN (Pin 4)
	VCC                            ---->   VCC       (Pin 6)
	GND                            ---->   GAIN      (Pin 2)
	GND                            ---->   GND       (Pin 5)

	EK-RA8M2				Pmod AMP2
	----------------------------------------------------------
	PWM_OUTPUT_PIN P713 (J2:4)      ---->  	AIN  	  (Pin 1)
	VCC -- (via 22k ohms resistor)	----> 	~SHUTDOWN (Pin 4)
	VCC                             ---->  	VCC 	  (Pin 6)
	GND                             ---->  	GAIN	  (Pin 2)
	GND                             ---->  	GND 	  (Pin 5)
	The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4,
	5-6, 7-8 to use the on-board debug functionality.

Note:
1) The user is expected to enter data not exceeding 15 bytes in size.
   Operation is not guaranteed for any user input value other than integer (e.g., float, char, special character)
   through the terminal.

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
