/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	This example project demonstrates basic functionalities of USBX Device Mass Storage Class module
    	on Renesas RA MCUs based on Renesas FSP using Azure RTOS. RA Board will act as a device MSC 
    	peripheral. The USBX PMSC driver internally configures memory device (block media with sdmmc or block media SPI flash)
	to enumerate as a memory drive on host PC. The user can read, write and perform various file operations
    	on the disk. The system initialization status and any errors occurred will be printed on the J-Link RTT Viewer.

	For EK-RA4L1:
	Since this board does not support the SD/MMC Host Interface (SDHI), the PMSC driver internally configures memory
	device (on board QSPI Flash) to pop up as a memory drive on host PC.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1,
			     MCK-RA8T1, EK-RA4L1

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

3. Hardware Connections:
	i. USB connection settings:
		The user needs to confirm the default USB speed selection used in the example project prior to set up the hardware connection.

		For EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
         		1. Jumper J12 placement is pins 2-3.
        		2. Connect Jumper j15 pins.
         		3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
	  	   	   port (J11) of the board. Connect the other end of this cable to USB port of the host PC.
         		4. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  	   	   port (J10) of the respective board.

		For MCK-RA8T1 (Full-Speed):
			1. Jumper JP9: Connect pins 2-3.
        		2. Connect Jumper JP10 pins.
        		3. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB Full Speed 
	  	   	   port (CN14) of the board. Connect the other end of this cable to USB port of the host PC.
        		4. Connect the Type-C USB end of the Type-C USB device cable to Type-C USB (DEBUG USB) 
	  	   	   port (CN11) of the respective board.

		For EK-RA4L1 (Full-Speed):
			1. The user must turn ON S4-4 to select USB device mode.
			2. Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
			3. Connect the RA board USB FS port (J11) to the PC via Type-C USB cable.
			4. Connect the RA board USB debug port (J10) to the PC via Type-C USB cable for EP debugging.

		For EK-RA6M3 (High-Speed):
        		1. Jumper J7: Connect pins 2-3.
        		2. Connect Jumper J17 pins.
        		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	  	   	   port (J6) of the board. Connect the other end of this cable to USB port of the host PC.
        		4. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  	   	   port (J10) of the respective board.
 
    		For EK-RA6M5, EK-RA8M1 (High-Speed):
         		1. Jumper J7: Connect pins 2-3.
        		2. Connect Jumper J17 pins.
        		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	  	   	   port (J31) of the board. Connect the other end of this cable to USB port of the host PC.
        		4. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  	   	   port (J10) of the respective board.

    		For EK-RA8D1 (High-Speed):
		* Note: The user must turn-off SW1-6 to use USBHS.
        		1. Jumper J7: Connect pins 2-3.
        		2. Connect Jumper J17 pins.
        		3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	  	   	   port (J31) of the board. Connect the other end of this cable to USB port of the host PC.
        		4. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	  	   	   port (J10) of the respective board.

       ii. SD Card connection settings:
		For EK-RA6M3:
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

    			Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

		For EK-RA6M1, EK-RA6M2:
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

        			Trace cut Jumper E15 and solder to E12
        			P410(DAT1)   --->    PMOD SD Pin7(DAT1)
        			P206(DAT2)   --->    PMOD SD Pin8(DAT2)
        			P205(DAT3)   --->    PMOD SD Pin1(CS)

		For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5:
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

		For EK-RA8M1:
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

		For EK-RA8D1:
    		* Note: The user must turn-off SW1-3, SW1-5, SW1-8 on board to use SDHI channel 1.
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

		For MCK-RA8T1:
    			Connect microSD Card to microSD Socket (CN12).

      iii. QSPI connection settings:
		For EK-RA4L1:
			Connect jumper J15 pins.
			E1 must be closed.

Note:
1) For RA board using external storage (SD Card):
   a. Connect PMOD: use short wiring connections (should be shorter than 10cm).
   b. Regarding the power supply pin: By default, Users can use PMOD SD Pin5 (GND) and PMOD SD Pin6 (VCC) as shown in the
      "Hardware Connections" section, or alternatively, PMOD SD Pin11 (GND) and PMOD SD Pin12 (VCC).
   c. Format the SD Card before performing any operation.

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

3) In case of changing USB mode (Full speed or High speed), for detailed configuration please refer to md file section
   "Module Configuration Notes".

4) Note that under rare situations, the user might need to run the EP in standalone mode for the drive to enumerate successfully.

5) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20001a84
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).

