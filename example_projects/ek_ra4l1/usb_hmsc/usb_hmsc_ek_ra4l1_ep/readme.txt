/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	This example project demonstrates basic functionalities of USB Host Mass Storage Class (HMSC) driver
        with FreeRTOS+FAT file system on Renesas RA MCUs based on Renesas FSP. FreeRTOS+FAT uses the underlying
        Block media driver. The Block media driver utilizes USB HMSC driver to perform file operations on the
        USB storage devices. Error and infor messages will be printed on J-Link RTT Viewer.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x OTG: Type-C USB cable.
	1 x USB Pendrive.

3. Hardware Connections:  
	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
		Set jumper J12 to pins 1-2.
                Remove the jumper from J15.
                Connect the USB device to the J11 connector using a micro USB host cable.

	For MCK-RA8T1 (Full-Speed):
        	Set jumper JP9 to pins 1-2.
                Remove the jumper from JP10.
                Connect the USB device to the CN14 connector using a Type-C USB host cable.

       For EK-RA4L1 (Full-Speed):
                Turn OFF S4-4 to select USB host mode.
                Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect the USB device to the J11 connector using a Type-C USB host cable.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
                Set jumper J7 to pins 1-2.
		Remove the jumper from J17.
		Connect the USB device to the J6 connector using a micro USB Host cable.

        For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
                For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
                Set jumper J7 to pins 1-2.
		Remove the jumper from J17.
		Connect the USB device to the J31 connector using a micro USB Host cable.

4. Configuration Settings: 
        To switch USB Speed in configuration:
        i. Full-Speed (Default): (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1,
                                      EK-RA8D1, MCK-RA8T1, EK-RA4L1)
                Setting USB module to be used  : Using USB0 module
                USB Speed                      : Full Speed
                USB Module Number              : USB_IP0 Port

                DMAC setting for Full-Speed (Default DMAC)
                        r_usb_basic stack:
                                DMA support                  : Enable
                                Transfer Source address      : Setting for FS module
                                Transfer Destination address : Setting for FS module

                        r_dmac stack for transfer and receive
                                Transfer size(for both g_transfer0 & g_transfer1): 2 Bytes
                                Activation Source(for g_transfer0)      	 : USBFS FIFO 1(DMA transfer request 1)
                                Activation Source(for g_transfer1)      	 : USBFS FIFO 0(DMA transfer request 0)

        ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1)
                Setting USB module to be used  : Using USB1 module
                USB Speed                      : Hi Speed
                USB Module Number              : USB_IP1 Port

                DMAC setting for High-Speed
                        r_usb_basic stack:
                                DMA support                  : Enable
                                Transfer Source address      : Setting for HS module
                                Transfer Destination address : Setting for HS module
                                
                        r_dmac stack for transfer and receive
                                Transfer size(for both g_transfer0 & g_transfer1): 4 Bytes
                                Activation Source(for g_transfer0)      	 : USBHS FIFO 1(DMA transfer request 1)
                                Activation Source(for g_transfer1)      	 : USBHS FIFO 0(DMA transfer request 0)

Note:
1) By default USB HMSC Example Project runs in Full-speed mode.

2) Format the USB drive (with FAT32 file system) before performing any operation.

3) Data written can also be manually verified in file ra_usb.txt.

4) The user is expected to execute "safely_eject" option before removing USB else further file operations may fail and USB data
   may get corrupted.

5) If USB is removed without "safely_eject" option, the user is recommended to perform "safely_eject" and "initialise USB" commands
   in sequence. This will make sure no USB operations fails after unexpected removal of USB.

6) The user is expected to enter RTT Viewer input data of size not exceeding 20 bytes.

7) Operation is not guaranteed for any user input value other than integer, char (i.e. float, special char) through
   J-Link RTT Viewer input.
﻿
8) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000974
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
9) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
