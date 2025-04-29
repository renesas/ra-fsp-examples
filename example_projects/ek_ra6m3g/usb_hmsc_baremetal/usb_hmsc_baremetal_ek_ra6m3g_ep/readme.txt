/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
    This example project demonstrates basic functionalities of USB Host Mass Storage Class (HMSC) driver
    with FAT file system with Baremetal using FreeRTOS+FAT on Renesas RA MCUs based on Renesas FSP.
    FreeRTOS+FAT uses the blockmedia driver, which uses an underlying USB HMSC driver to perform file operations
    on the storage devices. The project performs basic file operations (open, read, write, format and close) on
    the USB Flash drive.Error and EP status messages will be printed on J-link RTT Viewer.

Hardware Connections:

    High-Speed: (For EK-RA6M3/EK-RA6M3G)
        Jumper J7: Connect pins 1-2
        Remove Jumper J17 pins
        Connect USB device to J6 connector with help of micro USB Host cable.

    High-Speed: (For EK-RA6M5)
        Jumper J7: Connect pins 1-2
        Remove Jumper J17 pins
        Connect USB device to J31 connector with help of micro USB Host cable.

    Full-Speed: (For EK-RA4M3/EK-RA6M3/EK-RA6M3G/EK-RA6M4/EK-RA4M2)
        Jumper J12 placement is pins 1-2
        Remove Jumper j15 pins
        Connect USB device to J11 connector with help of micro USB Host cable.  

Note:
1. By default USB HMSC Example Project runs in Hi-speed mode for Hi-speed supported boards and in Full speed for other boards.
2. Format the USB drive(with FAT32 file system) before performing any operation.
