/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the Flash_HP HAL module APIs. The example project demonstrate the
        code flash and data flash operations once, user provides RTT input from the available commands list. 
   	To exit from the demo, user should choose the EXIT option.

2. Hardware Requirements:
	Supported RA boards: CK-RA6M5, EK-RA4E2, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, 
			     EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8M1, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, 
			     MCK-RA8T1, RSSK-RA6T1, FPB-RA8E1
 	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type C USB cable

Note:   
1) User is expected to enter data of size not exceeding 15 bytes.
   Operation is not guaranteed for any user input value, other than integer(i.e. float, char, special char) through RTT.
   Entering any input after EXIT option will result in error trap. Not recommended.

2) Initialize device back to factory default by using Renesas Device Partition Manager before run the example project.
   User can refer Section 5.3 "Setting up Hardware" in the document via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection
   For Renesas Device Partition Manager tool on each IDE:
   	a. Start E2Studio -> Run -> Renesas Debug Tools -> Renesas Device Partition Manager (Available on e2studio)   
   	b. Start Keil -> Tools -> Device Partition Manager (User need to set up)
      	   User can refer to set up via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-prerequisites		
   	c. Start IAR -> Tools -> Device Partition Manager (User need to set up)
	   User can refer to set up via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-EWARM-prerequisites

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22060680
   b. Keil:	0x22060e30
   c. IAR: 0x22060f34

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
