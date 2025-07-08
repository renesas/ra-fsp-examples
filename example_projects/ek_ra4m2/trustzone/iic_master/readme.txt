/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    The example project demonstrates the typical use of the IIC master HAL module APIs.
    The project initializes IIC master module with standard rate and interfaces with PmodACL™ Board for ADXL345.

    On power up after establishing the connection of sensor with RA board, it displays accelerometer axis data on
    RTTviewer. Any API/event failure will be displayed on RTTviewer.

2. Hardware Settings:
    Hardware Connection
    *******************
    PMOD ACL has two on board connectors, J2 is used for I2C communication.

    RA6M4_EK / EK-RA6M5
    --------
    SDA0/P401  ----> SDA (on PMOD-ACL)
    SCL0/P400  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA4M3_EK
    --------
    SDA0/P401  ----> SDA (on PMOD-ACL)
    SCL0/P408  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

    RA4M2_EK
    --------
    SDA1/P206  ----> SDA (on PMOD-ACL)
    SCL1/P205  ----> SCL (on PMOD-ACL)
    VCC pin    ----> VCC (on PMOD-ACL)
    GND pin    ----> GND (on PMOD-ACL)

Note:
    For the functioning of IIC Master on all the boards except for EK-RA6M3/EK-RA6M3G, external pull up resistors of value
    3.9 or 4.7K ohms are required to be connected on I2C(SDA/SCL) lines.
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20002e08
   b. Keil:	0x20002428
   c. IAR: 0x20002878
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
