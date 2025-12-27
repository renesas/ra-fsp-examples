/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The sample code accompanying this file shows the operation of a USBX Device Firmware Update (DFU) driver on
	Renesas RA MCUs based on Renesas FSP using Azure RTOS. The RA MCU board will be connected to the host PC,
	where it acts as a peripheral device. Prepare a file (.txt, .srec, .xml, .hex, etc.) to download to the RA MCU
	which has both CDC and DFU interfaces. In a real-world application, a new device firmware can be downloaded.
	In this example project, a set of data is used to demonstrate the download functionality.

		The DFU example application works in four steps:
		a. First, the system is initialized with support of the Interface Association Descriptor. The CDC and
		   DFU classes are then initialized with separate interfaces.

		b. Secondly, the device mounts as a CDC class and an application on the host (Windows PC) will exercise
		   the DFU class and send a download request. Upon reception of this command, the system un-initializes
		   the CDC class and initializes the DFU class.

		c. Then, the device will disappear from the bus for a short time (enough for the host and the device to
		   detect a reset sequence). Upon restarting, the device will be exclusively in DFU mode, downloading
		   the data sent from the host.

		d. Finally, when the new data has been downloaded, the host application resets the device, and upon 
		   re-enumeration, the device will revert to its normal operation with the new firmware. The RA MCU 
		   will display the USB enumeration status on the J-Link RTT Viewer.

		Following is the sequence used by the application.
		a. Connect two USB Cables to the PC: One for USB Debug port and other for USB FS port.

		b. Prepare a file (e.g., .txt, .hex, .xml, .srec etc) to download to the MCU.
		   Size of file must be smaller than 30KB by default. 
		   In case download file with larger size, please change value of BUFFER_SIZE macro in
		   usbx_cdc_dfu_peri_device_common.h file.
		   The example test file data.txt (29KB) is included in the EP folder.
		   For EK-RA4L1:
			Size of file must be smaller than 4KB due to the limited RAM size of this board.
			The example test file data.txt (4KB) is included in the EP folder.

		c. Launch Zadig application on Host PC and refer to md file for configuration details.

		d. Launch the command prompt in dfu_util directory (refer to md file for details) and download the file
		   to the MCU.

		e. The settings and errors will be displayed on the J-Link RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.3.0
	e2 studio: Version 2025-12
	SEGGER J-Link RTT Viewer: Version 8.92
	LLVM Embedded Toolchain for ARM: Version 21.1.1
	Zadig.exe - v2.0.1.160 (https://sourceforge.net/projects/libwdi/files/zadig/)
	dfu-util-0.11-binaries.tar.xz (https://sourceforge.net/projects/dfu-util/files/)

3. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA6M1, EK-RA6M3, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1, EK-RA4L1, EK-RA8E2,
	                     EK-RA8P1, MCK-RA8T2, EK-RA8D2
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Type-C USB cable for connecting the RA board to the host PC.
	1 x Host PC with at least 2 USB ports.

4. Hardware Connections:
	Connect USB debug port of the RA board to USB port of the host PC via a Type-C USB cable.

	For EK-RA4M2, EK-RA6M3, EK-RA6M5, EK-RA8D1, EK-RA8M1 (Full Speed):
		Jumper J12: Connect pins 2-3.
		Connect jumper J15 pins.
		Connect micro-AB USB Full Speed port (J11) of the RA board to USB port of the host PC via a micro
		USB cable.

	For EK-RA6M1 (Full Speed):
		Connect micro-AB USB Full Speed port (J9) of the RA board to USB port of the host PC via a micro
		USB cable.

	For MCK-RA8T1 (Full Speed):
		Jumper JP9: Connect pins 2-3.
		Connect jumper JP10 pins.
		Connect Type-C USB Full Speed port (CN14) of the RA board to USB port of the host PC via a Type-C
		USB cable.

	For EK-RA4L1 (Full Speed):
		The user must turn ON S4-4 to select USB device mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect Type-C USB Full Speed port (J11) of the RA board to USB port of the host PC via a Type-C
		USB cable.

	For EK-RA8E2, EK-RA8P1, EK-RA8D2 (Full Speed):
		Connect Type-C USB Full Speed port (J11) of the RA board to USB port of the host PC via a Type-C
		USB cable.

	For MCK-RA8T2 (Full Speed):
		Jumper JP6: Connect pins 2-3.
		Connect Jumper JP7 pins.
		Connect Type-C USB Full Speed port (CN18) of the RA board to USB port of the host PC via a Type-C
		USB cable.

Note:
1) This EP supports only USB Full Speed mode.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x22007bf0
   b. Keil:	Not Available
   c. IAR:	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
