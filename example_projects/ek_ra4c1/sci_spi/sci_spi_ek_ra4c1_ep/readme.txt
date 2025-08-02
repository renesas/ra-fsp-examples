/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic usage of SCI_SPI driver. The project writes commands to the 
	temperature module MAX31723PMB1 and reads the temperature value using SCI_SPI bus which is configured as master.
	The temperature values are continuously printed on J-Link RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Temperature sensor MAX31723PMB1 (e.g., https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31723PMB1?qs=UmMSjoC1xtH8e742i4OoUA%3D%3D).
	Some jumper wires used to connect the Temperature sensor to the RA board.

3. Hardware Connections:
	For EK-RA2A1:
	-----------     
		Connect Temperature sensor (J1) to PMODB (J6).
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMODB.
 
	For EK-RA4M1, EK-RA6M1, EK-RA6M2:
	-----------
		Connect Temperature sensor (J1) to PMODA (J5).
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMODA.
        
	For EK-RA6M3, EK-RA6M3G, EK-RA2L1, EK-RA4E2, EK-RA6E2, EK-RA2L2:
	-----------
		Connect Temperature sensor (J1) to PMOD1 (J26).
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.	

	For EK-RA4M3, EK-RA4M2, EK-RA6M4, EK-RA6M5, EK-RA2E1, EK-RA2E2, EK-RA8M1, EK-RA8D1:
	-----------
		Connect Temperature sensor (J1) to PMOD2 (J25).
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD2.

	For FPB-RA2E3:
	-----------
		Connect Temperature sensor (J1) to PMOD1.
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.

	For RSSK-RA6T1, FPB-RA6E1, FPB-RA4E1:
	-----------
		Temperature sensor (J1:4) to P102
		Temperature sensor (J1:2) to P101
		Temperature sensor (J1:3) to P100
		Temperature sensor (J1:5) to GND
		Temperature sensor (J1:6) to VCC
		Temperature sensor (J1:1) to P103

	For EK-RA4W1:
	-----------
		Connect Temperature sensor (J1) to PMOD (CN6).
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD.
		
	For MCK-RA6T2:
	-----------
		Temperature sensor (J1:1) to PA12
		Temperature sensor (J1:2) to PA09
		Temperature sensor (J1:3) to PA10
		Temperature sensor (J1:4) to PA08
		Temperature sensor (J1:5) to GND
		Temperature sensor (J1:6) to VCC

	For MCK-RA4T1, MCK-RA6T3:
	-----------
		Temperature sensor (J1:1) to P301 (CN3:8)
		Temperature sensor (J1:2) to P109 (CN5:2)
		Temperature sensor (J1:3) to P110 (CN5:3)
		Temperature sensor (J1:4) to P111 (CN3:3)
		Temperature sensor (J1:5) to GND  (CN3:32)
		Temperature sensor (J1:6) to VCC  (CN2:24)
	
	For MCK-RA8T1:
	-----------
		Temperature sensor (J1:1) to P604 (CN4:16)
		Temperature sensor (J1:2) to P603 (CN3:33)
		Temperature sensor (J1:3) to P602 (CN3:31)
		Temperature sensor (J1:4) to P601 (CN4:3)
		Temperature sensor (J1:5) to GND  (CN3:34)
		Temperature sensor (J1:6) to VCC  (CN3:24)
		
	For EK-RA2A2:
	-----------
		Temperature sensor (J1:1) to P503 (J1:3)
		Temperature sensor (J1:2) to P501 (J1:1)
		Temperature sensor (J1:3) to P500 (J4:19)
		Temperature sensor (J1:4) to P502 (J1:2)
		Temperature sensor (J1:5) to GND
		Temperature sensor (J1:6) to VCC  

	For FPB-RA8E1:
	-----------
		Connect Temperature sensor (J1) to PMOD2.
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD2.

	For EK-RA4L1:
	-----------
		Temperature sensor (J1:1) to P112 (J3:18)
		Temperature sensor (J1:2) to P109 (J3:19)
		Temperature sensor (J1:3) to P110 (J3:20)
		Temperature sensor (J1:4) to P111 (J3:17)
		Temperature sensor (J1:5) to GND  (J3:25)
		Temperature sensor (J1:6) to VCC  (J3:1)

	For EK-RA8E2:
	-----------
		The user must turn OFF SW4-4 to use PMOD2 for SPI.
		Connect Temperature sensor (J1) to PMOD2 (J25).
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD2.

	For FPB-RA2T1:
	-----------
		To use PMOD1 for SPI: Short E41, E42, E48, E49 and Open E35, E39, E40.
		Connect Temperature sensor (J1) to PMOD1.
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.

        For EK-RA8P1:
	-----------
		The user must close E15, E16 and open E10, E14 to use PMOD2 for SPI.
		Connect Temperature sensor (J1) to PMOD2 (J25).
		Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD2.

	For EK-RA4C1:
	-----------
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.
		Temperature sensor (J1:1) to P112 (J4:18)
		Temperature sensor (J1:2) to P109 (J4:21)
		Temperature sensor (J1:3) to P110 (J4:20)
		Temperature sensor (J1:4) to P111 (J4:19)
		Temperature sensor (J1:5) to GND  (J4:5)
		Temperature sensor (J1:6) to VCC  (J4:1)

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001a44
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

3) For GCC ARM Embedded Toolchain: To enable printing floats to the RTT Viewer, edit the project settings and ensure that
   "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf
