/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USBX Host UVC Module running on Renesas RA MCUs based on
	Renesas FSP using AzureRTOS. The USB camera will be connected as a peripheral device to the RA board which will
	act as a host device. A user menu will be provided over the RTT Viewer. The user can select option 1 to start
	video streaming and select option 2 to stop video streaming. After stopping the video streaming, the last
	captured image can be displayed on the e2studio's memory window. Error and info messages will be printed on
	J-link RTT Viewer.

2. Hardware Requirement:
	Supported RA boards: EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1, EK-RA8D2.
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x OTG Type-C cable.
	1 x USB video camera (e.g., https://www.amazon.com/Logitech-Widescreen-Correction-Noise-Reducing-FaceTime/dp/B003LVZO8S?th=1).
	1 x Host PC.

3. Hardware Connections:
	For EK-RA6M3, EK-RA6M5, EK-RA8M1 and EK-RA8D1:
		Note: For EK-RA8D1, the user needs to turn off SW1-6 to use USBHS.
		- Jumper J7: Connect pins 1-2.
		- Remove Jumper J17 pins.
		- Connect micro-AB USB Debug port (J10) of the board to USB port of the host PC via a micro USB cable.
		- Connect the USB video camera to the micro-AB USB High-Speed (J31) of the board via an OTG cable.

	For EK-RA8P1, EK-RA8D2:
		- Connect Type-C USB Debug port (J10) of the board to USB port of the host PC via a Type-C USB cable.
		- Connect the USB video camera to the Type-C USB High-Speed (J7) of the board via an OTG cable.

Note:
1) Please refer to USBX_huvc_notes.md for how to view JPEG images via memory view on e2studio.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000590
   b. Keil:  	Not Available
   c. IAR: 	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
