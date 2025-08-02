/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the functionality of FileX middleware on blockmedia QSPI with LevelX using
	onboard QSPI flash Macronix MX25L25645G. In this sample code, the project will perform various file operations
	(Open, Close, Read, Write, Format, Erase), and add time stamp to the file, while creating or modifying it on
	the QSPI flash. In summary, it showcases the FileX functionality using BlockMedia QSPI with LevelX driver on FSP.

     	J-Link RTT Viewer is used as user Interface. System messages (Error and info messages) will be printed on
	J-Link RTT Viewer during the execution of the project.

2. Hardware Requirements:
	Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4L1, EK-RA4C1
	1 x Renesas RA board.
	1 x Type C USB cable for programming and debugging.
	1 x Host Machine.

3. Hardware Connections:
     	For EK-RA6E2:
		Connect jumper J35 pins.
	For EK-RA4L1:
		Connect jumper J15 pins.
		E1 must be closed.
	For EK-RA4C1:
		The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use
		the on-board debug functionality.

Note:
1) EK-RA6E2 board uses onboard QSPI flash Adesto® AT25SF128A.

2) Format the QSPI Flash before performing any operation.

3) The EP writes 10k pre-defined data. The user can modify the size of data to be written through macro WRITE_ITEM_SIZE in application code.

4) Operation is not guaranteed for any user input value other than an integer, char (i.e. float, special char) through
   J-Link RTT Viewer input.

5) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000d24
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

