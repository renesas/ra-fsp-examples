/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of SSI driver on Renesas RA MCUs based on Renesas FSP and
	streaming example of SSI Slave on Renesas RA MCUs based on Renesas FSP. The two RA MCU's will be connected to the
	host PC. One RA MCU will act as the SSI Master and the other RA MCU will act as the SSI Slave. Both RA MCU will
	have two modes of operation 1. Master Write - Slave Read, 2. Master Read - Slave Write. On Selecting Master
	Write - Slave Read on both Master and Slave, the Master starts writing data, and the Slave reads the data. On
	Selecting Slave Write - Master Read on both Master and Slave, the Slave starts writing data, and the Master reads
	the data. The result of transfers between SSI Master and SSI Slave is displayed on the J-Link RTT Viewer.
	The two example projects ssi_master_ek_ra2l2_ep and ssi_slave_ek_ra2l2_ep are provided for the SSI Slave EP,
	one intended for Master and other for the Slave.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.1.0
	e2 studio: Version 2025-07
	SEGGER J-Link RTT Viewer: Version 8.58
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4E2, EK-RA6E2,
                             EK-RA8D1, EK-RA8M1, FPB-RA8E1, EK-RA4L1, EK-RA2L2
	2 x Renesas RA boards.
	2 x Micro USB cables for programming and debugging.
	1 x A PC with at least 2 USB ports (1 for Master and 1 for Slave).
	Some jumper wires.

4. Hardware Connections:
	Pins and Jumper Settings:
		For EK-RA6M2, EK-RA6M3:
			P112 SSISCK (Master) -----> P112 SSISCK (Slave) 
			P113 SSIWS  (Master) -----> P113 SSIWS  (Slave) 
			P115 SSITXD (Master) -----> P114 SSIRXD (Slave) 
			P114 SSIRXD (Master) -----> P115 SSITXD (Slave) 
			GND         (Master) -----> GND 	(Slave)
	   
		For EK-RA6M1:
			P403 SSIBCK  (Master) -----> P403 SSIBCK  (Slave)
			P404 SSILRCK (Master) -----> P404 SSILRCK (Slave)
			P405 SSITXD  (Master) -----> P406 SSIRXD  (Slave)
			P406 SSIRXD  (Master) -----> P405 SSITXD  (Slave)
			GND          (Master) -----> GND  	  (Slave)
	   
		For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5:
			P112 SSIBCK0  (Master) -----> P112 SSIBCK0  (Slave)
			P113 SSILRCK0 (Master) -----> P113 SSILRCK0 (Slave)
			P115 SSITXD0  (Master) -----> P114 SSIRXD0  (Slave)
			P114 SSIRXD0  (Master) -----> P115 SSITXD0  (Slave)
			GND           (Master) -----> GND	    (Slave)

		For EK-RA4E2, EK-RA6E2:
			P102 SSIBCK0  (Master) -----> P102 SSIBCK0  (Slave)
			P103 SSILRCK0 (Master) -----> P103 SSILRCK0 (Slave)
			P109 SSITXD0  (Master) -----> P110 SSIRXD0  (Slave)
			P110 SSIRXD0  (Master) -----> P109 SSITXD0  (Slave)
			GND           (Master) -----> GND	    (Slave)	
			For EK-RA4E2 (Master):
				P105 (GPT1) <--> P100 (AUDIO_CLK)

		For EK-RA8D1:
			P403 SSIBCK0  (Master) -----> P403 SSIBCK0  (Slave)
			P404 SSILRCK0 (Master) -----> P404 SSILRCK0 (Slave)
			P405 SSITXD0  (Master) -----> P406 SSIRXD0  (Slave)
			P406 SSIRXD0  (Master) -----> P405 SSITXD0  (Slave)
			GND           (Master) -----> GND	    (Slave)
			EK-RA8D1: The user needs to turn OFF SW1-3 and SW1-5 to use SSI.

		For EK-RA8M1:
			P112 SSIBCK0  (Master) -----> P112 SSIBCK0  (Slave) 
			P113 SSILRCK0 (Master) -----> P113 SSILRCK0 (Slave)
			P405 SSITXD0  (Master) -----> P114 SSIRXD0  (Slave) 
			P114 SSIRXD0  (Master) -----> P405 SSITXD0  (Slave) 
			GND           (Master) -----> GND 	    (Slave) 
			EK-RA8M1: Remove Jumper J61 to use P112, P114, P405 for SSI.

		For FPB-RA8E1:
			P112 SSIBCK0  (Master) (J3:18) -----> P112 SSIBCK0  (Slave) (J3:18)
			P113 SSILRCK0 (Master) (J3:17) -----> P113 SSILRCK0 (Slave) (J3:17)
			P405 SSITXD0  (Master) (J4:14) -----> P114 SSIRXD0  (Slave) (J3:15)
			P114 SSIRXD0  (Master) (J3:15) -----> P405 SSITXD0  (Slave) (J4:14)
			GND                    (J3:20) -----> GND           (Slave) (J3:20)

		For EK-RA4L1:
			P113 SSIBCK0  (Master) (J2:41) -----> P113 SSIBCK0  (Slave) (J2:41)
			P404 SSILRCK0 (Master) (J2:44) -----> P404 SSILRCK0 (Slave) (J2:44)
			P405 SSITXD0  (Master) (J2:42) -----> P406 SSIRXD0  (Slave) (J2:38)
			P406 SSIRXD0  (Master) (J2:38) -----> P405 SSITXD0  (Slave) (J2:42)
			GND 	      (Master) (J4:25) -----> GND           (Slave) (J4:25)
		
		For EK-RA2L2:
			P411 SSIBCK0  (Master) (J2:5) -----> P411 SSIBCK0  (Slave) (J2:5)
			P410 SSILRCK0 (Master) (J2:6) -----> P410 SSILRCK0 (Slave) (J2:6)
			P409 SSITXD0  (Master) (J2:7) -----> P408 SSIRXD0  (Slave) (J2:8)
			P408 SSIRXD0  (Master) (J2:8) -----> P409 SSITXD0  (Slave) (J2:7)
			GND 	      (Master) (J2:40)-----> GND 	   (Slave) (J2:40)

	Connect the Micro USB end of the Micro USB device cable to Debug port of the board.
	Connect the other end of this cable to USB port of the host PC.

