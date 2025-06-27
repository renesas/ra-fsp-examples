/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the basic functionality of the MRAM driver on Renesas RA MCUs using the Renesas
	Flexible Software Package (FSP). The user can interact with the application via RTT input to execute write, read,
 	and erase operations, starting at a fixed MRAM address. The operation status and data content are displayed in
	the RTT Viewer.

2. Hardware Requirements:
	Supported RA boards: EK-RA8P1
 	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type-C USB cable.

Note:
1) The user is expected to enter data not exceeding 15 bytes in size. Operation is not guaranteed for any user input
   value other than integer (e.g., float, char, special char) through RTT Viewer. Providing any input after selecting
   the EXIT option is invalid.

2) Initialize device back to factory default by using "Renesas Device Partition Manager" before run the example project.
   The user can refer to section "5.3 Setting up Hardware" in the document via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection
   For "Renesas Device Partition Manager" tool on each IDE:
   	- Start E2Studio -> Run -> Renesas Debug Tools -> Renesas Device Partition Manager (Available on e2studio)

3) Before flashing or debugging this example project, ensure that "Set TrustZone secure/non-secure boundaries" is set
   to "No" in the "Debug Configurations". The setting can be found at:
   Right-click on the project > Debug As > Debug Configurations... > Renesas GDB Hardware Debugging > mram_ep Debug_Flat
    > Debugger > Connection Settings > TrustZone > Set TrusZone secure/non-secure boundaries

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000650
   b. Keil:	Not available
   c. IAR: 	Not available

5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
