/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    This example project demonstrates basic functionalities of USBX Device Mass Storage Class module
    on Renesas RA MCUs based on Renesas FSP using Azure RTOS. RA Board will act as a device MSC 
    peripheral. The USBX PMSC driver internally configures memory device (block media with sdmmc) to 
    enumerate as a   memory drive on host PC. User can read, write and perform various file operations
    on the disk. The system initialization status and any errors occurred will be printed on the RTT
    viewer.

2. Hardware Requirements:

    2x USB A to USB Micro B Cable.(One for onboard debugger and other for USB MSC connection)
    1x PMOD based full sized SD Card or Micro SD CARD with Adapter.

3. Hardware Connections:

    User needs to confirm the default USB speed selection used in the example project prior to set up the hardware connection.

    Full Speed:(For EK-RA6M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, EK-RA6M5, EK-RA4M2, EK-RA4M3 boards)
        - Jumper j12 placement is pins 2-3
        - Connect Jumper j15 pins
        - Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	  port (J11) of the board.  Connect the other end of this cable to USB port of the host PC.
        - Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  port (J10) of the respective board.

    High Speed:(For EK-RA6M3 boards)
        - Jumper j7: Connect pins 2-3
        - Connect Jumper j17 pins
        - Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	  port (J6) of the board.  Connect the other end of this cable to USB port of the host PC.
        - Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  port (J10) of the respective board.

    High Speed:(For EK-RA6M5 boards)
        - Jumper j7: Connect pins 2-3
        - Connect Jumper j17 pins
        - Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	  port (J31) of the board.  Connect the other end of this cable to USB port of the host PC.
        - Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  port (J10) of the respective board.

For EK-RA6M3
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P415(CD)     --->    PMOD SD Pin9(CD)
        P414(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M2/EK-RA6M1
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P210(CD)     --->    PMOD SD Pin9(CD)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        Tracecut Jumper E15 and solder to E12
        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

For EK-RA6M4/EK-RA6M5/EK-RA4M3/EK-RA4M2
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P210(CD)     --->    PMOD SD Pin9(CD)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001aac
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).