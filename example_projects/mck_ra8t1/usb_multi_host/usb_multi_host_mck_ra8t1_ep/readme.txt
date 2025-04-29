/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of USB Host Composite (CDC and MSC) on Renesas RA MCUs based
	on Renesas FSP. In this example, the application will configure the MCU as a Host Communications Device Class (HCDC)
	and as a Host Mass Storage Class (HMSC). The EP will present a menu list for user to communicate with both the PMSC
	and PCDC devices. The usb_composite EP can be used as the PCDC device and the PMSC device to demonstrate this
	USB Multi Host example. For HCDC operation, the host prompts the user to input text for transfer to the PCDC device
 	and retrieves echoed data from the PCDC device. For HMSC operation, the EP performs File Operations such as Creating,
 	Writing, Reading and Deleting of a file and USB Specific operations such as formatting, ejecting and reinitializing a
 	USB drive.

   	In case only the PCDC device is connected or only the PMSC device is connected, The EP will provide the corresponding
	operation: USB CDC operation or USB MSC operation.

	Note: 
		- Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via
		  J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to
		  the SEGGER J-Link RTT Viewer. If supported, it defaults to the serial terminal (UART).
		- To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please refer to the instructions provided in 
		  the "Special Topic" section of usb_multi_host_notes.md.
		- RA boards supported for J-Link OB VCOM: EK-RA8D1, EK-RA8M1 and MCK-RA8T1.

2. Software Requirements:
    Renesas Flexible Software Package (FSP): Version 5.9.0
    e2 studio: Version 2025-04
    SEGGER J-Link RTT Viewer: Version 8.12f
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application
   
3. Hardware Requirements:
	2 x RA board:
		1 x RA board runs the usb_composite EP or usb_pcdc EP (optional).
		1 x RA board runs the USB Multi-Host EP.
	1 x Type-C USB host cable (type-A Female to Type-C Male).
	1 x Type-C USB device cable (type-A Female to Type-C Male).
	1 x USB Flash Drive (optional).
	1 x OTG cable: Type-C to USB Host (Female).
						
4. Hardware Connections:
	The USB Multi-Host Example Project configures the USB speed mode as follows:

	For EK-RA4M2, EK-RA4M3, EK-RA6M4:
		- Full-Speed:
			+ Set jumper J12 to pins 1-2.
			+ Remove the jumper from J15.
			+ Connect the USB device to the J11 connector using a micro USB host cable.
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB Debug port (J10) of the 
			  RA board and Connect the other end of this cable to the USB port of the host PC. 

	For EK-RA6M3, EK-RA6M3G:
	  	- High-Speed (Default)
			+ Set jumper J7 to pins 1-2.
			+ Remove the jumper from J17.
			+ Connect the USB device to the J6 connector using a micro USB Host cable.
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB Debug port (J10) of the 
			  RA board and Connect the other end of this cable to the USB port of the host PC.

		- Full-Speed
			+ Set jumper J12 to pins 1-2.
			+ Remove the jumper from J15.
			+ Connect the USB device to the J11 connector using a micro USB host cable.
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB Debug port (J10) of the 
			  RA board and Connect the other end of this cable to the USB port of the host PC.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1:
		- High-Speed (Default for EK-RA8D1)
			+ For EK-RA8D1, the user needs to turn off SW1-6 to use USB HS.		
			+ Set jumper J7 to pins 1-2.
			+ Remove the jumper from J17.
			+ Connect the USB device to the J31 connector using a micro USB host cable.
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB Debug port (J10) of the 
			  RA board and Connect the other end of this cable to the USB port of the host PC.

		- Full-Speed (Default for EK-RA6M5 and EK-RA8M1)
		  	+ Set jumper J12 to pins 1-2.
			+ Remove the jumper from J15.
			+ Connect the USB device to the J11 connector using a micro USB host cable.
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB Debug port (J10) of the 
			  RA board and Connect the other end of this cable to the USB port of the host PC.

	For MCK-RA8T1:
		- Full-Speed
			+ Set jumper JP9 to pins 1-2.
			+ Remove the jumper from JP10.
			+ Connect the USB device to the CN14 connector using a Type-C USB host cable.
			+ Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Debug port (CN11) of the 
			  RA board and Connect the other end of this cable to the  USB port of the host PC. 

NOTE:
1) To change USB speed mode, please refer to the Special Topic section in "usb_multi_host_notes.md".

2) For HMSC operation:
   a. Format the USB drive (with FAT16/FAT32 file system) before performing any operation.
      Note: Due to limitations in RAM size on the MCU, it is unable to format the USB drive (FAT16 or FAT32) when testing
  	    with the usb_composite EP.
   b. Data written can also be manually verified in file ra_usb.txt.
   c. User is expected to execute "safely_eject" option before removing USB else further file operations may fail and USB
      data may get corrupted.
   d. If USB is removed without "safely_eject" option, user is recommended to perform "safely_eject" and re-initialise USB. 
      This will make sure no USB operations fail after unexpected removal of USB.
	   
3) To use the Terminal Application, configure the serial port with the following parameters:
    	- COM port is port provided by the J-Link on-board.
      	- Baud rate: 115200 bps
      	- Data length: 8-bits  
      	- Parity: none
      	- Stop bit: 1-bit
      	- Flow control: none
