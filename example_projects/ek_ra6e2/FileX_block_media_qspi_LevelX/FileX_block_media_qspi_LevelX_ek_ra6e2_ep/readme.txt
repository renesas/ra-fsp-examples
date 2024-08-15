/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     The example project demonstrates the functionality of FileX middleware on blockmedia QSPI with LevelX using onboard 
     QSPI flash Adesto® AT25SF128A. In this sample code, the project will perform various file operations (Open, Close, Read,
     Write, Format, Erase), and add time stamp to the file, while creating or modifying it on the QSPI flash. In summary,
     it showcases the FileX functionality using BlockMedia QSPI with LevelX driver on FSP.

     JlinkRTTViewer is used as user Interface. System messages (Error and info messages) will be printed on JlinkRTTViewer
     during the execution of the project.

2. Hardware Connections:
     EK_RA6E2:
     Connect jumper J35 pins.

Note:
 - Format the QSPI Flash before performing any operation.
 - EP writes 10k pre-defined data. User can modify the size of data to be written through macro WRITE_ITEM_SIZE 
   in application code.
 - Operation is not guaranteed for any user input value other than an integer, char(i.e. float, special char) through
   JlinkRTTViewer input.
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000092c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
