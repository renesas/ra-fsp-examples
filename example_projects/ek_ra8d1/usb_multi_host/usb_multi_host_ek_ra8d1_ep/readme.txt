/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

   This project demonstrates the basic functionalities of USB Host Composite(CDC and MSC) on Renesas RA MCUs based on Renesas FSP.
   In this example, the application will configure the MCU as a Host Communications Device Class(HCDC) and as a Host Mass Storage
   Class(HMSC). The EP will present a menu list for user to communicate with both the PMSC and PCDC devices. The usb_composite EP 
   can be used as the PCDC device and the PMSC device to demonstrate this USB Multi Host example. For HCDC operation, the host prompts
   the user to input text for transfer to the PCDC device and retrieves echoed data from the PCDC device. For HMSC operation, the EP
   performs File Operations such as Creating, Writing, Reading and Deleting of a file and USB Specific operations such as formatting,
   ejecting and reinitializing a USB drive.

   In case only the PCDC device is connected or only the PMSC device is connected, The EP will provide the corresponding operation:
   USB CDC operation or USB MSC operation.

   The EP information will be displayed on the terminal application. The User can select between the SEGGER J-Link RTT viewer and
   the serial terminal (UART) with J-Link OB VCOM for the terminal application. Note that the EP supports the serial terminal by default.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.0.0
	e2 studio: Version 2025-04.1
	SEGGER J-Link RTT Viewer: Version 8.44a
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Terminal Console Application: Tera Term or a similar application 
   
3. Hardware Requirements:
	2 x RA board:
	  - 1 x RA board runs the usb_composite EP or usb_pcdc EP (optional).
          - 1 x RA board runs the USB Multi-Host EP.
	1 x Micro USB host cable (type-A female to micro-B male)
    	2 x Micro USB device cable (type-A male to micro-B male)
	1 x USB Flash Drive (optional).
						
4. Hardware Connections:
	The USB Multi-Host Example Project configures the USB speed mode as follows:
        For EK-RA8D1:
	  - High-Speed (Default)
	    + For EK-RA8D1, the user needs to turn off SW1-6 to use USB HS.		
            + Set jumper J7 to pins 1-2.
	    + Remove the jumper from J17.
	    + Connect the USB device to the J31 connector using a micro USB host cable.
            + Connect the micro USB end of the micro USB device cable to micro-AB USB Debug port (J10) of the 
              RA board and Connect the other end of this cable to the USB port of the host PC.

          - Full-Speed
	    + Set jumper J12 to pins 1-2.
	    + Remove the jumper from J15.
	    + Connect the USB device to the J11 connector using a micro USB host cable.
	    + Connect the micro USB end of the micro USB device cable to micro-AB USB Debug port (J10) of the 
	      RA board and Connect the other end of this cable to the USB port of the host PC.
	
	Note: To change to another USB speed mode, please refer to the Special Topic section in "usb_multi_host_notes.md".

NOTE:

For HMSC operation:
	1) Format the USB drive(with FAT16/FAT32 file system) before performing any operation.
	   Note: Due to limitations in RAM size on the MCU, it is unable to format the USB drive (FAT16 or FAT32) when testing with the usb_composite EP.
	2) Data written can also be manually verified in file ra_usb.txt.
	3) User is expected to execute "safely_eject" option before removing USB else further file operations may fail and USB data
	   may get corrupted.
	4) If USB is removed without "safely_eject" option, user is recommended to perform "safely_eject" and re-initialise USB. 
	   This will make sure no USB operations fail after unexpected removal of USB.
	   
Using Terminal Application: 
	1) By default, EP information is printed to the host PC via the Serial Terminal. To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please follow the instructions in the usb_multi_host_notes.md file.
	2) The configuration parameters of the serial port on the terminal application are as follows:
		- COM port is port provided by the J-Link on-board.
		- Baud rate: 115200 bps
		- Data length: 8-bits  
		- Parity: none
		- Stop bit: 1-bit
		- Flow control: none