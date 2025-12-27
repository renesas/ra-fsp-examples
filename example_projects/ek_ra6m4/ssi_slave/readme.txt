/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionalities of I2S driver on Renesas RA MCUs based on Renesas FSP and streaming example of SSI slave on Renesas RA MCUs 
	based on Renesas FSP.
	The two RA MCU's will be connected to the host PC. One RA MCU will act as the SSI Master and the other RA MCU will act as the SSI Slave. 
	Both RA MCU will have two modes of operation 
	1. Master Write - Slave Read , 2. Master Read - Slave Write .
	On Selecting Master Write - Slave Read on both Master and Slave, the Master starts writing data, and the Slave reads the data.
	On Selecting Slave Write - Master Read on both Master and Slave, the Slave starts writing data, and the Master reads the data. 
	The result of transfers between SSI Master and SSI Slave is displayed on the J-Link RTT Viewer.
	The two example projects ssi_master_ek_ra6m4_ep and ssi_slave_ek_ra6m4_ep are provided for the the SSI Slave EP, one intended for Master and other for the Slave.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	SEGGER J-Link RTT Viewer: Version 8.92
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	
     
3. Hardware Requirements:
   
   Hardware : 
	1) 2x RA board 
	2) 2x Micro USB device cable
	3) A PC with at atleast 2 USB port(1 for Master and 1 for Slave)
	4) 5x Connecting/Jumper wires

4. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below instructions to observe the operation of the system.

   Supported Boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5.

	1) Connect the Master RA MCU and Slave RA MCU device to host PC via two USB Cables.

	2) Open the RTT viewer to view the application status/transfer messages.	

	3) For Selecting Master Write - Slave Read operation, press "1" 
		a) Master starts writing and Slave reads and both RA MCUs display the data transaction/messages on J-Link RTT Viewer.
		b) Note : For the Master Write - Slave Read operation mode, after the transaction is completed, turn off the Slave first by pressing any key.For the Master, press any key to terminate the process.

	4) For Selecting Master Read - Slave Write operation, press "2" 
		a) Master starts reading and Slave writes and both RA MCUs display the data transaction/messages on J-Link RTT Viewer.
		b) Note : For the Master Read - Slave Write operation mode, after the transaction is completed, turn off the Master first by pressing any key.For the Slave, press any key to terminate the process.

5. The user cannot change the configuration from Master to Slave at run time.

	1) The example project ssi_master_ek_ra6m4_ep has Master configuration settings and runs in master mode and the example project ssi_slave_ek_ra6m4_ep has Slave configuration settings and runs in slave mode.
	
6. Hardware settings for the project:
	
    Pin and Jumper Settings:
     	
	   EK-RA6M3                   EK-RA6M3
	1) P112 SSISCK(Master) -----> P112 SSISCK(Slave) 
	   P113 SSIWS(Master)  -----> P113 SSIWS(Slave) 
	   P115 SSITXD(Master) -----> P114 SSIRXD(Slave) 
	   P114 SSIRXD(Master) -----> P115 SSITXD(Slave) 
	   GND                 -----> GND
	   
	   EK-RA6M1                   EK-RA6M1
	2) P403 SSIBCK(Master) -----> P403 SSIBCK(Slave)
	   P404 SSILRCK(Master)-----> P404 SSILRCK(Slave)
	   P405 SSITXD(Master) -----> P406 SSIRXD(Slave)
	   P406 SSIRXD(Master) -----> P405 SSITXD(Slave)
	   GND                 -----> GND  
	   
	   EK-RA6M2                   EK-RA6M2
	3) P112 SSISCK(Master) -----> P112 SSISCK(Slave)
	   P113 SSIWS(Master)  -----> P113 SSIWS(Slave)
	   P115 SSITXD(Master) -----> P114 SSIRXD(Slave)
	   P114 SSIRXD(Master) -----> P115 SSITXD(Slave)
	   GND                 -----> GND
	   
	   EK-RA6M4                   EK-RA6M4
	4) P112 SSIBCK0(Master)-----> P112 SSIBCK0(Slave)
	   P113 SSILRCK0(Master)----> P113 SSILRCK0(Slave)
	   P115 SSITXD0(Master)-----> P114 SSIRXD0(Slave)
	   P114 SSIRXD0(Master)-----> P115 SSITXD0(Slave)
	   GND                 -----> GND
	   
	   EK-RA6M5                   EK-RA6M5
	5) P112 SSIBCK0(Master)-----> P112 SSIBCK(Slave)
	   P113 SSILRCK0(Master)----> P113 SSILRCK0(Slave)
	   P115 SSITXD0(Master)-----> P114 SSIRXD0(Slave)
	   P114 SSIRXD0(Master)-----> P115 SSITXD0(Slave)
	   GND                 -----> GND
	   
	   EK-RA4M2                   EK-RA4M2
	6) P112 SSIBCK0(Master)-----> P112 SSIBCK0(Slave)
	   P113 SSILRCK0(Master)----> P113 SSILRCK0(Slave)
	   P115 SSITXD0(Master)-----> P114 SSIRXD0(Slave)
	   P114 SSIRXD0(Master)-----> P115 SSITXD0(Slave)
	   GND                 -----> GND
	   
	   EK-RA4M3                   EK-RA4M3
	7) P112 SSIBCK0(Master)-----> P112 SSIBCK0(Slave)
	   P113 SSILRCK0(Master)----> P113 SSILRCK0(Slave)
	   P115 SSITXD0(Master)-----> P114 SSIRXD0(Slave)
	   P114 SSIRXD0(Master)-----> P115 SSITXD0(Slave)
	   GND                 -----> GND
	   
	8) Connect the micro USB end of the micro USB device cable to Debug port of the board. Connect the other end of this cable to USB port of the 
	   host PC.

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008b8
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
