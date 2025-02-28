/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     	The example project demonstrates the typical use of the SPI HAL module APIs using SPI and DTC modules. The project
        configures SPI module in Master and Slave mode. Once the modules are initialized and the SPI channel is configured,
	Master and Slave can transmit and receive data based on commands from user sent through J-Link RTT Viewer.
	SPI data transmission and reception are handled by the DTC module.

2. Hardware Requirements:
	Supported RA boards: EK-RA4L1
	2 x Renesas RA boards.
	2 x Type-C USB cables for programming and debugging.
	Some jumper wires.

3. Hardware Connections:
     	For EK-RA4L1:
		Board1 (Master)			Board2 (Slave)
     		P210 MISO0  (J2:45)	-->	P210 MISO0  (J2:45)
     		P211 MOSI0  (J2:43)	-->	P211 MOSI0  (J2:43)
     		P209 RSPCK0 (J2:39)	-->	P209 RSPCK0 (J2:39)

4. Verifying Operation:
    	To run this example project, the user needs to import, build the spi_master and spi_slave EP.
        Then, follow below instructions to observe the EP operation:
			
       		a. Connect the Board1 and the Board2 to host PC via two Type-C USB Cables.

		b. Download the spi_master EP to Board1 to run as Master.
		   Download the spi_slave EP to Board2 to run as Slave.

		c. Connect Board1 to Board2 as the instructions mentioned in the "Hardware Connections" section.

       		d. Open the J-Link RTT Viewer for 2 boards to see the output status of the Example Projects
		   for Master and Slave.

       	    	e. For selecting Write and Read operation, press "1":
	  		- The user needs to enter an input message to Board1.
			- The transmitted/received message will be displayed on J-Link RTT Viewer.

       	    	f. For selecting WriteRead operation, press "2":
	  	   	- The user needs to enter an input message to Board1.
			- Both transmitted/received messages will be displayed on J-Link RTT Viewer.

		g. For selecting Exit operation, press "3":
	  	   	- Board1/Board2 closes the SPI.

Note: 
1) The user is expected to enter data of size not exceeding 64 bytes.
   As SPI bit width is set to 32 bits, each 4 bytes of user data input will be transferred in single SPI transmission.
   Operation is not guaranteed for any user input value other than integer, char (i.e. float, special char) through RTT.

2) The user cannot change the configuration from Master to Slave at run time.
   The spi_master EP has Master configuration settings and runs in Master mode.
   The spi_slave EP has Slave configuration settings and runs in Slave mode.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200009f4
   b. Keil:	0x20000504
   c. IAR:      0x200002c4
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).