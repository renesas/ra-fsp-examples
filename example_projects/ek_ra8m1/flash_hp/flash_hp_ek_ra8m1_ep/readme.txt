/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates the typical use of the Flash_HP HAL module APIs. The example project demonstrate the
        code flash and data flash operations once, user provides RTT input from the available commands list. 
   	To exit from the demo, user should choose the EXIT option.

2. Hardware Requirements:
	- RA board	
	- Micro USB device cable - 1x

3. Hardware Connections:
	Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	port of the respective board. Connect the another end of this cable to USB port of the host PC. 

Note:   User is expected to enter data of size not exceeding 15 bytes.
	Operation is not guaranteed for any user input value, other than integer(i.e. float, char, special char) through RTT.
	Entering any input after EXIT option will result in error trap. Not recommended.

1) Initialize device back to factory default by using Renesas Device Partition Manager before run the example project.
   User can refer Section 5.3 "Setting up Hardware" in the document via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection
   For Renesas Device Partition Manager tool on each IDE:
   - Start E2Studio -> Run -> Renesas Debug Tools -> Renesas Device Partition Manager (Available on e2studio)

   - Start Keil -> Tools -> Device Partition Manager (User need to set up)
     User can refer to set up via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-MDK-prerequisites

   - Start IAR -> Tools -> Device Partition Manager (User need to set up)
	 User can refer to set up via link: https://renesas.github.io/fsp/_s_t_a_r_t__d_e_v.html#RASC-EWARM-prerequisites

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000b38
   b. Keil:	0x22000798
   c. IAR: 0x220007d8
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
