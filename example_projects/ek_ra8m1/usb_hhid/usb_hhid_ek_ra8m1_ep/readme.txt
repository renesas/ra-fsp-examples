/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    This example project demonstrates basic functionalities of USB HHID driver with FreeRTOS on Renesas RA 
    MCUs based on Renesas FSP. USB HHID driver configures keyboard as a hhid device. on pressing key from
    the keyboard, received keycode is decoded and displayed on the Jlink RTTViewer.
    Error and info messages will be printed on JlinkRTTViewer.

2. Hardware Requirements:
    External Hardware : USB OTG cable   - 1 nos
                        USB PC Keyboard - 1 nos

3. Hardware Connections:
    EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3, EK-RA6M3G & EK-RA8M1 (Only supported on these boards & with Full Speed mode)

    Full-Speed :
        Jumper J12 placement is pins 1-2
        Remove Jumper J15 pins
        Connect USB keyboard to J11 connector with help of OTG cable.

Note: User is requested to enter key from a to z, A to Z, 0 to 9 and any special character on keyboard.

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220015a0
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
