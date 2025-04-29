/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	The example project demonstrates the typical use of the IIC master HAL module APIs.
    	The project initializes IIC master module with standard rate and interfaces with PmodACL™ Board for ADXL345.

    	On power up after establishing the connection of sensor with RA board, it displays accelerometer axis data on
    	J-Link RTT Viewer. Any API/event failure will be displayed on J-Link RTT Viewer.

2. Software Requirements:
	Refer to the Tools section in the Release Notes for set up suitable Software version.
	https://github.com/renesas/fsp/releases

3. Hardware Connections:
    	PMOD ACL has two on board connectors, J2 is used for I2C communication.
    	For EK-RA6M4/EK-RA6M5:
    	--------
    		SDA0/P401  ----> SDA (on PMOD-ACL)
    		SCL0/P400  ----> SCL (on PMOD-ACL)
    		VCC pin    ----> VCC (on PMOD-ACL)
    		GND pin    ----> GND (on PMOD-ACL)

    	For EK-RA4M3:
    	--------
    		SDA0/P401  ----> SDA (on PMOD-ACL)
    		SCL0/P408  ----> SCL (on PMOD-ACL)
    		VCC pin    ----> VCC (on PMOD-ACL)
    		GND pin    ----> GND (on PMOD-ACL)

    	For EK-RA4M2:
    	--------
    		SDA1/P206  ----> SDA (on PMOD-ACL)
    		SCL1/P205  ----> SCL (on PMOD-ACL)
    		VCC pin    ----> VCC (on PMOD-ACL)
    		GND pin    ----> GND (on PMOD-ACL)

    	For EK-RA8M1/EK-RA8D1:
    	--------
    		SDA1 P511  ----> SDA (on PMOD-ACL)
    		SCL1 P512  ----> SCL (on PMOD-ACL)
    		VCC pin    ----> VCC (on PMOD-ACL)
    		GND pin    ----> GND (on PMOD-ACL)

   	For MCK-RA8T1:
    	--------
    		SDA1/P206 (CN2:P24)  ----> SDA (on PMOD-ACL)
    		SCL1/P205 (CN2:P25)  ----> SCL (on PMOD-ACL)
    		VCC pin   (CN1:P24)  ----> VCC (on PMOD-ACL)
    		GND pin   (CN1:P34)  ----> GND (on PMOD-ACL)

    	For FPB-RA8E1:
    	--------
    		SDA1 P511 (J4:P4)  ----> SDA (on PMOD-ACL)
    		SCL1 P512 (J4:P3)  ----> SCL (on PMOD-ACL)
    		VCC pin   (J4:P1)  ----> VCC (on PMOD-ACL)
    		GND pin   (J4:P2)  ----> GND (on PMOD-ACL)

    	For EK-RA4L1:
    	--------
		Place J15 jumper to use P100, P101 for I2C channel 0.
    		SDA0 P100 (J3:P2)  ----> SDA (on PMOD-ACL)
    		SCL0 P101 (J3:P4)  ----> SCL (on PMOD-ACL)
    		VCC pin   (J3:P1)  ----> VCC (on PMOD-ACL)
    		GND pin   (J3:P25) ----> GND (on PMOD-ACL)

Note:
1) For the functioning of IIC Master on all the boards except for EK-RA8M1/EK-RA8D1/FPB-RA8E1/EK-RA4L1 external 
   pull up resistors of value 3.9 or 4.7K ohms are required to be connected on I2C (SDA/SCL) lines.﻿

2) The user should view section 5 of application note R11AN0467 on how to build and run a trustzone project.
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection

3) For the IAR project, the user can launch the Workspace using the .eww workspace file in the non-secure project folder. 
   It included multiple workspace which has both secure (_s) and non-secure (_ns) project.

4) For the KEIL project, the user can launch the Workspace using the .uvmpw file in keil folder. It included multiple workspace 
   which has both secure (_s) and non-secure (_ns) project.

5) The user must build secure project (_s) first, then build non-secure (_ns) project. 

6) After the projects are built successfully, the user should follow these steps in sequence to launch the debug session:
   - Open Renesas Device Partition Manager, then choose "Connection Type" as SWD.
   - Initialize device back to factory default. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Set Trustzone secure / non-secure boundaries. Refer to the corresponding screen shots in the R11AN0467 to perform the action.
   - Launch the Debug session from the non-secure project.

7) For IAR project, the user can change setting in the Tools -> Option -> Stack -> Uncheck "Stack pointer(s) not
   valid until program reaches" option to ignore Stack Warning at launch. 

8) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000260c
   b. Keil:	0x20002008
   c. IAR: 0x20002058
 
9) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
