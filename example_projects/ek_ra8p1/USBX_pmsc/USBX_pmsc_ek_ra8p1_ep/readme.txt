/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	This example project demonstrates basic functionalities of USBX Peripheral Mass Storage Class module on Renesas
    	RA MCUs based on Renesas FSP using Azure RTOS. RA board will act as a device MSC peripheral. The USBX PMSC
    	driver internally configures memory device (Block Media SD/MMC or Block Media SPI flash) to enumerate as
	a memory drive on host PC. The user can read, write and perform various file operations on the disk.
    	The system initialization status and any errors will be printed on the J-Link RTT Viewer.

	For EK-RA4L1:
	Since this board does not support the SD/MMC Host Interface (SDHI), the PMSC driver internally configures memory
	device (on board QSPI Flash) to pop up as a memory drive on host PC.

	For EK-RA8E2:
	Since this board does not support the SD/MMC Host Interface (SDHI), the PMSC driver internally configures memory
	device (on board OSPI Flash) to pop up as a memory drive on host PC.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1,
			     MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1
	1 x Renesas RA board.
	2 x USB cables.
		- 2 x Type-C USB cables: MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1.
		- 2 x Micro USB cables: All other boards.
	1 x PC running Windows 10.
	For RA boards that support the SD/MMC Host Interface (SDHI):
		- For MCK-RA8T1:
			1 x Micro SD Card.
		- For RA boards without on-board Pmod SD slot:
			1 x SD Card.
			1 x Pmod SD: Full-sized SD Card Slot (e.g., https://digilent.com/shop/pmod-sd-full-sized-sd-card-slot/).
			10 x Jumper wires.

3. Hardware Connections:
	i. USB connection settings:
		The user needs to confirm the default USB speed selection used in the example project prior to setting up
		the hardware connection.

		For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
			- Jumper J12: Connect pins 2-3.
			- Connect Jumper J15 pins.
			- Connect the RA board USB FS port (J11) to the PC via a micro USB cable.
			- Connect the RA board USB debug port (J10) to the PC via a micro USB cable for EP debugging.

		For EK-RA6M1, EK-RA6M2 (Full-Speed):
			- Connect the RA board USB device port (J9) to the PC via a micro USB cable.
			- Connect the RA board USB debug port (J11) to the PC via a micro USB cable for EP debugging.

		For MCK-RA8T1 (Full-Speed):
			- Jumper JP9: Connect pins 2-3.
			- Connect Jumper JP10 pins.
			- Connect the RA board USB FS port (CN14) to the PC via a Type-C USB cable.
			- Connect the RA board USB debug port (CN11) to the PC via a Type-C USB cable for EP debugging.

		For EK-RA4L1 (Full-Speed):
			- Turn ON S4-4 to select USB device mode.
			- Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
			- Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.
			- Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

		For EK-RA8E2, EK-RA8P1 (Full Speed):
			- Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.
			- Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

		For EK-RA6M3 (High-Speed):
			- Jumper J7: Connect pins 2-3.
			- Connect Jumper J17 pins.
			- Connect the RA board USB HS port (J6) to the PC via a micro USB cable.
			- Connect the RA board USB debug port (J10) to the PC via a micro USB cable for EP debugging.

		For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
			Note: For EK-RA8D1, The user needs to turn OFF SW1-6 to use USBHS.
			- Jumper J7: Connect pins 2-3.
			- Connect Jumper J17 pins.
			- Connect the RA board USB HS port (J31) to the PC via a micro USB cable.
			- Connect the RA board USB debug port (J10) to the PC via a micro USB cable for EP debugging.

		For EK-RA8P1 (High-Speed):
			- Connect the RA board USB HS port (J7) to the PC via a Type-C USB cable.
			- Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

	ii. SD Card connection settings:
		For EK-RA6M3:
			Hardware connection for SDHI channel 0 as below:
    			1 Bit Width:
				SD0CMD  P412  	--->    Pmod SD MOSI (J1:2)
        			SD0DAT0 P411  	--->    Pmod SD MISO (J1:3)
        			SD0CLK  P413  	--->    Pmod SD SCK  (J1:4)
        			GND	   	--->    Pmod SD GND  (J1:5)
        			VCC	   	--->    Pmod SD VCC  (J1:6)
				SD0CD 	P415    --->    Pmod SD CD   (J1:9)
        			SD0WP   P414   	--->    Pmod SD WP   (J1:10)
    			4 Bits Width:
        			Above mentioned connections for 1 Bit Width. In addition below connections
        			SD0DAT1 P410  	--->    Pmod SD DAT1 (J1:7)
        			SD0DAT2 P206	--->    Pmod SD DAT2 (J1:8)
        			SD0DAT3 P205	--->    Pmod SD CS   (J1:1)

			Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

		For EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M4, EK-RA6M5:
			Hardware connection for SDHI channel 0 as below:
    			1 Bit Width:
        			SD0CMD  P412  	--->    Pmod SD MOSI (J1:2)
        			SD0DAT0 P411  	--->    Pmod SD MISO (J1:3)
        			SCD0CLK P413  	--->    Pmod SD SCK  (J1:4)
        			GND	   	--->    Pmod SD GND  (J1:5)
        			VCC	   	--->    Pmod SD VCC  (J1:6)
				SCD0CD  P210   	--->    Pmod SD CD   (J1:9)
    				SCD0WP  P209   	--->    Pmod SD WP   (J1:10)
    			4 Bits Width:
        			Above mentioned connections for 1 Bit Width. In addition below connections
				Note: For EK-RA6M1 and EK-RA6M2, cut Jumper E15 and solder to E12 to use P205.
        			SCD0DAT1 P410  	--->    Pmod SD DAT1 (J1:7)
        			SCD0DAT2 P206	--->    Pmod SD DAT2 (J1:8)
        			SCD0DAT3 P205	--->    Pmod SD CS   (J1:1)

		For EK-RA8M1:
    			The user needs to remove jumper J61 to use SHDI channel 0.
    			Hardware connection for SDHI channel 0 as below:
    			1 Bit Width:
        			SD0CMD  P307 	--->    Pmod SD MOSI (J1:2)
        			SD0DAT0 P304   	--->    Pmod SD MISO (J1:3)
        			SD0CLK  P308    --->    Pmod SD SCK  (J1:4)
        			GND           	--->    Pmod SD GND  (J1:5)
        			VCC    		--->    Pmod SD VCC  (J1:6)
				SD0CD   P503    --->    Pmod SD CD   (J1:9)
        			SD0WP   P305	--->    Pmod SD WP   (J1:10)
    			4 Bits Width:
        			Above mentioned connections for 1 Bit Width. In addition below connections
        			SD0DAT1 P303	--->    Pmod SD DAT1 (J1:7)
        			SD0DAT2 P302	--->    Pmod SD DAT2 (J1:8)
        			SD0DAT3 P301	--->    Pmod SD CS   (J1:1)

		For EK-RA8D1:
    			The user must turn OFF SW1-3, SW1-5, SW1-8 on board to use SDHI channel 1.
    			Hardware connection for SDHI channel 1 as below:
    			1 Bit Width:
        			SD1CMD  P401	--->    Pmod SD MOSI (J1:2)
        			SD1DAT0 P402 	--->    Pmod SD MISO (J1:3)
        			SD1CLK  P400  	--->    Pmod SD SCK  (J1:4)
        			GND            	--->    Pmod SD GND  (J1:5)
        			VCC    	       	--->    Pmod SD VCC  (J1:6)
				SD1CD  P406     --->    Pmod SD CD   (J1:9)
        			SD1WP   P700    --->    Pmod SD WP   (J1:10)
    			4 Bits Width:
        			Above mentioned connections for 1 Bit Width. In addition below connections
        			SD1DAT1 P403   	--->    Pmod SD DAT1 (J1:7)
        			SD1DAT2 P404   	--->    Pmod SD DAT2 (J1:8)
        			SD1DAT3 P405   	--->    Pmod SD CS   (J1:1)

		For MCK-RA8T1:
			Connect microSD Card to microSD Socket (CN12).

		For EK-RA8P1:				
			Hardware connection for SDHI channel 0 as below:
			1 Bit Width:
				SD0CMD 	PD04 (J2:32) 	--->    Pmod SD MOSI (J1:2)
				SD0DAT0 PD03 (J2:31) 	--->    Pmod SD MISO (J1:3)
				SD0CLK 	PD05 (J2:27) 	--->    Pmod SD SCK  (J1:4)
				GND 	     (J2:39)	--->    Pmod SD GND  (J1:5)
				VCC	     (J2:1)	--->    Pmod SD VCC  (J1:6)
				SD0CD   P503 (J37:14) 	--->    Pmod SD CD   (J1:9)
				SD0WP 	PD06 (J3:4) 	--->    Pmod SD WP   (J1:10)
			4 Bits Width:
				Above mentioned connections for 1 Bit Width. In addition below connections
				SD0DAT1 PD02 (J2:33)	--->    Pmod SD DAT1 (J1:7)
				SD0DAT2 PD01 (J2:34)	--->    Pmod SD DAT2 (J1:8)
				SD0DAT3 P111 (J1:3)	--->    Pmod SD CS   (J1:1)

	iii. QSPI connection settings:
		For EK-RA4L1:
			Connect jumper J15 pins.
			E1 must be closed.

	iv. OSPI connection settings:
		For EK-RA8E2:
			Turn OFF SW4-3 to use OSPI flash.

Note:
1) For RA board using external storage (SD Card):
   a. By default, USB PMSC Example Project runs in High-Speed mode (if supported) and uses SDHI in 4-bit bus width mode.
   b. Connect Pmod: use short wiring connections (should be shorter than 10cm).
   c. Regarding the power supply pin: By default, users can use Pmod SD GND (J1:5) and Pmod SD VCC (J1:6) as shown in
      the "Hardware Connections" section, or alternatively, Pmod SD GND (J1:11) and Pmod SD VCC (J1:12).
   d. Format the SD Card before performing any operation.

2) For EK-RA4L1, EK-RA8E2: Before performing any file system operations, the user must format the USB drive using a
   Host PC with full permissions by following these steps:
   - A pop-up window will appear with the message:
     "You need to format the disk in drive "X": before you can use it. Do you want to format it?", the user needs to
     click the "Format disk" button to choose the Format option.
   - In Format option, setting as below:
   	+ File system: FAT (Default).
	+ Allocation unit size: 16 kilobytes.
	+ Volume label: Setting name of USB drive (Optional).
	+ Format options: Enable Quick Format.
   - After finished above settings, click the "Start" button to start format process.
   - Click the "Yes" button to confirm.
   - Once the format process is finished, the user can access to USB drive and perform any file system operations.

3) The user should use the host PC with full access permissions to perform any file operations on the disk
   (e.g., create, write, read, open, delete).

4) In case of changing USB mode (Full Speed or High Speed), for detailed configuration please refer to
   "Module Configuration Notes" section in USBX_pmsc_notes.md file.

5) Note that under rare situations, the user might need to run the EP in standalone mode for the drive to
  enumerate successfully.

6) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000650
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
7) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
