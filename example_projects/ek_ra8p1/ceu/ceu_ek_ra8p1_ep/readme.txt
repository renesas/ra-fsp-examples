/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionality of CEU driver on Renesas RA MCUs based on FSP.
	On successful initialization of CEU and camera setup:

	For EK-RA8D1, EK-RA8P1:
		The user is requested to select memory (1. SDRAM and 2. SRAM) to save image.
		Type '1' and enter to select 'SDRAM' to choose SXGA (1280 x 960) format and store the image in SDRAM.
		Type '2' and enter to select 'SRAM' to choose VGA (640 x 480) format and store the image in SRAM.

	For FPB-RA8E1:
		The user can press any key after initialization to capture and store a QVGA (320 x 240) image in SRAM.

	By default, the example project configures the camera in test pattern mode, and the image captured is a
	color bar. The user can get the actual image by modifying the macro CAM_TEST_PATTERN in the cam_sensor.h file
	with a value of 0U. The captured image can be viewed in e2studio using the 'image viewer'.

2. Hardware Requirements:
	Supported RA Boards: EK-RA8D1, FPB-RA8E1, EK-RA8P1.
	1 x Renesas RA Board.
	1 x Type-C USB cable for programming and debugging.
	For EK-RA8D1, EK-RA8E1:
		1 x OV3640 camera module. (Part Number: ArduCam B0156 - https://www.arducam.com/product/b0156-1-4-3-mega-pixel-m12-mount-ov3640-camera-module-with-jpeg-output/)
	For EK-RA8P1:
		1 x OV5640 camera module. (Part Number: CU450_OV5640)

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

	For EK-RA8D1:
		Connect the camera module to the camera connector J59 on the RA board.
		Set the configuration switches on the RA board as below:
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+
		|     OFF     |     OFF     |       ON     |     OFF    |      OFF   |     OFF    |      ON     |    OFF    |
		+-------------+-------------+--------------+------------+------------+------------+-------------+-----------+

	For FPB-RA8E1:
		Connect the camera module to the camera connector J4 on the RA board.

	For EK-RA8P1:
		Connect the camera module to the Camera Expansion Port J35 on the RA board.
		Open J41 to use P405, P406 for Parallel Camera.
		Set the configuration switches on the RA board as below:
		+------------------+------------------+------------+---------------+-----------+------------+------------------+------------------+
		| SW4-1 PMOD1_SEL0 | SW4-2 PMOD1_SEL1 | SW4-3 OSPI | SW4-4 ARDUINO | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
		+------------------+------------------+------------+---------------+-----------+------------+------------------+------------------+
		|       OFF        |       OFF        |     OFF    |      OFF      |    OFF    |     ON     |       OFF        |       OFF        |
		+------------------+------------------+------------+---------------+-----------+------------+------------------+------------------+
Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000870
   b. Keil:	Not Available
   c. IAR:	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

3) For viewing the captured image in e2studio, please refer to the Verifying Operation section of ceu_notes.md.
