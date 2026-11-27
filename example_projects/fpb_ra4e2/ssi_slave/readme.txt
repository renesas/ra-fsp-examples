/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of SSI driver on Renesas RA MCUs based on Renesas FSP and
	streaming example of SSI Slave on Renesas RA MCUs based on Renesas FSP. The two RA MCU's will be connected to
	the host PC. One RA MCU will act as the SSI Master and the other RA MCU will act as the SSI Slave. Both RA MCU
	will have two modes of operation 1. Master write - Slave read, 2. Master read - Slave write. On selecting
	Master write - Slave read on both Master and Slave, the Master starts writing data, and the Slave reads the
	data. On selecting Slave write - Master read on both Master and Slave, the Slave starts writing data, and the
	Master reads the data. The result of transfers between SSI Master and SSI Slave is displayed on the terminal.
	The two example projects ssi_master_ep and ssi_slave_ep are provided for the SSI Slave EP, one intended
	for Master and other for the Slave.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	Terminal Console Application: Tera Term or a similar application
	LLVM Embedded Toolchain for ARM: Version 21.1.1

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4E2,
	                     EK-RA6E2, EK-RA8D1, EK-RA8M1, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA2L2, EK-RA8P1,
	                     EK-RA8M2, EK-RA8D2, FPB-RA4E2.
	2 x Renesas RA boards.
	2 x USB cables for programming and debugging (USB cable type varies by board model).
	1 x Host PC with at least 2 USB ports (1 for Master and 1 for Slave).
	Some jumper wires.

