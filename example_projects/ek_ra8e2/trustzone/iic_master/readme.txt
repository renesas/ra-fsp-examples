/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	The example project demonstrates the typical use of the IIC master HAL module APIs.
    	The project initializes IIC master module with standard rate and interfaces with Pmod ACL Board for ADXL345.

    	On power up after establishing the connection of sensor with RA board, it displays accelerometer axis data on
    	J-Link RTT Viewer. Any API/event failure will be displayed on J-Link RTT Viewer.

2. Software Requirements:
	Refer to the Tools section in the Release Notes for set up suitable Software version.
	https://github.com/renesas/fsp/releases

3. Hardware Requirements:
	1 x Renesas RA board.
	1 x Pmod ACL (https://digilent.com/shop/pmod-acl-3-axis-accelerometer/).
	1 x Type-C USB cable for programming and debugging.
	4 x Jumper wires used to connect the RA board to the Pmod ACL.

4. Hardware Connections:
    	Pmod ACL has two on board connectors, J2 is used for I2C communication.
    	For EK-RA6M4, EK-RA6M5:
    	--------
    		SDA0 P401  ----> SDA (on Pmod ACL)
    		SCL0 P400  ----> SCL (on Pmod ACL)
    		VCC pin    ----> VCC (on Pmod ACL)
    		GND pin    ----> GND (on Pmod ACL)

    	For EK-RA4M3:
    	--------
    		SDA0 P401  ----> SDA (on Pmod ACL)
    		SCL0 P408  ----> SCL (on Pmod ACL)
    		VCC pin    ----> VCC (on Pmod ACL)
    		GND pin    ----> GND (on Pmod ACL)

    	For EK-RA4M2:
    	--------
    		SDA1 P206  ----> SDA (on Pmod ACL)
    		SCL1 P205  ----> SCL (on Pmod ACL)
    		VCC pin    ----> VCC (on Pmod ACL)
    		GND pin    ----> GND (on Pmod ACL)

    	For EK-RA8M1, EK-RA8D1:
    	--------
    		SDA1 P511  ----> SDA (on Pmod ACL)
    		SCL1 P512  ----> SCL (on Pmod ACL)
    		VCC pin    ----> VCC (on Pmod ACL)
    		GND pin    ----> GND (on Pmod ACL)

   	For MCK-RA8T1:
    	--------
    		SDA1 P206 (CN2:24)  ----> SDA (on Pmod ACL)
    		SCL1 P205 (CN2:25)  ----> SCL (on Pmod ACL)
    		VCC pin   (CN1:24)  ----> VCC (on Pmod ACL)
    		GND pin   (CN1:34)  ----> GND (on Pmod ACL)

    	For FPB-RA8E1:
    	--------
    		SDA1 P511 (J4:4)  ----> SDA (on Pmod ACL)
    		SCL1 P512 (J4:3)  ----> SCL (on Pmod ACL)
    		VCC pin   (J4:1)  ----> VCC (on Pmod ACL)
    		GND pin   (J4:2)  ----> GND (on Pmod ACL)

    	For EK-RA4L1:
    	--------
		Place J15 jumper to use P100, P101 for I2C channel 0.
    		SDA0 P100 (J3:2)  ----> SDA (on Pmod ACL)
    		SCL0 P101 (J3:4)  ----> SCL (on Pmod ACL)
    		VCC pin   (J3:1)  ----> VCC (on Pmod ACL)
    		GND pin   (J3:25) ----> GND (on Pmod ACL)

    	For EK-RA8E2:
    	--------
    		SDA1 P511 (J4:3)  ----> SDA (on Pmod ACL)
    		SCL1 P512 (J4:5)  ----> SCL (on Pmod ACL)
    		VCC pin   (J4:1)  ----> VCC (on Pmod ACL)
    		GND pin   (J4:19) ----> GND (on Pmod ACL)

Note:
1) For the functioning of IIC Master on all the boards except for EK-RA8M1, EK-RA8D1, FPB-RA8E1, EK-RA4L1, EK-RA8E2 external
   pull up resistors of value 3.9 or 4.7K ohms are required to be connected on I2C (SDA/SCL) lines.﻿

2) The user should view "Example Project for IP Protection" section of application note R11AN0467 on how to build and run
   a trustzone project via link: https://www.renesas.com/en/document/apn/security-design-arm-trustzone-using-cortex-m33

3) The user should refer to the "RASC User Guide for MDK and IAR" section for instructions on using RASC for IAR and Keil
   via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-IAR-user-guide

4) For the IAR project, the user can launch the workspace using the .eww file in the non-secure project folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

5) For the KEIL project, the user can launch the workspace using the .uvmpw file in keil folder.
   It included multiple workspaces which has both secure (_s) and non-secure (_ns) projects.

6) The user must build secure project (_s) first, then build non-secure (_ns) project.

7) After the projects are built successfully, the user should follow these steps in sequence to launch the debug session:
   - Open Renesas Device Partition Manager (In e2studio or RASC -> Run -> Renesas Debug Tools -> Renesas Device Partition
     Manager), then choose "Connection Type" as SWD.
   - Initialize device. Refer to the corresponding screen shots in "Setting up Hardware" section of the R11AN0467 to 
     perform the action.
   - Set Trustzone secure / non-secure boundaries. Refer to the corresponding screen shots in "Download and Debug the
     Application Projects" section of the R11AN0467 to perform the action.
   - Launch the Debug session from the non-secure project.

8) For IAR project, the user can change setting in the Tools -> Option -> Stack -> Uncheck "Stack pointer(s) not
   valid until program reaches" option to ignore Stack Warning at launch.

9) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x32042450
   b. Keil:	0x32042850
   c. IAR: 0x320428ac
 
10) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
