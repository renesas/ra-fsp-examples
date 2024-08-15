/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of USB Peripheral Vendor class driver on Renesas RA MCUs based on Renesas FSP.
	In this example, the application will configure the MCU as a Vendor Peripheral device. This Peripheral device will be connected to
	the USB Host Vendor device which is another RA board. After the enumeration is completed, the Vendor Peripheral board will read 
	15 Bytes of data from the Vendor Host board and write back the same data to the Vendor Host board. The Vendor Host and Vendor 
	Peripheral applications are designed to continuously transfer data between both the boards. User will be able to see all the 
	operation sequence and status on JLink RTTViewer.

2. Supported Boards:
	EK-RA2A1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1

3. Hardware Requirements:
    	External Hardware : USB OTG cable   - 1 nos
                            USB Cable	    - 3 nos
	EK-RA2A1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1 - Running USB PVND Example Project 
	Any RA board running USB HVND Example Project

4. Configuration changes to switch Speed in configurator: 

    i.  Full-Speed : (EK-RA2A1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, EK-RA6M5, EK-RA8M1)
        USB Speed                     :    Full Speed
        USB Module Number             :    USB_IP0 Port

    ii. High-Speed :(EK-RA6M3, EK-RA6M3G)
        USB Speed                     :    Hi Speed
        USB Module Number             :    USB_IP1 Port

5. Hardware Connections:
	Connect Board 1, running USB PVND Example Project, through OTG cable to Board 2, running USB HVND Example Project.

	EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2(Full speed)
	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J9) of the board.
	Connect the other end of this cable through OTG Cable to Board 2. 

	EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5, EK-RA8M1(Full Speed)
	Jumper J12 placement is pins 2-3
	Connect jumper J15 pins	
	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J11) of the board.
	Connect the other end of this cable through OTG Cable to Board 2.

	EK-RA6M3, EK-RA6M3G(Hi Speed)
        Jumper J7 placement is pins 2-3
	Connect jumper J17 pins
	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed port (J6) of the board.
	Connect the other end of this cable through OTG Cable to Board 2.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220011f0
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
