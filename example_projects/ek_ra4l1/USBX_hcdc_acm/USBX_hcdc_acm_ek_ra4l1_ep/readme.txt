/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     	This Example Project demonstrates the basic functionality of USBX Host CDC ACM module on Renesas RA MCUs using 2 RA boards.
	Board 1 acts as Host device running USBX HCDC-ACM example project and Board 2 acts as Peripheral device running USBX PCDC-ACM
	example project and connected with type-C USB cable via OTG. The peripheral initiates the communication and once the host reads
	the data initiated by the peripheral. The host will then send predefined data to the peripheral and the device will echo the
	data back to the host. The application status messages will be displayed on the J-Link RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1
	2 x Renesas RA boards:
		- 1 x RA board runs the USBX HCDC ACM Example Project.
		- 1 x RA board runs the USBX PCDC ACM Example Project.
    	1 x OTG cable: Type-C to USB Host (Female).
        3 x USB cables for programming and debugging.

3. Configuration Settings:
	To select USB Speed in configuration:
        i. Full-Speed: (For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1)
           USB Speed                      :    Full Speed
           USB Module Number              :    USB_IP0 Port
           DMA Source Address             :    FS Address
           DMA Destination Address        :    FS Address
           DMAC Transfer Size             :    2 Bytes

       ii. High-Speed: (For EK-RA6M3)
           USB Speed                      :    Hi Speed
           USB Module Number              :    USB_IP1 Port
           DMA Source Address             :    HS Address
           DMA Destination Address        :    HS Address
           DMAC Transfer Size             :    4 Bytes

4. Hardware Connections:
	Connect Board 1, running USBX HCDC ACM Example Project, through OTG cable to Board 2, running USBX PCDC ACM Example Project.

	For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full Speed):
         	Jumper J12 placement is pins 1-2.
         	Remove Jumper J15 pins.
         	Connect USB device to J11 connector with help of micro USB Host cable.

       	For EK-RA6M3 (High Speed):
         	Jumper J7: Connect pins 1-2.
         	Remove Jumper J17 pins.
         	Connect USB device to J6 connector with help of micro USB Host cable.

	For MCK-RA8T1 (Full Speed):
		Jumper JP9 placement is pins 1-2.
          	Remove Jumper J10 pins.
          	Connect USB device to CN14 connector with help of Type-C USB Host cable.

	For EK-RA4L1 (Full Speed):
		The user must turn OFF S4-4 to select USB Host Controller mode.
		Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		Connect USB device to J11 connector with help of Type-C USB Host cable.

5. Verifying Operation:
   	1. Import, generate and build the USBX PCDC ACM and USBX HCDC ACM EP.
   	2. Now, flash USBX PCDC ACM code on RA board (i.e Board 2) which will acts as peripheral device.
   	3. Open J-Link RTT Viewer for Board 2.
   	4. Next, flash USBX HCDC ACM code on another RA Board (i.e Board 1) which will acts as host device.
   	5. Open J-Link RTT Viewer for Board 1 and verify host device operation.

   	Link to Errata: https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

Note:
1) Both the USBX HCDC ACM and PCDC ACM boards need to be running at High Speed or Full Speed in order for the correct EP functionality.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000099c
   b. Keil: 	Not Available 
   c. IAR:  	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
