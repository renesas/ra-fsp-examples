/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about the Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        This project showcases the AI inference capabilities of the Arm® Ethos™-U Neural Processing Unit (NPU) integrated 
	into Renesas RA microcontrollers. It implements a face detection application using a pre-trained model sourced 
	from the Arm ML-Zoo, optimized for the Ethos-U NPU via the Vela compiler. Inference is executed using TensorFlow Lite 
	for Microcontrollers (TFLM). The input image is supplied by the user as a statically defined array in C and 
	the application outputs bounding box coordinates of detected faces to the terminal.

	Note:
	- This project also includes an example demonstrating how to integrate face detection using the Arm Ethos-U55 NPU 
          with the RUHMI (Robust Unified Heterogeneous Model Integration) framework. This integration showcases 
          advanced deployment capabilities for AI applications on Renesas RA microcontrollers. 

        - Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM.
          By default, use the serial terminal (UART) via J-Link OB VCOM to print the terminal log. To use the SEGGER J-Link RTT Viewer 
          instead of the Serial Terminal, please refer to the instructions provided in the 'Special Topic' section of md file.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.1.0
	e2 studio: Version 2025-07
	SEGGER J-Link RTT Viewer: Version 8.58
	LLVM Embedded Toolchain for ARM: Version 18.1.3
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	- Supported RA boards: EK-RA8P1.
	- 1 x RA board.
	- 1 x USB Type C cable for programming and debugging.

4. Hardware Connections:

	- Connect the Type-C end of the USB-C cable to the USB Debug port (J10) on the board. Connect the other end of this cable 
	  to the USB port of the host PC.
	
Note:
1. For the Serial Terminal application: The macro USE_VIRTUAL_COM is set to 1.
	1.1 Enable echo in Tera Term: Setup → Terminal… → check Local echo.
        1.2 Serial port settings:
    	    - COM port: J-Link on-board
	    - Baud rate: 115200 bps
	    - Data length: 8 bits
	    - Parity: None
	    - Stop bit: 1
2. For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section of md file.