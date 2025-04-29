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
     	The two example projects ssi_master_fpb_ra8e1_ep and ssi_slave_fpb_ra8e1_ep are provided for the the SSI Slave EP,
     	one intended for Master and other for the Slave.

2. Software Requirements:
     	Renesas Flexible Software Package (FSP): Version 5.9.0
     	e2 studio: Version 2025-04
     	SEGGER J-Link RTT Viewer: Version 8.12f
     	LLVM Embedded Toolchain for ARM: Version 18.1.3

3. Hardware Requirements:
     	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4E2, EK-RA6E2,
                             EK-RA8D1, EK-RA8M1, FPB-RA8E1.
     	2 x Renesas RA boards.
     	2 x Type-C USB cables for programming and debugging.
     	1 x A PC with at least 2 USB ports (1 for Master and 1 for Slave).
     	Some jumper wires.

4. Hardware Connections:
     	Pin and Jumper Settings:
     	        1) For EK-RA6M3:
	   		P112 SSISCK (Master) -----> P112 SSISCK (Slave) 
	   		P113 SSIWS  (Master) -----> P113 SSIWS  (Slave) 
	   		P115 SSITXD (Master) -----> P114 SSIRXD (Slave) 
	   		P114 SSIRXD (Master) -----> P115 SSITXD (Slave) 
	   		GND                  -----> GND
	   
		2) For EK-RA6M1:
	   		P403 SSIBCK  (Master) -----> P403 SSIBCK  (Slave)
	   		P404 SSILRCK (Master) -----> P404 SSILRCK (Slave)
	   		P405 SSITXD  (Master) -----> P406 SSIRXD  (Slave)
	   		P406 SSIRXD  (Master) -----> P405 SSITXD  (Slave)
	   		GND                   -----> GND  
	   
		3) For EK-RA6M2:
	   		P112 SSISCK (Master) -----> P112 SSISCK (Slave)
	   		P113 SSIWS  (Master) -----> P113 SSIWS  (Slave)
	   		P115 SSITXD (Master) -----> P114 SSIRXD (Slave)
	   		P114 SSIRXD (Master) -----> P115 SSITXD (Slave)
	   		GND                  -----> GND
	   
		4) For EK-RA6M4:
	   		P112 SSIBCK0  (Master) -----> P112 SSIBCK0  (Slave)
	   		P113 SSILRCK0 (Master) -----> P113 SSILRCK0 (Slave)
	   		P115 SSITXD0  (Master) -----> P114 SSIRXD0  (Slave)
	   		P114 SSIRXD0  (Master) -----> P115 SSITXD0  (Slave)
	   		GND                    -----> GND
	   
		5) For EK-RA6M5:
	   		P112 SSIBCK0  (Master) -----> P112 SSIBCK   (Slave)
	   		P113 SSILRCK0 (Master) -----> P113 SSILRCK0 (Slave)
	   		P115 SSITXD0  (Master) -----> P114 SSIRXD0  (Slave)
	   		P114 SSIRXD0  (Master) -----> P115 SSITXD0  (Slave)
	   		GND                    -----> GND
	   
		6) For EK-RA4M2:
	   		P112 SSIBCK0  (Master) -----> P112 SSIBCK0  (Slave)
	   		P113 SSILRCK0 (Master) -----> P113 SSILRCK0 (Slave)
	   		P115 SSITXD0  (Master) -----> P114 SSIRXD0  (Slave)
	   		P114 SSIRXD0  (Master) -----> P115 SSITXD0  (Slave)
	   		GND                    -----> GND
	   
		7) For EK-RA4M3:
	   		P112 SSIBCK0  (Master) -----> P112 SSIBCK0  (Slave)
	   		P113 SSILRCK0 (Master) -----> P113 SSILRCK0 (Slave)
	   		P115 SSITXD0  (Master) -----> P114 SSIRXD0  (Slave)
	   		P114 SSIRXD0  (Master) -----> P115 SSITXD0  (Slave)
	   		GND                    -----> GND

   		8) For EK-RA4E2:
	   		P102 SSIBCK0  (Master) -----> P102 SSIBCK0  (Slave)
	   		P103 SSILRCK0 (Master) -----> P103 SSILRCK0 (Slave)
	   		P109 SSITXD0  (Master) -----> P110 SSIRXD0  (Slave)
	   		P110 SSIRXD0  (Master) -----> P109 SSITXD0  (Slave)
	   		GND                    -----> GND
	   		EK-RA4E2 (Master)
	   		P105 (GPT1) <--> P100 (AUDIO_CLK)

   		9) For EK-RA6E2:
	   		P102 SSIBCK0  (Master) -----> P102 SSIBCK0  (Slave)
	   		P103 SSILRCK0 (Master) -----> P103 SSILRCK0 (Slave)
	   		P109 SSITXD0  (Master) -----> P110 SSIRXD0  (Slave)
	   		P110 SSIRXD0  (Master) -----> P109 SSITXD0  (Slave)
	   		GND                    -----> GND

       		10) For EK-RA8D1:
	   		P403 SSIBCK0  (Master) -----> P403 SSIBCK0  (Slave)
	   		P404 SSILRCK0 (Master) -----> P404 SSILRCK0 (Slave)
	   		P405 SSITXD0  (Master) -----> P406 SSIRXD0  (Slave)
	   		P406 SSIRXD0  (Master) -----> P405 SSITXD0  (Slave)
	   		GND                    -----> GND
	   		EK-RA8D1: The user needs to turn OFF SW1-3 and SW1-5 to use SSI.

       		11) For EK-RA8M1:
           		P112 SSIBCK0  (Master) -----> P112 SSIBCK0  (Slave) 
	   		P113 SSILRCK0 (Master) -----> P113 SSILRCK0 (Slave)
	   		P405 SSITXD0  (Master) -----> P114 SSIRXD0  (Slave) 
	   		P114 SSIRXD0  (Master) -----> P405 SSITXD0  (Slave) 
	   		GND                    -----> GND  
	   		EK-RA8M1: Remove Jumper J61 to use P112, P114, P405 for SSI.

       		12) For FPB-RA8E1:
	   		P112 SSIBCK0  (Master) (J3:18) -----> P112 SSIBCK0  (Slave) (J3:18)
	   		P113 SSILRCK0 (Master) (J3:17) -----> P113 SSILRCK0 (Slave) (J3:17)
	   		P405 SSITXD0  (Master) (J4:14) -----> P114 SSIRXD0  (Slave) (J3:15)
	   		P114 SSIRXD0  (Master) (J3:15) -----> P405 SSITXD0  (Slave) (J4:14)
	   		GND                            -----> GND                   (J3:20)

       		13) Connect the Type-C USB end of the Type-C USB device cable to Debug port of the board.
		    Connect the other end of this cable to USB port of the host PC.

