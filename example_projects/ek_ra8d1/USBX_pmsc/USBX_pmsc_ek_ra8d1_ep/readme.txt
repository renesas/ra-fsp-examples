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

    	2x USB A to USB Micro B Cable.(One for onboard debugger and other for USB MSC connection).
    	1x PMOD based full sized SD Card or Micro SD CARD with Adapter.

3. Hardware Connections:

    User needs to confirm the default USB speed selection used in the example project prior to set up the hardware connection. 
    (In this EP, we current use short wire connecting between PMOD and board: shorter than 10cm)

    Full Speed:(For EK-RA6M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, EK-RA6M5, EK-RA4M2, EK-RA4M3, EK-RA8M1, EK-RA8D1 boards)
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
 
    High Speed:(For EK-RA6M5, EK-RA8M1 boards)
        - Jumper j7: Connect pins 2-3
        - Connect Jumper j17 pins
        - Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	  port (J31) of the board.  Connect the other end of this cable to USB port of the host PC.
        - Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  port (J10) of the respective board.

    High Speed:(For EK-RA8D1 boards)
	*Note: User must turn-off SW1-6 to use USBHS.
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

    For EK-RA8M1
    1 Bit Width
        P811(CMD)    --->    PMOD SD Pin2(MOSI)
        P812(DAT0)   --->    PMOD SD Pin3(MISO)
        P810(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P503(CD)     --->    PMOD SD Pin9(CD)
        P504(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P500(DAT1)   --->    PMOD SD Pin7(DAT1)
        P501(DAT2)   --->    PMOD SD Pin8(DAT2)
        P502(DAT3)   --->    PMOD SD Pin1(CS)

    For EK-RA8D1
    *Note: User must turn-off SW1-3, SW1-5, SW1-8 on board to use SDHI channel 1.
    Hardware connection for SDHI channel 1 as below:
    1 Bit Width
        P401(CMD)    --->    PMOD SD Pin2(MOSI)
        P402(DAT0)   --->    PMOD SD Pin3(MISO)
        P400(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P406(CD)     --->    PMOD SD Pin9(CD)
        P700(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P403(DAT1)   --->    PMOD SD Pin7(DAT1)
        P404(DAT2)   --->    PMOD SD Pin8(DAT2)
        P405(DAT3)   --->    PMOD SD Pin1(CS)

    Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

Note: 
1) Connect PMOD: use short wiring connections (should be shorter than 10cm).

2) Regarding the power supply pin: By default, Users can use PMOD SD Pin5(GND) and PMOD SD Pin6(VCC) as shown in the hardware connection, 
   or alternatively, PMOD SD Pin11(GND) and PMOD SD Pin12(VCC).

3) Format the SD Card before performing any operation.

4) In case of changing USB mode (Full speed or High speed), For detailed configuration please refer to md file section "Module Configuration Notes".

5) Note that under rare situations, the user might need to run the EP in standalone mode for the drive to enumate successfully.

6) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22001b00
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
7) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

