/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USB PHID + PHID Composite Device on Renesas RA MCUs based
	on Renesas FSP. The RA board is configured to function as either a Keyboard or a Mouse. On pressing "k" from
	the RTT Viewer, the MCU will work as a Keyboard. The MCU sends characters a to z and numbers 0 to 9 to the host
	PC. On pressing "m" from the RTT Viewer, the MCU will work as a Mouse. The MCU sends mouse coordinates to the
	host PC, making the pointer move in a square pattern on the host PC screen.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Micro USB cable for debugging and programming.
	1 x Type-C USB cable.
	1 x Host PC with at least 2 USB ports (1 for debug and 1 for COM port).

3. Hardware Connections:
	For EK-RA6M2:
		+ Connect micro-AB USB Full-Speed port (J9) of the board to USB port of the host PC via a micro USB cable.
		+ Connect micro-AB USB Debug port (J11) of the board to USB port of the host PC via a micro USB cable.

	For EK-RA8M1, EK-RA8D1:
		+ Jumper J12: Connect pins 2-3.
		+ Connect the jumper J15 pins.
		+ Connect micro-AB USB Full-Speed port (J11) of the board to USB port of the host PC via a micro USB cable.
		+ Connect micro-AB USB Debug port (J10) of the board to USB port of the host PC via a micro USB cable.

	For MCK-RA8T1:
		+ Jumper JP9: Connect pins 2-3.
		+ Connect the jumper JP10 pins.
		+ Connect Type-C USB Full-Speed port (CN14) of the board to USB port of the host PC via a Type-C USB cable.
		+ Connect Type-C USB Debug port (CN11) of the board to USB port of the host PC via a Type-C USB cable.

	For EK-RA4L1:
		+ Turn ON S4-4 to select USB device mode.
		+ Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		+ Connect Type-C USB Full-Speed port (J11) of the board to USB port of the host PC via a Type-C USB cable.
		+ Connect Type-C USB Debug port (J10) of the board to USB port of the host PC via a Type-C USB cable.

	For EK-RA8E2, EK-RA8P1:
		+ Connect Type-C USB Full-Speed port (J11) of the board to USB port of the host PC via a Type-C USB cable.
		+ Connect Type-C USB Debug port (J10) of the board to USB port of the host PC via a Type-C USB cable.

	For EK-RA2L2:
		+ Connect Type-C USB Full-Speed port (J11) of the board to USB port of the host PC via a Type-C USB cable.
		+ Connect micro-AB USB Debug port (J10) of the board to USB port of the host PC via a micro USB cable.

Note :
1) Open Device Manager, and check 'Human Interface Devices', to see ' USB Input Device' has been detected.

2) Open Notepad on the PC, lowercases a - z, and numbers 0 - 9 will be shown on the Notepad in case "k" (Keyboard) is chosen.

3) On the PC, mouse will move square shape on the screen in case "m" (Mouse) is chosen.

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200049d0
   b. Keil: 	Not Available
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).