/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of USB Composite driver on Renesas RA MCUs based on Renesas FSP using FreeRTOS. 
        In this example, the application will configure the MCU as a CDC device as well as a Mass storage device. 
        Tera term is used to communicate with the CDC interface of the MCU. Data written by the tera term will be echoed back on the terminal by RA board.
        User at the same time can read/write the data from/to the Mass storage device using host machine.

2. Hardware Requirements:
	1 x Renesas RA board.
	2 x micro USB cable or Type-C USB cable (For MCK-RA8T1).
	
3. Hardware Connections :
	For EK-RA6M1, EK-RA6M2: 
		- Full-Speed
	  		+ Connect the micro USB end of the micro USB device cable to micro-AB USB Full-Speed 
	  		  port (J9) of the board. Connect the other end of this cable to USB port of the host PC.

	For EK-RA4M2, EK-RA4M3, EK-RA6M4: 
		- Full-Speed	  	
			+ Jumper J12: Connect pins 2-3.
	  		+ Connect the jumper J15 pins. 
	  		+ Connect the micro USB end of the micro USB device cable to micro-AB USB Full-Speed 
	  		  port (J11) of the board. Connect the other end of this cable to USB port of the host PC. 

	For EK-RA6M3, EK-RA6M3G: 
		- High-Speed (Default)
			+ Jumper J7: Connect pins 2-3.
			+ Connect the jumper J17 pins.
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB High-Speed 
			  port (J6) of the board. Connect the other end of this cable to USB port of the host PC. 
		
		- Full-Speed
			+ Jumper J12: Connect pins 2-3.
	  		+ Connect the jumper J15 pins. 
	  		+ Connect the micro USB end of the micro USB device cable to micro-AB USB Full-Speed 
	  		  port (J11) of the board. Connect the other end of this cable to USB port of the host PC. 

	For EK-RA6M5, EK-RA8D1: 
		- High-Speed
			+ For EK-RA8D1, the user needs to turn OFF SW1-6 to use USB HS.
			+ Jumper J7: Connect pins 2-3.
			+ Connect the jumper J17 pins.
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB High-Speed 
			  port (J31) of the board. Connect the other end of this cable to USB port of the host PC. 

		- Full-Speed (Default)
			+ Jumper J12: Connect pins 2-3.
			+ Connect the jumper J15 pins. 
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB Full-Speed 
			  port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

	For EK-RA8M1: 
		- High-Speed (Default)
			+ Jumper J7: Connect pins 2-3.
			+ Connect the jumper J17 pins.
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB High-Speed 
			  port (J31) of the board. Connect the other end of this cable to USB port of the host PC. 

		- Full-Speed 
			+ Jumper J12: Connect pins 2-3.
			+ Connect the jumper J15 pins. 
			+ Connect the micro USB end of the micro USB device cable to micro-AB USB Full-Speed 
			  port (J11) of the board. Connect the other end of this cable to USB port of the host PC.

	For MCK-RA8T1: 
		- Full-Speed
			+ Jumper JP9: Connect pins 2-3.
			+ Connect the jumper JP10 pins.
			+ Connect the Type-C end of the Type-C USB device cable to Type-C USB Full-Speed 
			  port (CN14) of the board. Connect the other end of this cable to USB port of the host PC.

4. USB Disconnect Behaviour :
        The application also handles removing & connecting the Peripheral USB Cable use case.
        Ex: If the user removes the peripheral USB cable from the board, the Composite USB example project will be running 
        and it waits for the cable to be connected. Once the Cable is connected back, the example project works the same.

        Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

        Note : PMSC Disk content is retained only when the Peripheral USB cable is removed & connected back. 
               If the user removes the power cable of board, the disk content will be lost as the disk is implemented on RAM.