4. Hardware Connections:
	Connect USB debug ports of the two RA boards to USB ports of the host PC using two appropriate USB cables for
	EP programming and debugging.

	Pins and jumper settings:
		For EK-RA6M2, EK-RA6M3:
			SSISCK P112 (Master) -----> SSISCK P112 (Slave)
			SSIWS  P113 (Master) -----> SSIWS  P113 (Slave)
			SSITXD P115 (Master) -----> SSIRXD P114 (Slave)
			SSIRXD P114 (Master) -----> SSITXD P115 (Slave)
			GND         (Master) -----> GND 	(Slave)
	   
		For EK-RA6M1:
			SSIBCK  P403 (Master) -----> SSIBCK  P403 (Slave)
			SSILRCK P404 (Master) -----> SSILRCK P404 (Slave)
			SSITXD  P405 (Master) -----> SSIRXD  P406 (Slave)
			SSIRXD  P406 (Master) -----> SSITXD  P405 (Slave)
			GND          (Master) -----> GND  	  (Slave)

		For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5:
			SSIBCK0  P112 (Master) -----> SSIBCK0  P112 (Slave)
			SSILRCK0 P113 (Master) -----> SSILRCK0 P113 (Slave)
			SSITXD0  P115 (Master) -----> SSIRXD0  P114 (Slave)
			SSIRXD0  P114 (Master) -----> SSITXD0  P115 (Slave)
			GND           (Master) -----> GND	    (Slave)

		For EK-RA4E2, EK-RA6E2:
			SSIBCK0  P102 (Master) -----> SSIBCK0  P102 (Slave)
			SSILRCK0 P103 (Master) -----> SSILRCK0 P103 (Slave)
			SSITXD0  P109 (Master) -----> SSIRXD0  P110 (Slave)
			SSIRXD0  P110 (Master) -----> SSITXD0  P109 (Slave)
			GND           (Master) -----> GND	    (Slave)
			For EK-RA4E2 (Master):
			GPT1 P105 <--> AUDIO_CLK P100

		For EK-RA8D1:
			SSIBCK0  P403 (Master) -----> SSIBCK0  P403 (Slave)
			SSILRCK0 P404 (Master) -----> SSILRCK0 P404 (Slave)
			SSITXD0  P405 (Master) -----> SSIRXD0  P406 (Slave)
			SSIRXD0  P406 (Master) -----> SSITXD0  P405 (Slave)
			GND           (Master) -----> GND	    (Slave)
			The user needs to turn OFF SW1-3 and SW1-5 to use SSI.

		For EK-RA8M1:
			SSIBCK0  P112 (Master) -----> SSIBCK0  P112 (Slave)
			SSILRCK0 P113 (Master) -----> SSILRCK0 P113 (Slave)
			SSITXD0  P405 (Master) -----> SSIRXD0  P114 (Slave)
			SSIRXD0  P114 (Master) -----> SSITXD0  P405 (Slave)
			GND           (Master) -----> GND 	    (Slave)
			Remove Jumper J61 to use P112, P114, P405 for SSI.

		For FPB-RA8E1:
			SSIBCK0  P112 (J3:18) (Master)  -----> SSIBCK0  P112 (J3:18) (Slave)
			SSILRCK0 P113 (J3:17) (Master)  -----> SSILRCK0 P113 (J3:17) (Slave)
			SSITXD0  P405 (J4:14) (Master)  -----> SSIRXD0  P114 (J3:15) (Slave)
			SSIRXD0  P114 (J3:15) (Master)  -----> SSITXD0  P405 (J4:14) (Slave)
			GND           (J3:20) (Master)  -----> GND           (J3:20) (Slave)

		For EK-RA4L1:
			SSIBCK0  P113 (J2:41) (Master)  -----> SSIBCK0  P113 (J2:41) (Slave)
			SSILRCK0 P404 (J2:44) (Master)  -----> SSILRCK0 P404 (J2:44) (Slave)
			SSITXD0  P405 (J2:42) (Master)  -----> SSIRXD0  P406 (J2:38) (Slave)
			SSIRXD0  P406 (J2:38) (Master)  -----> SSITXD0  P405 (J2:42) (Slave)
			GND 	      (J4:25) (Master)  -----> GND           (J4:25) (Slave)

		For EK-RA8E2:
			SSIBCK0  P112 (J14:24) (Master)  -----> SSIBCK0  P112 (J14:24) (Slave)
			SSILRCK0 P113 (J14:22) (Master)  -----> SSILRCK0 P113 (J14:22) (Slave)
			SSITXD0  P115 (J14:8)  (Master)  -----> SSIRXD0  P114 (J14:10) (Slave)
			SSIRXD0  P114 (J14:10) (Master)  -----> SSITXD0  P115 (J14:8)  (Slave)
			GND           (J14:26) (Master)  -----> GND           (J14:26) (Slave)

		For EK-RA2L2:
			SSIBCK0  P411 (J2:5)  (Master)  -----> SSIBCK0  P411 (J2:5)  (Slave)
			SSILRCK0 P410 (J2:6)  (Master)  -----> SSILRCK0 P410 (J2:6)  (Slave)
			SSITXD0  P409 (J2:7)  (Master)  -----> SSIRXD0  P408 (J2:8)  (Slave)
			SSIRXD0  P408 (J2:8)  (Master)  -----> SSITXD0  P409 (J2:7)  (Slave)
			GND 	      (J2:40) (Master)  -----> GND           (J2:40) (Slave)

		For EK-RA8P1, EK-RA8D2:
			SSIBCK0  P112 (J37:23) (Master)  -----> SSIBCK0  P112 (J37:23) (Slave)
			SSILRCK0 P113 (J14:24) (Master)  -----> SSILRCK0 P113 (J14:24) (Slave)
			SSITXD0  P115 (J37:22) (Master)  -----> SSIRXD0  P114 (J37:21) (Slave)
			SSIRXD0  P114 (J37:21) (Master)  -----> SSITXD0  P115 (J37:22) (Slave)
			GND           (J17:19) (Master)  -----> GND           (J17:19) (Slave)

		For EK-RA8M2:
			SSIBCK0  P112 (J1:19) (Master)  -----> SSIBCK0  P112 (J1:19) (Slave)
			SSILRCK0 P113 (J1:15) (Master)  -----> SSILRCK0 P113 (J1:15) (Slave)
			SSITXD0  P115 (J1:17) (Master)  -----> SSIRXD0  P114 (J1:18) (Slave)
			SSIRXD0  P114 (J1:18) (Master)  -----> SSITXD0  P115 (J1:17) (Slave)
			GND           (J1:39) (Master)  -----> GND           (J1:39) (Slave)

		For FPB-RA4E2:
			SSIBCK0  P102 (J4:14) (Master)  -----> SSIBCK0  P102 (J4:14) (Slave)
			SSILRCK0 P103 (J4:13) (Master)  -----> SSILRCK0 P103 (J4:13) (Slave)
			SSITXD0  P109 (J4:2)  (Master)  -----> SSIRXD0  P110 (J4:3)  (Slave)
			SSIRXD0  P110 (J4:3)  (Master)  -----> SSITXD0  P109 (J4:2)  (Slave)
			GND           (J4:8)  (Master)  -----> GND           (J4:8)  (Slave)
			EK-RA4E2 (Master):
			Connect GTIOC4A P302 (J3:27) <--> AUDIO_CLK P213 (J3:9)

5. Verifying Operation:
	To run the example project, the user needs to compile and download the Master and Slave projects to the
	two RA boards and then follow the instructions below to observe the operation of the system.
		a. Set up hardware connection properly as mentioned in the 'Hardware Connections' section.

		b. Run the example projects on Master and Slave boards respectively and then open two instances
		   of the serial terminal application (e.g., Tera Term) to see the output status of the example
		   projects for Master and Slave.

		c. To select the Master Write - Slave Read operation, press "1" on the Master EP first, and then
		   press "1" on the Slave EP.
			- Master starts writing and Slave reads and both RA MCUs display the data transaction/messages
			  on the terminal.
			- After the transaction is completed, pressing any key to terminate the process.

		d. To select the Master Read - Slave Write operation, press "2" on the Slave EP first, and then
		   press "2" on the Master EP.
			- Master starts reading and Slave writes and both RA MCUs display the data transaction/messages
			  on the terminal.
			- After the transaction is completed, pressing any key to terminate the process.

Note:
1) The user cannot change the configuration from Master to Slave at run time.
   The example project ssi_master_ep has Master configuration settings and runs in Master mode and the example
   project ssi_slave_ep has Slave configuration settings and runs in Slave mode.

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
