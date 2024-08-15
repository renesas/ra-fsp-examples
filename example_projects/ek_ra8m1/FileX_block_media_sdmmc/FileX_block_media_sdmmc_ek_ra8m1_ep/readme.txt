/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     The example project shows the operation of a FileX interface on blockmedia over SDHI driver on RA MCU. 
     In this sample code, the project will perform various file operations (Open, Close, Read, Write, Format), and optionally adding time stamp
     to the file, while creating the file on the SD Card.In summary it showcases the FileX functionality using BlockMedia SDMMC driver on FSP.

     JlinkRTTViewer is used as user Interface. System messages (Error and info messages) will be printed on JlinkRTTViewer during the execution of the project.

    Configuration changes to switch Bus Width in configurator: (Same for EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4 and EK-RA8M1 boards)

    For 4 Bit Width
    Under SDMMC Stack
        Bus Width: 4 Bits        

2. Hardware Requirements:
    External Hardware : PMOD based SD card Module:
                        Full sized SD Card  or Micro SD CARD with Adapter to fit into Full sized SD card Slot

3. Hardware Connections:

For EK-RA6M3:
    4 Bit Width
        J4:33-P412(CMD)    --->    PMOD SD Pin2(MOSI)
        J4:36-P411(DAT0)   --->    PMOD SD Pin3(MISO)
        J4:34-P413(CLK)    --->    PMOD SD Pin4(SCLK)
        GND                --->    PMOD SD Pin5(GND)
        +3V3(VCC)          --->    PMOD SD Pin6(VCC)
        J4:32-P415(CD)     --->    PMOD SD Pin9(CD)
        J4:31-P414(WP)     --->    PMOD SD Pin10(WP)
        J4:35-P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        J1:38-P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        J2:04-P205(DAT3)   --->    PMOD SD Pin1(CS)


For EK-RA6M4
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


For FPB_RA6E1
    1 Bit Width 
       P412(CMD)(J3 Pin20)    --->   PMOD SD Pin2(MOSI)
       P411(DAT0)(J3 Pin21)   --->   PMOD SD Pin3(MISO)
       P413(CLK)(J3 Pin19)    --->   PMOD SD Pin4(SCLK)
       GND                    --->   PMOD SD Pin5(GND)
       +3V3(VCC)              --->   PMOD SD Pin6(VCC)
       P210(CD)(J3 Pin35)     --->   PMOD SD Pin9(CD)
       P209(WP)(J3 Pin36)     --->   PMOD SD Pin10(WP)

    4 Bit Width
       Above mentioned connections for 1 Bit Width. In addition below connections

       P410(DAT1)(J3 Pin22) --->    PMOD SD Pin7(DAT1)
       P206(DAT2)(J3 Pin31) --->    PMOD SD Pin8(DAT2)
       P205(DAT3)(J3 Pin32) --->    PMOD SD Pin1(CS)


For EK_RA8M1
    *Note: User need to remove jumper J61 to use SHDI channel 0.
    Hardware connection for SDHI channel 0 as below:
    1 Bit Width
       P307(CMD)(J54 Pin5) 	---> PMOD SD Pin2(MOSI)
       P304(DAT0)(J54 Pin9) 	---> PMOD SD Pin3(MISO)
       P308(CLK)(J54 Pin4) 	---> PMOD SD Pin4(SCLK)
       GND 			---> PMOD SD Pin5(GND)
       +3V3(VCC) 		---> PMOD SD Pin6(VCC)
       P306(CD)(J54 Pin7) 	---> PMOD SD Pin9(CD)
       P305(WP)(J54 Pin8) 	---> PMOD SD Pin10(WP)

    4 Bit Width
       Above mentioned connections for 1 Bit Width. In addition below connections

       P303(DAT1)(J54 Pin6)  ---> PMOD SD Pin7(DAT1)
       P302(DAT2)(J54 Pin14) ---> PMOD SD Pin8(DAT2)
       P301(DAT3)(J54 Pin12) ---> PMOD SD Pin1(CS)

Note:
0. Connect PMOD: use short wiring connections (should be shorter than 10cm).
1. Ensure that PMOD SD Pin11(GND) is connected to GND and PMOD SD Pin12(VCC) is connected to 3.3V.
2. Format the SD Card before performing any operation.
3. EP checks if available space on media is enough to create directory and write 10k pre-defined data.
   User can modify the size of data to be written through macro WRITE_ITEM_SIZE in application code and the
   space availability for the same will be checked by EP. 
   Space required for a directory creation is 512 bytes. DIR_SPACE_REQUIRED macro can be modified accordingly,
   if additional directories are to be created by the user to check available space on media.
4. User Input "3" given once, will display required details. Multiple inputs as "3" may not give expected results
5. User is expected to enter RTT input data of size not exceeding 20 bytes.
6. Operation is not guaranteed for any user input value other than integer, char(i.e. float, special char) through
   JlinkRTTViewer input.
﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000958
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
