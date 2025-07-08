/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

      The sample code accompanying this file shows the operation of a USBX Device Firmware Update (DFU) driver on Renesas 
	  RA MCUs based on Renesas FSP using Azure RTOS. The RA MCU board will be connected to the host PC, where it acts as 
	  a peripheral device. Prepare a file (.txt,.srec,.xml,.hex, etc.) to download to the RA MCU which has both CDC and 
	  DFU interfaces. In a real-world application, a new device firmware can be downloaded. In this example project, a 
	  set of data is used to demonstrate the download functionality.

      The DFU example application works in four steps:

		a. First, the system is initialized with support of the Interface Association Descriptor. The CDC and DFU classes are 
		   then initialized with separate interfaces.

		b. Secondly, the device mounts as a CDC class and an application on the host (Windows PC) will exercise the DFU class 
		   and send a download request. Upon reception of this command, the system un-initializes the CDC class and initializes 
		   the DFU class.

		c. Then, the device will disappear from the bus for a short time (enough for the host and the device to detect a reset 
		   sequence). Upon restarting, the device will be exclusively in DFU mode, downloading the data sent from the host.

		d. Finally, when the new data has been downloaded, the host application resets the device, and upon re-enumeration, 
		   the device will revert to its normal operation with the new firmware. The RA MCU will display the USB enumeration 
		   status on the J-Link RTT Viewer.

      Following is the sequence used by the application.

        a. Connect two USB Cables to the PC : One for USB Debug port and other for USB FS port.
        b. Prepare a  file (e.g. .txt, .hex, .xml, .srec etc) to download to the MCU.
        c. Launch Zadig application on Host PC and refer to md file for configuration details.
        d. Launch the command prompt in dfu_util directory(refer to md file for details) and download the file to the MCU.
        e. The settings and errors will be displayed on the J-Link RTT Viewer.
	
2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.0.0
	e2 studio: Version 2025-04.1
	SEGGER J-Link RTT Viewer: Version 8.44a
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Zadig.exe - v2.0.1.160 (https://sourceforge.net/projects/libwdi/files/zadig/)
	dfu-util-0.11-binaries.tar.xz (https://sourceforge.net/projects/dfu-util/files/)

3. Hardware Requirement:

        a. RA MCU board
        b. 2x Micro USB cables
        c. Host System (Windows PC) with at least 2 USB ports

4. Hardware Connections:

        Supported Board(Full Speed) - EK-RA6M3,EK-RA6M5,EK-RA6M1,EK-RA4M2.		
        a. Connect RA board to Host PC machine using micro USB cable.
        b. Connect another cable to the Full Speed port of the RA MCU board.
		
		Hardware settings for the project:
	
		1. Jumper Settings:

			EK-RA6M3,EK-RA6M5,EK-RA4M2 (Full Speed)
			a. Jumper J12: Connect pins 2-3
			b. Connect jumper J15 pins
			
		2. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port of the board.
			EK-RA6M1: USB Full Speed port(J9)
			EK-RA6M3,EK-RA6M5,EK-RA4M2: USB Full Speed port(J11)
		3. Connect the other end of this cable to USB port of the host PC.


1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20008c9c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
