/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the QSPI as block media on Renesas RA MCU's using the USB composite Class driver (PMSC + PCDC).
	In this example, the application will configure the MCU as a CDC device as well as a Mass storage device on Block
	media on QSPI. Users can use a Serial terminal (like Tera-term) to communicate with the board. Data written by the
	Tera Term will be echoed back on the terminal by the RA board. The user at the same time can read/write the data
 	from/to the board using a host machine.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4L1
	1 x Renesas RA board.
	2 x Type-C USB cables.
	1 x Host PC with at least 2 USB ports (1 for debug and 1 for COM port).

3. Hardware Connections:
        For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5:
                + Jumper J12: Connect pins 2-3.
                + Connect jumper J15 pins.
                + Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
                  port (J11) of the board. Connect the other end of this cable to a USB port of the host PC. 
 
        For EK-RA4L1:
                + The user needs to turn ON S4-4 to use USB Device.
                + Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
                + Connect the Type-C end of the Type-C USB device cable to Type-C USB Full-Speed 
                  port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

4. USB Disconnect Behaviour:
        The application also handles removing & connecting the Peripheral USB Cable use case.
        Ex: If the user removes the peripheral USB cable from the board, the Composite USB example project will be running 
        and it waits for the cable to be connected. Once the Cable is connected back, the example project works the same.

5. Verifying Operation:
	1. Build Download and Run firmware on board. Connect the cable to USB peripheral connector.        
	2. Confirm that the PC detects an unformatted media using the "Disk Management" application on Windows.
	3. Format the drive to use it as a flash drive.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000b8c
   b. Keil:     Not Available 
   c. IAR:      Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
