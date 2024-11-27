/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the basic functionality of CEU driver on Renesas RA MCUs based on FSP. 
	On successful initialization of CEU and camera setup:
	
	For EK-RA8D1:  User is requested to select memory (1. SDRAM and 2. SRAM) to save image. 
		       Type '1' and enter to select 'SDRAM' to choose SXGA (1280 x 960) format and store the image in SDRAM.
		       Type '2' and enter to select 'SRAM' to choose VGA (640 x 480) format and store the image in SRAM.
	               The captured image can be viewed in e2studio using the 'image viewer'.
	
	For FPB-RA8E1: The user can press any key after initialization to capture and store a QVGA (320 x 240) image in SRAM.
	
	By default, the Example Project configures the camera in test pattern mode, and the image captured is a color bar.
	The user can get the actual image by modifying the macro OV3640_TEST_PATTERN in the ov3640.h file with a value of 0U.
	
	Enter any key to choose QVGA (320 x 240) format and store the image in SRAM.
	The captured image can be viewed in e2studio using the 'image viewer'.

2. Hardware Requirements:
     	Supported RA Boards: EK-RA8D1, FPB-RA8E1.
	1 x Renesas RA Board.
	1 x Type-C USB cable for programming and debugging.
	1 x OV3640 camera module. (Part Number: ArduCam B0156 - https://www.arducam.com/product/b0156-1-4-3-mega-pixel-m12-mount-ov3640-camera-module-with-jpeg-output/)
	
	For EK-RA8D1: the OV3640 camera module is included in the kit.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.
	
	For EK-RA8D1:
		Connect the camera module to the camera connector J59 on the EK-RA8D1 board.
		Set the configuration switches on the EK-RA8D1 board as below:
			SW1-1 PMOD1: 	OFF
			SW1-2 TRACE: 	OFF
			SW1-3 CAMERA: 	ON
			SW1-4 ETHA: 	OFF
			SW1-5 ETHB: 	OFF
			SW1-6 GLCD: 	OFF
			SW1-7 SDRAM: 	ON
			SW1-8 I3C:      OFF
	
	For FPB-RA8E1:
		Connect the camera module to the camera connector J4 on the FPB-RA8E1 board.
Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060040
   b. Keil:	Not Available
   c. IAR:      Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

3) For viewing the captured image in e2studio, please refer to the Verifying Operation section of ceu_notes.md.
