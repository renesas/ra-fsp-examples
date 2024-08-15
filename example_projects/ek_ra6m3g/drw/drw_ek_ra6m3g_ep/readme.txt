/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    The Example Project demonstrates the basic functionalities of DRW device driver
    on Renesas RA MCUs based on Renesas FSP. On successful initialization of DRW and GLCDC module, the
    EP will display the frame buffer set in the GLCDC module and
    draw various shape and shape fills to cover the entire screen of external LCD connected to RA MCU. 
    Additionally, API Failure messages will be displayed on RTT Viewer.

2. Hardware Requirements:
    Renesas RA boards: EK-RA6M3G.
    1 x Micro USB cable.

3. Hardware Connections:
    Connect the USB Debug port on EK-RA6M3G to the PC using a micro USB cable.
    Connect LCD to the RA board


Note : User can also view the raw image in memory monitor of the e2studio IDE using the address of g_framebuffer
       and specifying the resolution.