5. Verifying Operation
    	To run this example project, the user needs to compile and download the example project to the evaluation kit and
        then follow below instructions to observe the operation of the system.
       		1. Connect the Master RA MCU and Slave RA MCU device to host PC via two Type-C USB Cables.

       		2. Open the J-Link RTT Viewer to view the application status/transfer messages.

       	    	3. For Selecting Master Write - Slave Read operation, press "1"
	  		a. Master starts writing and Slave reads and both RA MCUs display the data transaction/messages on J-Link RTT Viewer.
	  	   	b. Note: For the Master Write - Slave Read operation mode, after the transaction is completed, turn off
				 the Slave first by pressing any key. For the Master, press any key to terminate the process.

       	    	4. For Selecting Master Read - Slave Write operation, press "2"
	  	   	a. Master starts reading and Slave writes and both RA MCUs display the data transaction/messages on J-Link RTT Viewer.
	  	   	b. Note: For the Master Read - Slave Write operation mode, after the transaction is completed, turn off the Master
                    	         first by pressing any key. For the Slave, press any key to terminate the process.

Note:
1) The user cannot change the configuration from Master to Slave at run time.
   The example project ssi_master_fpb_ra8e1_ep has Master configuration settings and runs in Master mode and the example
   project ssi_slave_fpb_ra8e1_ep has Slave configuration settings and runs in Slave mode.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060080
   b. Keil:     Not Available 
   c. IAR:      Not Available
   
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).