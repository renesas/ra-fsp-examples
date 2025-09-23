/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the ICU HAL module APIs. The project initializes the ICU
	module. The user is requested to press the push button to trigger the external IRQ and this then will start
	toggling of user LED.
	
	For MCK-RA4T1, MCK-RA6T3: The user is requested to flip the toggle switch (OFF->ON) to trigger the external
        IRQ and this then will start toggling of the user LED.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	1 x Pmod BTN (For MCK-RA8T1: e.g., https://digilent.com/reference/pmod/pmodbtn/start).
	1 x Jumper wire (For FPB-RA2E3, FPB-RA2T1).

3. Hardware Connections:
	For MCK-RA4T1, MCK-RA6T3:
		Connect to inverter board.
		Use Switch S1 (toggle switch).

	For MCK-RA8T1:
		Connect Pmod BTN (J1:1) to PMOD (CN6:1).
		Use Switch BTN1 (push button on Pmod BTN).

	For EK-RA2A1, EK-RA6M1, EK-RA6M2, EK-RA6M5, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA4M1, EK-RA4M3, EK-RA4M2,
	    EK-RA2E1, EK-RA2L1, FPB-RA4E1, FPB-RA6E1, EK-RA4E2, EK-RA6E2, EK-RA8M1, EK-RA8D1, FPB-RA0E1, FPB-RA8E1,
	    EK-RA4L1, FPB-RA0E2 and EK-RA2L2:
		Use Switch S1 (push button).

	For RSSK-RA6T1, MCK-RA6T2, EK-RA2E2, EK-RA2A2:
		Use Switch S2 (push button).

	For FPB-RA2E3:
		Connect wire to short NMI P200 (J3:21) to P206 (J3:18).
		Use Switch S1 (push button).

	For EK-RA4W1, EK-RA8E2, EK-RA8P1, EK-RA4C1, FPB-RA0L1:
		Use Switch SW1 (push button).
		For EK-RA4C1:
			The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4
			to use the on-board debug functionality.
		For FPB-RA0L1:
			The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
			The user must close E20 to use P002 for LED1.

	For FPB-RA2T1:
		The user must close E8, E15 and cut E10 to use P213 for LED1.
		Connect wire to short NMI P200 (J3:21) to IRQ0 P206 (J3:18).
		Use Switch S1 (push button).

Note: 	
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio:	0x20004840
   b. Keil:	0x20004818
   c. IAR:	0x20004860

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called
   _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings,
   iar\Debug\List).
