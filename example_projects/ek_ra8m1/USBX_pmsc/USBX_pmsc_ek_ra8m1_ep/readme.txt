/***********************************************************************************************************************
* Copyright [2020-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
*
* This software is supplied by Renesas Electronics America Inc. and may only be used with products of Renesas Electronics Corp.
* and its affiliates (“Renesas”).  No other uses are authorized.  This software is protected under all applicable laws, 
* including copyright laws.
* Renesas reserves the right to change or discontinue this software.
* THE SOFTWARE IS DELIVERED TO YOU “AS IS,” AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT 
* PERMISSIBLE UNDER APPLICABLE LAW,DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE.  TO THE MAXIMUM 
* EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE (OR ANY PERSON 
* OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING, WITHOUT LIMITATION, 
* ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES;
* ANY LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF 
* THE POSSIBILITY OF SUCH LOSS,DAMAGES, CLAIMS OR COSTS.
* **********************************************************************************************************************/

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
    (In this EP, we current use short wire connecting between PMOD and board: shorter than 10cm)

    Full Speed:(For EK-RA6M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, EK-RA6M5, EK-RA4M2, EK-RA4M3, EK-RA8M1 boards)
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

For EK_RA8M1
    *Note: User need to remove jumper J61 to use SHDI channel 0.
    Hardware connection for SDHI channel 0 as below:	
    1 Bit Width
       P307(CMD)     --->    PMOD SD Pin2(MOSI)
       P304(DAT0)    --->    PMOD SD Pin3(MISO)
       P308(CLK)     --->    PMOD SD Pin4(SCLK)
       GND 	     --->    PMOD SD Pin5(GND)
       +3V3(VCC)     --->    PMOD SD Pin6(VCC)
       P306(CD)      --->    PMOD SD Pin9(CD)
       P305(WP)      --->    PMOD SD Pin10(WP)

    4 Bit Width
       Above mentioned connections for 1 Bit Width. In addition below connections

       P303(DAT1)    --->    PMOD SD Pin7(DAT1)
       P302(DAT2)    --->    PMOD SD Pin8(DAT2)
       P301(DAT3)    --->    PMOD SD Pin1(CS)

    Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

Note:
1) By default USBX PMSC Example Project runs on High-speed mode and SDHI on 4 bit Bus width.

2) Connect PMOD: use short wiring connections (should be shorter than 10cm).

3) Ensure that PMOD SD Pin11(GND) is connected to GND and PMOD SD Pin12(VCC) is connected to 3.3V.

4) Format the SD Card before performing any operation.

5) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220012e8
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

