/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
   	This example project demonstrates basic functionalities of USB Host Mass Storage Class (HMSC) driver
    	with FAT file system with Baremetal using FreeRTOS+FAT on Renesas RA MCUs based on Renesas FSP.
    	FreeRTOS+FAT uses the blockmedia driver, which uses an underlying USB HMSC driver to perform file operations
    	on the storage devices. The project performs basic file operations (open, read, write, format and close) on
    	the USB Flash drive. Error and EP status messages will be printed on J-Link RTT Viewer.

2. Hardware Connections: 
	For EK-RA6M3, EK-RA6M3G (High-Speed):
		Jumper J7: Connect pins 1-2.
		Remove Jumper J17 pins.
		Connect USB device to J6 connector with help of micro USB Host cable.
			
	For EK-RA8M1, EK-RA6M5 (High-Speed):
		Jumper J7: Connect pins 1-2.
		Remove Jumper J17 pins.
		Connect USB device to J31 connector with help of micro USB Host cable.
 
	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA8D1, EK-RA8M1 (Full-Speed):
		Jumper J12 placement is pins 1-2.
		Remove Jumper J15 pins.
		Connect USB device to J11 connector with help of micro USB Host cable.

 	For MCK-RA8T1 (Full-Speed):
        	Jumper JP9 placement is pins 1-2
        	Remove Jumper J10 pins
        	Connect USB device to CN14 connector with help of Type-C USB Host cable.

	For EK-RA4L1 (Full-Speed):
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		The user must turn OFF S4-4 to select USB Host Controller mode.
		Connect USB device to J11 connector with help of Type-C USB Host cable.

	For EK-RA8E2 (Full-Speed):
		Connect USB device to J11 connector with help of Type-C USB Host cable.
		
Note:
1) By default, the USB HMSC Example Project runs in High-speed mode for High-Speed supported boards and in Full-Speed mode for other boards.

2) Format the USB drive (with FAT32 file system) before performing any operation.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220400c0
   b. Keil:     Not Available 
   c. IAR:      Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
