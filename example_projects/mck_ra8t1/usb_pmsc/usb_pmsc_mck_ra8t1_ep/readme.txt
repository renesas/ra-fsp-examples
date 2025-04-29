/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
   	This example project demonstrates basic functionalities of USB PMSC driver with FreeRTOS on Renesas RA MCUs
    	based on Renesas FSP. FreeRTOS uses the underlying Block media driver. The PMSC driver internally configures
	memory device (SD Card or on board QSPI Flash) to pop up as a memory drive on host PC.
    	Error and info messages will be printed on J-Link RTT Viewer.

	For EK-RA4L1:
	Since this board does not support the SD/MMC Host Interface (SDHI), the PMSC driver internally configures memory
	device (on board QSPI Flash) to pop up as a memory drive on host PC.

2. Hardware Requirements:
	Supported RA boards: EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1, EK-RA4L1

	For MCK-RA8T1, EK-RA4L1:
		1 x Renesas RA board.
		2 x Type-C USB cables.
		1 x PC running Windows 10.
		1 x Micro SD Card (For MCK-RA8T1).

	For other boards:
		1 x Renesas RA board.
		2 x Micro USB cables.
		1 x PC running Windows 10.
		1 x SD Card.
		1 x PMOD SD Full sized SD Card Slot.
		9 x Jumper wires.

3. Configuration Settings:
	To select USB Speed in configuration:
    	i. Full-Speed: (For EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1)
           Setting USB module to be used:    Using USB0 module
           USB Speed                    :    Full Speed
           USB Module Number            :    USB_IP0 Port

       ii. High-Speed: (For EK-RA6M3, EK-RA6M3G, EK_RA6M5, EK-RA8M1, EK-RA8D1)
           Setting USB module to be used:    Using USB1 module
           USB Speed                    :    Hi Speed
           USB Module Number            :    USB_IP1 Port

	To select Bus Width in SDHI configuration:
    	For EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
    		4 Bit Width:
    			Under SDMMC Stack
        			Bus Width: 4 Bits

    		1 Bit Width:
    			Under SDMMC Stack
        			Bus Width: 1 Bits

4. Hardware Connections:
	i. USB connection settings:
		For EK-RA6M1, EK-RA6M2 (Full-Speed): 
   			Connect the micro USB end of the micro USB device cable to micro-AB USB Full-Speed 
   			port (J9) of the board. Connect the other end of this cable to USB port of the host PC.
		  
		For EK-RA6M3, EK-RA6M3G,EK-RA6M5, EK-RA6M4, EK-RA8M1, EK-RA8D1 (Full-Speed):
   			Jumper J12 placement is pins 2-3.
        		Connect Jumper J15 pins.
        		Connect USB device to J11 connector with help of micro USB Host cable.

		For MCK-RA8T1 (Full-Speed):
        		Jumper JP9: Connect pins 2-3.
        		Connect Jumper JP10 pins.
        		Connect USB device to CN14 connector with help of Type-C USB Host cable.

		For EK-RA4L1 (Full-Speed):
			The user must turn ON S4-4 to select USB device mode.
			Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
			Connect the RA board USB FS port (J11) to the PC via Type-C USB cable.
			Connect the RA board USB debug port (J10) to the PC via Type-C USB cable for EP debugging.

		For EK-RA6M3, EK-RA6M3G (High-Speed):
        		Jumper J7: Connect pins 2-3.
        		Connect Jumper J17 pins.
        		Connect USB device to J6 connector with help of micro USB Host cable.

    		For EK-RA6M5, EK-RA8D1, EK-RA8M1 (High-Speed):
			* Note (For EK-RA8D1: The user must turn-off SW1-6 to use USBHS).
        		Jumper J7: Connect pins 2-3.
        		Connect Jumper J17 pins.
        		Connect USB device to J31 connector with help of micro USB Host cable.

       ii. SD Card connection settings:
    		For EK-RA6M1:
    			1 Bit Width
        			P412(CMD)    --->    PMOD SD Pin2(MOSI)
        			P411(DAT0)   --->    PMOD SD Pin3(MISO)
        			P413(CLK)    --->    PMOD SD Pin4(SCLK)
        			GND          --->    PMOD SD Pin5(GND)
        			+3V3(VCC)    --->    PMOD SD Pin6(VCC)
        			P209(WP)     --->    PMOD SD Pin10(WP)

    			4 Bit Width
        			Above mentioned connections for 1 Bit Width. In addition below connections
        
        			Trace cut Jumper E15 and solder to E12
        			P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        			P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        			P205(DAT3)   --->    PMOD SD Pin1(CS)
					
   		For EK-RA6M2:
    			1 Bit Width
        			P412(CMD)    --->    PMOD SD Pin2(MOSI)
        			P411(DAT0)   --->    PMOD SD Pin3(MISO)
        			P413(CLK)    --->    PMOD SD Pin4(SCLK)
        			GND          --->    PMOD SD Pin5(GND)
        			+3V3(VCC)    --->    PMOD SD Pin6(VCC)
        			P209(WP)     --->    PMOD SD Pin10(WP)

    			4 Bit Width
        			Above mentioned connections for 1 Bit Width. In addition below connections

        			Trace cut Jumper E15 and solder to E12
        			P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        			P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        			P205(DAT3)   --->    PMOD SD Pin1(CS)

		For EK-RA6M3, EK-RA6M3G:
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

    		For EK-RA6M4:
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

    		For EK-RA6M5:
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

    		For EK-RA8M1:
    			The user need to remove jumper J61 to use SHDI channel 0.

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

    		For EK-RA8D1:
    			The user must turn-off SW1-3, SW1-5, SW1-8 on board to use SDHI channel 1.

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

    		For MCK-RA8T1:
			Connect microSD Card to microSD Socket (CN12).

      iii. QSPI connection settings:
		For EK-RA4L1:
			Connect jumper J15 pins.
			E1 must be closed.
		
Note:
1) For RA board using external storage (SD Card):
   a. By default, USB PMSC Example Project runs on Full-Speed mode and SDHI on 4 bit Bus width.
   b. Connect PMOD: use short wiring connections (should be shorter than 10cm).
   c. Regarding the power supply pin: By default, users can use PMOD SD Pin5 (GND) and PMOD SD Pin6 (VCC) as shown in 
      the "Hardware Connections" section, or alternatively, PMOD SD Pin11 (GND) and PMOD SD Pin12 (VCC).
   d. Format the SD Card before performing any operation.

2) For EK-RA4L1: Before performing any file system operations, the user must format the USB drive using a Host PC with
   full permissions by following these steps:
   - A pop-up window will appear with the message:
     "You need to format the disk in drive "X": before you can use it. Do you want to format it?", the user need to
     click the "Format disk" button to choose the Format option.
   - In Format option, setting as below:
   	+ File system: FAT (Default).
	+ Allocation unit size: 16 kilobytes.
	+ Volume label: Setting name of USB drive (Optional).
	+ Format options: Enable Quick Format.
   - After finished above settings, click the "Start" button to start format process.
   - Click the "Yes" button to confirm.
   - Once the format process is finished, the user can access to USB drive and perform any file system operations.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000ad0
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
