/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

    This project demonstrates the basic functionality of Netx Crypto on Renesas RA MCUs based on RSIP7 Hardware acceleration operating in an AzureRTOS environment.

The example project executes ECC,RSA and AES algorithms(including different chaining modes) each in its own thread in multi threaded environment.

From the RTT viewer, user can enter any key to enable concurrent execution of the different algorithms.

The algorithms being executed, and the operational status will be displayed on Jlink RTT viewer.



2. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below instructions to observe the operation of the system.
	Supported RA Boards:
	SCE9 - EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5
	RSIP7 - EK-RA8M1, EK-RA8D1, MCK-RA8T1

3. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
   
4. Hardware Requirements:
   
   Hardware : 
	1) RA board 
	2) 1x Type C USB device cable
	3) A PC running Windows 10 with at least 1 USB port for debug

NOTE:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22001a00
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).