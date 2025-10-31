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
	Supported RA boards: EK-RA4L1, EK-RA2L2
	2 x Renesas RA boards.
	2 x Micro USB cables for programming and debugging.
	Some jumper wires.

3. Hardware Connections:
     	For EK-RA4L1:
		Board1 (Master)			Board2 (Slave)
     		MISO0  P210 (J2:45)	-->	MISO0  P210 (J2:45)
     		MOSI0  P211 (J2:43)	-->	MOSI0  P211 (J2:43)
     		RSPCK0 P209 (J2:39)	-->	RSPCK0 P209 (J2:39)

     	For EK-RA2L2:
		Board1 (Master)			Board2 (Slave)
     		MISO0  P100 (J2:39)	-->	MISO0  P100 (J2:39)
     		MOSI0  P101 (J2:38)	-->	MOSI0  P101 (J2:38)
     		RSPCK0 P102 (J2:37)	-->	RSPCK0 P102 (J2:37)

4. Verifying Operation:
    	To run this example project, the user needs to import, build the spi_master and spi_slave EP.
        Then, follow below instructions to observe the EP operation:
			
       		a. Connect the Board1 and the Board2 to host PC via 2 micro USB cables.

		b. Download the spi_master EP to Board1 to run in Master mode.
		   Download the spi_slave EP to Board2 to run in Slave mode.

		c. Connect Board1 to Board2 as the instructions mentioned in the "Hardware Connections" section.

       		d. Open the J-Link RTT Viewer for 2 boards to see the output status. It is important to note that 
		   the user must select the same option from the EP menu for both Board1 and Board2.

       	    	e. For selecting Write and Read operation, press "1":
	  		- The user needs to enter an input message to Board1.
			- The transmitted/received or any error message will be displayed on J-Link RTT Viewer.

       	    	f. For selecting WriteRead operation, press "2":
	  	   	- The user needs to enter an input message to Board1.
			- The transmitted/received or any error message will be displayed on J-Link RTT Viewer.

		g. For selecting Exit operation, press "3":
	  	   	- 2 boards close the SPI module and stop the EP here.
			- Restart 2 boards to run the EP again.

Note:
1) By default, the user is expected to enter data of size less than 40 bytes.
   If the user wants to transfer data exceeding 40 bytes, please change the value of BUFF_LEN macro in spi_ep.h file.
   As SPI bit width is set to 32 bits, each 4 bytes of user data input will be transferred in single SPI transmission.
   Operation is not guaranteed for any user input value other than integer, char (i.e. float, special char) through RTT.

2) The user cannot change the configuration from Master to Slave at run time.
   The spi_master EP has Master configuration settings and runs in Master mode.
   The spi_slave EP has Slave configuration settings and runs in Slave mode.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004cf8
   b. Keil:	0x200048a8
   c. IAR: 0x200048f0
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).