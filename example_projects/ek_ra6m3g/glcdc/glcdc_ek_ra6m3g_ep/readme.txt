/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the basic functionalities of the GLCDC device driver on Renesas RA MCUs based on Renesas FSP. 
	On successful initialization of the GLCDC module, the EP will read the display resolution and frame buffer set in the GLCDC 
	module and draw color bands to cover the entire screen of external LCD connected to RA MCU. User can view raw image in e2studio IDE.
	Errors and Status information will be printed on Jlink RTT Viewer during the execution of the project.

2. Hardware Requirements:
    Renesas RA boards: EK-RA6M3G.
    1 x Micro USB cable.

3. Hardware Connections:
    Connect the USB Debug port on EK-RA6M3G to the PC using a micro USB cable.


Note : User can also view the raw image in memory monitor of the e2studio IDE using the address of g_framebuffer
       and specifying the resolution.