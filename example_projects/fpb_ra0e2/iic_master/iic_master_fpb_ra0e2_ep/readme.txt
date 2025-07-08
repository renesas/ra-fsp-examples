/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the IIC master HAL module APIs.
	The project initializes IIC master module with standard rate and interfaces with PmodACL Board for ADXL345.
	On power up after establishing the connection of sensor with RA board, it displays accelerometer axis data on
	RTT Viewer. Any API/event failure will be displayed on RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x PmodACL.
	1 x Type-C USB cable for programming and debugging.
	4 x Jumper wires used to connect the RA board to the PmodACL.
	
3. Hardware Connections:
      	For CK-RA6M5:
    	--------
		SDA1 P511  ----> SDA (on PmodACL)
    		SCL1 P512  ----> SCL (on PmodACL)
    		VCC pin    ----> VCC (on PmodACL)
    		GND pin    ----> GND (on PmodACL)

	For EK-RA6M3G, EK-RA6M3:
 	--------
		SDA2 P511  ----> SDA (on PmodACL)
		SCL2 P512  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For EK-RA6M2:
	--------
		SCL1 P100  ----> SCL (on PmodACL)
		SDA1 P206  ----> SDA (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For EK-RA6M1:
	---------
		SCL0 P400  ----> SCL (on PmodACL)
		SDA0 P401  ----> SDA (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For EK-RA6M4, EK-RA6M5:
	--------
		SDA0 P401  ----> SDA (on PmodACL)
		SCL0 P400  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
 		GND pin    ----> GND (on PmodACL)

	For EK-RA4M1:
	--------
		SDA1 P206  ----> SDA (on PmodACL)
		SCL1 P100  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For EK-RA2A1:
	--------
		SDA0 P401  ----> SDA (on PmodACL)
		SCL0 P000  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For RSSK-RA6T1:
	----------
		SDA1 P101  ----> SDA (on PmodACL)
		SCL1 P100  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For EK-RA2L1:
	--------
		SDA0 P407  ----> SDA (on PmodACL)
		SCL0 P408  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For EK-RA4M3:
	--------
		SDA0 P401  ----> SDA (on PmodACL)
		SCL0 P408  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For EK-RA2E1:
	--------
		SDA0 P401  ----> SDA (on PmodACL)
		SCL0 P400  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For EK-RA4M2:
	--------
		SDA1 P206  ----> SDA (on PmodACL)
		SCL1 P205  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For FPB-RA6E1, FPB-RA4E1, FPB-RA2E3:
	---------
		SDA0 P401  ----> SDA (on PmodACL)
		SCL0 P400  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For MCK-RA6T2:
	---------
		SDA0 PB07  ----> SDA (on PmodACL)
		SCL0 PB06  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For EK-RA8M1, EK-RA8D1, FPB-RA8E1:
	--------
		SDA1 P511  ----> SDA (on PmodACL)
		SCL1 P512  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

	For MCK-RA8T1:
	--------
		SDA1 P206 (CN2:24)  ----> SDA (on PmodACL)
		SCL1 P205 (CN2:25)  ----> SCL (on PmodACL)
		VCC pin   (CN1:24)  ----> VCC (on PmodACL)
		GND pin   (CN1:34)  ----> GND (on PmodACL)
    
	For EK-RA2A2:
	--------
		SDA0 P410  ----> SDA (on PmodACL)
		SCL0 P411  ----> SCL (on PmodACL)
		VCC pin    ----> VCC (on PmodACL)
		GND pin    ----> GND (on PmodACL)

        For FPB-RA0E1
        --------
        	SDAA0 P101  ----> SDA (on PmodACL)
        	SCLA0 P100  ----> SCL (on PmodACL)
        	VCC pin     ----> VCC (on PmodACL)
        	GND pin     ----> GND (on PmodACL)

	For EK-RA4L1:
	--------
		Place J15 jumper to use P100, P101 for I2C channel 0.
		SDA0 P100 (J3:2) ----> SDA (on PmodACL)
		SCL0 P101 (J3:4) ----> SCL (on PmodACL)
		VCC pin    	 ----> VCC (on PmodACL)
		GND pin    	 ----> GND (on PmodACL)

	For FPB-RA0E2:
	--------
		SDAA0 P913 (J1:19)  ----> SDA (on PmodACL)
		SCLA0 P914 (J1:18)  ----> SCL (on PmodACL)
		VCC pin    (J1:11)  ----> VCC (on PmodACL)
		GND pin    (J1:8)   ----> GND (on PmodACL)

Note:
1) For the functioning of IIC Master on all the boards except for EK-RA6M3, EK-RA6M3G, FPB-RA6E1, FPB-RA4E1, EK-RA8M1,
   FPB-RA2E3, EK-RA8D1, MCK-RA8T1, FPB-RA8E1, EK-RA4L1 external pull up resistors of value 3.9 or 4.7K ohms are required
   to be connected on I2C (SDA/SCL) lines.﻿

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20004e00
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the build configuration folder (Debug/Release).

4) For GCC ARM Embedded Toolchain: To enable printing floats to RTT Viewer, edit the project settings and ensure that
   "Use float with nano printf" is enabled. The setting can be found by:
   Properties > C/C++ Build > Settings > Tool Settings > GNU Arm Cross C Linker > Miscellaneous > Use float with nano printf
