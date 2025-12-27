/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

    This project demonstrates the basic functionalities of Local Interconnect Network (LIN) on Renesas RA MCUs based on 
    the Renesas FSP. The LIN modules communicate with transceivers that satisfy the ISO9141 protocol. The Master defines 
    four distinct messages, each assigned a unique ID, allowing the user to select and transmit a specific message to 
    the Slave. The Master sends a start frame with the selected ID to retrieve data, and the Slave responds accordingly. 
    The user can select a baud rate (2400, 4800, 9600, 10400, 14400, 19200) from the application menu. Additionally, 
    the LIN module's baud rate can be configured to other supported values, as specified in the markdown file, by modifying 
    the configuration.xml. For the SAU LIN Slave, users can enter Low Power Mode (Software Standby) via the EP menu and 
    wake up when triggered by the Master.

    Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, 
    depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer. 
    If supported, it defaults to the serial terminal (UART).
    To modify the default terminal settings, refer to the instructions in the Special Topic section of sci_b_lin_notes.md.

    J-Link OB VCOM support: EK-RA8M1, FPB-RA0E2.

2. Software Requirements:
    Renesas Flexible Software Package (FSP): Version 6.3.0
    e2 studio: Version 2025-12
    SEGGER J-Link RTT Viewer: Version 8.92
    GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
    Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
   Supported RA Boards: FPB-RA0E2, EK-RA8M1
    
   2 x Renesas RA boards
   2 × USB cables:
       - FPB-RA0E2: USB Type-C cables
       - EK-RA8M1: Micro-USB cables
   2 LIN transceivers
       Link Product: https://www.mikroe.com/lin-click
       Manufacturer Product Number: MIKROE-3816
   1 x 12V DC Power Supply
   10 x Jumper cables (female to female)
   3 x cables for LIN bus connection
   2 × 10kΩ pull-up resistors
	
4. Hardware Connections:

    Board         | Pin Name | Pin Number
    --------------------------------------------
    FPB-RA0E2     | RX_PIN   | P403 (J1 Pin 4)
                  | TX_PIN   | P402 (J1 Pin 3)
    --------------------------------------------				 
    EK-RA8M1      | RX_PIN   | P610 (J54 Pin 22)
                  | TX_PIN   | P609 (J54 Pin 16)
    
    Refer to the RX_PIN and TX_PIN assignments for the connections below:		  
    Board1(Master)                 LIN transceiver(Board1)    LIN transceiver(Board2)   Board2(Slave)
    ------------------------------------------------------------------------------------------------	                      
    3.3V       <---------------->  3.3V                       3.3V  <---------------->  3.3V     
    GND        <---------------->  GND                        GND   <---------------->  GND      
    RX_PIN     <---------------->  TX                         TX    <---------------->  RX_PIN
    TX_PIN     <---------------->  RX      (See Note 2)       RX    <---------------->  TX_PIN
    3.3V       <---------------->  EN                         EN    <---------------->  3.3V  
                                   LIN    <--------------->   LIN 							  
                                   GND: connect to 0V         GND: connect to 0V
                                   VS : connect to 12V        VS:  connect to 12V
                                   
    Note: 
         1. The LIN transceiver requires a 12V supply. Connect VS to 12V and GND to ground.
         2. For LIN transceiver (TLE7259-3), digital I/O levels compatible with 3.3V and 5V microcontrollers.
         3. The RX pin of LIN transceiver requires an external pull-up resistor connected to the microcontroller supply to define 
            the output voltage level (refer to the hardware connections image in the .md file).
         4. For LIN transceiver Master Mode, the resistor at JP2 (MODE) must be relocated to the right side: Removing the resistor 
            at the left side and soldering it at the right side.

Note: 
    When using the SEGGER J-Link RTT Viewer, the RTT block address may be required to observe the EP's operation. Find the address 
    for the _SEGGER_RTT variable in the .map file (e2studio\Debug).