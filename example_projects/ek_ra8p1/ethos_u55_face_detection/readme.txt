/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about the Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project showcases the AI inference capabilities of the Arm Ethos-U Neural Processing Unit (NPU)
	integrated into Renesas RA microcontrollers. It implements a face detection application using a pre-trained
	model sourced from the Arm ML-Zoo, optimized for the Ethos-U NPU via the Vela compiler. Inference is
	executed using TensorFlow Lite for Microcontrollers (TFLM). The input image is supplied by the user as a
	statically defined array in C and the application outputs bounding box coordinates of detected faces to
	the terminal.

	Note:
	- This project also includes an example demonstrating how to integrate face detection using the Arm
	  Ethos-U55 NPU with the RUHMI (Robust Unified Heterogeneous Model Integration) framework. This integration
	  showcases advanced deployment capabilities for AI applications on Renesas RA microcontrollers.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.6.0
	e2 studio: Version 2026-07
	LLVM Embedded Toolchain for ARM: Version 22.1.0
	Terminal Console Application: Tera Term or a similar application

3. Hardware Requirements:
	Supported RA boards: EK-RA8P1.
	1 x Renesas RA board.
	1 x USB cable for programming and debugging (USB cable type varies by board model).

4. Hardware Connections:
	Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming
	and debugging.

Note:
1) To view output logs on a serial terminal application (e.g., Tera Term), configure the settings as shown below:
	a. To make sure the received data shows properly, the user needs to set:
		[Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR].

	b. To echo back characters typed in Tera Term, the user needs to enable it through:
		[Setup] -> [Terminal...] -> Check [Local echo].

	c. The configuration parameters of the serial port on the serial terminal application:
		Port: The port is provided by the J-Link On-Board.
		Speed: 115200
		Data: 8 bit
		Parity: none
		Stop bits: 1 bit
		Flow control: none

2) For detailed instructions on running the EP and its operation, refer to the "Verifying Operation" section
   of ethos_u55_face_detection_notes file.
