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
    EK-RA6M5, EK-RA6M4, EK-RA4M3, EK-RA4M2, EK-RA6M3 & EK-RA6M3G (Only supported on these boards & with Full Speed mode)

    Full-Speed :
        Jumper J12 placement is pins 1-2
        Remove Jumper J15 pins
        Connect USB keyboard to J11 connector with help of OTG cable.

    Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

Note : User is requested to enter key from a to z, A to Z, 0 to 9 and any special character on keyboard.