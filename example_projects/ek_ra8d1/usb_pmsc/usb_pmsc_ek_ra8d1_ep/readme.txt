/***********************************************************************************************************************
* Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
    This example project demonstrates basic functionalities of USB PMSC driver with FreeRTOS on Renesas RA MCUs
    based on Renesas FSP. FreeRTOS uses the underlying Block media driver. The PMSC driver internally configures 
    memory device (sdcard) to pop up as a memory drive on host PC. Error and info messages will be printed on JlinkRTTViewer.

Configuration changes to switch Speed in USB configurator and Bus Width in SDHI configurator: 

    Full-Speed : Default (EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1 and EK-RA8D1 boards)
        Setting USB module to be used  :    Using USB0 module
        USB Speed                      :    Full Speed
        USB Module Number              :    USB_IP0 Port

    High-Speed : (EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA8M1 and EK-RA8D1 boards)
        Setting USB module to be used  :    Using USB1 module
        USB Speed                      :    Hi Speed
        USB Module Number              :    USB_IP1 Port

    (For EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA8M1, EK-RA8D1 boards)
    For 4 Bit Width : 
      Under SDMMC Stack
      Bus Width: 4 Bits

    For 1 Bit Width : 
      Under SDMMC Stack
      Bus Width: 1 Bits

2. Hardware Requirements:
    External Hardware : PMOD SD Full sized SD Card slot

3. Hardware Connections:

    Full Speed:(For EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G,EK-RA6M5, EK-RA6M4, EK-RA8M1, EK-RA8D1 boards)
        Jumper j12 placement is pins 2-3
        Connect Jumper j15 pins
        Connect USB device to J11 connector with help of micro USB Host cable.

    High Speed:(For EK-RA6M1, EK-RA6M2, EK-RA6M3 and EK-RA6M3G)
        Jumper j7: Connect pins 2-3
        Connect Jumper j17 pins
        Connect USB device to J6 connector with help of micro USB Host cable.

    High Speed:(For EK-RA8M1)
        Jumper j7: Connect pins 2-3
        Connect Jumper j17 pins
        Connect USB device to J31 connector with help of micro USB Host cable.

    High Speed:(For EK-RA8D1)
	*Note: User must turn-off SW1-6 to use USBHS.
        Jumper j7: Connect pins 2-3
        Connect Jumper j17 pins
        Connect USB device to J31 connector with help of micro USB Host cable.

    For EK-RA6M3 & EK-RA6M3G
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P414(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

    For EK-RA6M2
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        Tracecut Jumper E15 and solder to E12
        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

    For EK-RA6M1
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections
        
        Tracecut Jumper E15 and solder to E12
        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

    For EK-RA6M4
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections

        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

    For EK-RA6M5
    1 Bit Width
        P412(CMD)    --->    PMOD SD Pin2(MOSI)
        P411(DAT0)   --->    PMOD SD Pin3(MISO)
        P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P209(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections
        
        P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        P205(DAT3)   --->    PMOD SD Pin1(CS)

    For EK-RA8M1
    *Note: User need to remove jumper J61 to use SHDI channel 0.
    Hardware connection for SDHI channel 0 as below:
    1 Bit Width
        P307(CMD)    --->    PMOD SD Pin2(MOSI)
        P304(DAT0)   --->    PMOD SD Pin3(MISO)
        P308(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P305(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections
        
        P303(DAT1)   --->    PMOD SD Pin7(DAT1)
        P302(DAT2)   --->    PMOD SD Pin8(DAT2)
        P301(DAT3)   --->    PMOD SD Pin1(CS)

    For EK-RA8D1
    *Note: User must turn-off SW1-3, SW1-5, SW1-8 on board to use SDHI channel 1.
    Hardware connection for SDHI channel 1 as below:
    1 Bit Width
        P401(CMD)    --->    PMOD SD Pin2(MOSI)
        P402(DAT0)   --->    PMOD SD Pin3(MISO)
        P400(CLK)    --->    PMOD SD Pin4(SCLK)
        GND          --->    PMOD SD Pin5(GND)
        +3V3(VCC)    --->    PMOD SD Pin6(VCC)
        P700(WP)     --->    PMOD SD Pin10(WP)

    4 Bit Width
        Above mentioned connections for 1 Bit Width. In addition below connections
        
        P403(DAT1)   --->    PMOD SD Pin7(DAT1)
        P404(DAT2)   --->    PMOD SD Pin8(DAT2)
        P405(DAT3)   --->    PMOD SD Pin1(CS)

Note: 
1) By default USB PMSC Example Project runs on Full-speed mode and SDHI on 4 bit Bus width.

2) Connect PMOD: use short wiring connections (should be shorter than 10cm).

3) Regarding the power supply pin: By default, Users can use PMOD SD Pin5(GND) and PMOD SD Pin6(VCC) as shown in the hardware connection, 
   or alternatively, PMOD SD Pin11(GND) and PMOD SD Pin12(VCC).

4) Format the SD Card before performing any operation.

5) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000ad0
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