5. Verifying Operation
	To run this example project, the user needs to compile and download the example project to the evaluation kit and
	then follow below instructions to observe the operation of the system.
		1. Connect the Master RA MCU and Slave RA MCU device to host PC via two Micro USB Cables and connect wires
		   as mentioned in the 'Hardware Connections' section above.

		2. Run the Example Projects on Master and Slave respectively and then open two instances of J-Link RTT Viewer
		   to see the output status of the Example Projects for Master and Slave.

		3. For Selecting Master Write - Slave Read operation, press "1" on both the EPs (Master and Slave sequentially).
			a. Master starts writing and Slave reads and both RA MCUs display the data transaction/messages on J-Link RTT Viewer.
			b. After the transaction is completed, turn off the Slave first by pressing any key and then for the Master press
			   any key to terminate the process.

		4. For Selecting Master Read - Slave Write operation, press "2" on both the EPs (Master and Slave sequentially).
			a. Master starts reading and Slave writes and both RA MCUs display the data transaction/messages on J-Link RTT Viewer.
			b. After the transaction is completed, turn off the Master first by pressing any key, and then for the Slave press
			   any key to terminate the process.

Note:
1) The user cannot change the configuration from Master to Slave at run time.
   The example project ssi_master_ek_ra2l2_ep has Master configuration settings and runs in Master mode and the example
   project ssi_slave_ek_ra2l2_ep has Slave configuration settings and runs in Slave mode.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200050f8
   b. Keil:     Not Available 
   c. IAR:      Not Available
   
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).