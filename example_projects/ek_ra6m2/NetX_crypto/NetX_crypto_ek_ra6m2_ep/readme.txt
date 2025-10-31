/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

    This project demonstrates the basic functionality of Netx Crypto on Renesas RA MCUs based on SCE7 Hardware acceleration operating in an AzureRTOS environment.

    The example project executes ECC,RSA and AES algorithms(including different chaining modes) each in its own thread in multi threaded environment.

    From the RTT viewer, user can enter any key to enable concurrent execution of the different algorithms.

    The algorithms being executed, and the operational status will be displayed on Jlink RTT viewer.

2. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below instructions to observe the operation of the system.
    Supported RA Boards:
    SCE7 - EK_RA6M2 	 

3. Software Requirements:

	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
   
4. Hardware Requirements:
   
   Hardware : 
    1) RA board 
    2) 1x Micro USB device cable
    3) A PC running Windows 10 with at least 1 USB port for debug
    