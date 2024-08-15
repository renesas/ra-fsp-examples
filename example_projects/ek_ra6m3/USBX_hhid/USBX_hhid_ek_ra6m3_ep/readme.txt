/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    This example project demonstrates basic functionalities of USBX Host HID driver with AZURE RTOS on Renesas RA MCUs based
    on Renesas FSP. USBX HHID driver enumerates Human Interface Devices limited to keyboard or mouse using USBX middleware. 
    Upon Connecting a keyboard and pressing a key, the received keycode is displayed on the Jlink RTTViewer. 
    Upon connecting a mouse, the x and y axis position will be printed on Jlink RTTViewer. The updated value of x and y axis 
    prints only when there is change in X and Y Position. Error and info messages will be printed on JlinkRTTViewer.

2. Supported Boards:
    EK-RA6M4, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M5

3. Hardware Requirements:
    External Hardware : Micro USB 2.0 OTG cable  - 1x
                        USB PC Keyboard - 1 x
                        USB Mouse       - 1 x

4. Hardware Connections:
    EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3 (Only supported on these boards & with Full Speed mode)

    Full-Speed :
        Jumper J12 placement is pins 1-2
        Remove Jumper J15 pins
        Connect USB keyboard to J11 connector with help of OTG cable.

    Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086
    
    Note: EP does not support high speed as USBX HHID does not support it.

