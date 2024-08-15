/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     This Example Project demonstrates the basic functionality of USB Host Communication Class (HCDC) driver on Renesas RA MCUs
using 2 RA boards. Board1 (with USB HCDC Example Project running on it) communicates with Board2 (with USB PCDC Example Project 
running). Board1 initiates the communication by sending commands to Board2 and Board2 responds by sending the data. Board1 prints 
the received data on the RTTViewer.

2. Supported Boards:
	EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 and MCK-RA8T1

3. Hardware Requirements:
    External Hardware : USB OTG cable   - 1 nos
                        USB Cable	- 3 nos

	EK-RA4M2/EK-RA4M3/EK-RA6M3/EK-RA6M3G/EK-RA6M4/EK-RA6M5/EK-RA8M1/EK-RA8D1/MCK-RA8T1 - Running USB HCDC Example Project 
	Any RA board running USB PCDC Example Project

4. Configuration changes to switch Speed in configurator: 

    i. Full-Speed : (EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1 boards)
        USB Speed                      :    Full Speed
        USB Module Number              :    USB_IP0 Port

   ii. High-Speed : (EK-RA6M3, EK-RA6M3G, EK-RA8M1, EK-RA8D1 boards)
        USB Speed                      :    Hi Speed
        USB Module Number              :    USB_IP1 Port

5. Hardware Connections:
	Connect Board 1, running USB HCDC Example Project, through OTG cable to Board 2, running USB PCDC Example Project.

   Jumper Settings: 

    i. Full-Speed: (EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 boards)
        Jumper j12 placement is pins 1-2
        Remove Jumper j15 pins
        Connect USB device to J11 connector with help of micro USB Host cable.

  ii.  High-Speed:(EK-RA6M3, EK-RA6M3G)
        Jumper j7: Connect pins 1-2
        Remove Jumper j17 pins
        Connect USB device to J6 connector with help of micro USB Host cable.

 iii.  High-Speed:(EK-RA8M1, EK-RA8D1)
	Note: For EK-RA8D1, User need to turn OFF SW1-6 to use USBHS
        Jumper j7: Connect pins 1-2
        Remove Jumper j17 pins
        Connect USB device to J31 connector with help of micro USB Host cable.

  iv. Full-Speed:(MCK-RA8T1)
        Jumper jp9: Connect pins 1-2
        Remove Jumper jp10 pins
        Connect USB device to CN14 connector with help of type C USB Host cable.

    Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

Note: 	
1) By default USB HCDC Example Project (i.e., Board 1) runs in Full-speed mode.

2) RTT Print Buffer size should be configured to 512.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000978
   b. Keil:  	Not Available 
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
