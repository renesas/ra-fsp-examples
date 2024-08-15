/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

	This project demonstrates the basic functionality of USB PHID + PHID Composite Device on Renesas RA MCUs based on Renesas FSP.
	The project configures RA board as a Keyboard and a Mouse.
	On pressing "k" from RTTViewer, MCU will work as a Keyboard. MCU sends the host PC characters from a - z & numbers 0 - 9.
	On pressing "m" from RTTViewer, MCU will work as a Mouse. MCU sends the host PC  the coordinate of the mouse which let it move square on host PC's screen.

2. Hardware Requirement:

	micro USB Cable : 2 nos.

3. Hardware connection :

	EK-RA6M2:
	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
	port (J9) of the board.  Connect the other end of this cable to USB port of the
	host PC.

	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
	port (J11) of the board.  Connect the other end of this cable to USB port of the
	host PC.

	EK-RA8M1:
	Connect the micro USB end of the micro USB device cable to micro-AB USB Debug
	port (J10) of the board.  Connect the other end of this cable to USB port of the
	host PC.
	
	Jumper J12 placement is pins 2-3.
	Connect jumper J15 pin.
	Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed
	port (J11) of the board.  Connect the other end of this cable to USB port of the
	host PC.

Note :
1. Open Device Manager, and check 'Human Interface Devices', to see ' USB Input Device' has been detected.

2. Open Notepad on the PC, lower cases a - z, and numbers 0 - 9 will be shown on the Notepad in case "k" (Keyboard) is chosen.

3. On the PC, mouse will move square shape on the screen in case "m" (Mouse) is chosen.

4. Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000a10
   b. Keil: Not Available 
   c. IAR: Not Available
 
5. If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).