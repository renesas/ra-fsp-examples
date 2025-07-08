/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionalities of USBX Host Mass Storage Class (HMSC) driver
    	with AZURERTOS+FAT file system on Renesas RA MCUs based on Renesas FSP. FileX interface on USBX uses the underlying
    	HMSC driver to perform various file operations. Error and info messages will be printed on J-Link RTT Viewer.

2. Hardware Requirements:
    	Supported RA boards: EK-RA6M4, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1
			     EK-RA8E2
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x OTG cable.
	1 x USB Pendrive.

3. Hardware Connections: 
	For EK-RA6M3 (High-Speed):
        	Jumper J7: Connect pins 1-2
        	Remove Jumper J17 pins
        	Connect USB device to J6 connector with help of micro USB Host cable.

    	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
        	Jumper J7: Connect pins 1-2
        	Remove Jumper J17 pins
        	Connect USB device to J31 connector with help of micro USB Host cable.
		Note: For EK-RA8D1, the user must turn OFF SW1-6 to use USBHS.

	For EK-RA6M4, EK-RA4M2, EK-RA4M3 (Full-Speed):
        	Jumper J12 placement is pins 1-2
       		Remove Jumper J15 pins
        	Connect USB device to J11 connector with help of micro USB Host cable.

    	For MCK-RA8T1 (Full-Speed):
        	Jumper JP9 placement is pins 1-2
        	Remove Jumper JP10 pins
        	Connect USB device to CN14 connector with help of Type-C Host cable.
		
	For EK-RA4L1 (Full-Speed):
		Turn OFF S4-4 to select USB Host mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
        	Connect USB device to J11 connector with help of Type-C USB Host cable.

	For EK-RA8E2 (Full-Speed):
        	Connect USB device to J11 connector with help of Type-C USB Host cable.

4. Configuration Settings: 
	To switch USB Speed in configuration:
     	i. Full-Speed: (For EK-RA6M4, EK-RA4M2, EK-RA4M3, MCK-RA8T1, EK-RA4L1, EK-RA8E2)
        	USB Speed                      :    Full Speed
        	USB Module Number              :    USB_IP0 Port
        	DMA Source Address             :    FS Address
        	DMA Destination Address        :    FS Address
        	DMAC Transfer Size             :    2 Bytes

	ii. High-Speed: (For EK-RA6M3, EK-RA6M5, EK-RA8M1, EK-RA8D1)
        	USB Speed                      :    Hi Speed
        	USB Module Number              :    USB_IP1 Port
        	DMA Source Address             :    HS Address
        	DMA Destination Address        :    HS Address
        	DMAC Transfer Size             :    4 Bytes

Note:
1) The user is expected to execute the "Eject" option before removing USB, or else further file operations may fail and USB data may get corrupted.

2) Operation is not guaranteed for any user input value other than int or char types (e.g., float, special characters) through
   J-Link RTT Viewer input.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220405b0
   b. Keil: 	Not Available 
   c. IAR:  	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).