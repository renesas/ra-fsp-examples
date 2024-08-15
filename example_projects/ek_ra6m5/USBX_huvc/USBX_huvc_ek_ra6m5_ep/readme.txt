/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

      This project demonstrates the basic functionality of USBX Host UVC Module running on Renesas RA MCUs based on 
      Renesas FSP using AzureRTOS. The USB camera will be connected as a peripheral device to the RA board which will 
      act as a host device. A user menu will be provided over the RTT Viewer. The User can select option 1 to start 
      video streaming and select option 2 to stop video streaming. After stopping the video streaming, the last captured
      image can be displayed on the e2studio's memory window. Error and info messages will be printed on J-link RTT Viewer.

2. Hardware Requirement:

     1. RA MCU board
     2. 1x Micro USB cables for programming and debugging
	 3. 1x Micro USB host cable (type-A female to micro-B male)
     4. 1x USB Video Camera(Logitech C310 HD WEBCAM)
	 5. Host System (Windows PC)

3. Hardware Connections:

    Supported Board (USB High Speed Required)- EK-RA6M3, EK-RA6M5:		
		1. Connect RA board to Host PC machine using micro USB cable.
        2. Connect the USB High Speed USB port of the RA MCU board to the USB Video camera using the Micro USB host cable (type-A female to micro-B male).

4. Hardware settings for the project:
	
    Jumper Settings:

	EK-RA6M3 and EK-RA6M5 (USB High Speed Required)
		1. Jumper J7: Connect pins 1-2
		2. Remove Jumper J17 pins

NOTE:
1) Please refer to USBX_huvc_notes.md for how to view JPEG images via memory view on e2studio.
2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000a14
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
