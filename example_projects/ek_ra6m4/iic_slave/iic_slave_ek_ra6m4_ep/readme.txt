/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    The example project demonstrates typical use of the IIC slave HAL module APIs.
    The project initializes IIC slave and IIC master module with standard rate and is made interfaced with loop-back mechanism.
    It performs Slave read and write operation continuously once initialization is successful. On successful I2C transaction
    (6 bytes), Data transceived is compared. Led blinks on data match else it is turned ON as sign of failure.
    Output message for both corresponding slave operations is displayed on RTT Viewer. Any API/event failure message is also 
    displayed.

2. Hardware Settings:
    Two jumper wires are required to establish loop back connection along IIC lines within the board with pins as mentioned below.
    RA6M3_EK/RA6M3G_EK
    --------
    Channel 0 has been used by IIC slave and channel 2 been used by IIC master
    1) Slave IIC pins
        IIC0 P401  ----> SDA
        IIC0 P408  ----> SCL
    2) Master IIC pins
        IIC2 P511  ----> SDA
        IIC2 P512  ----> SCL

    RA6M2_EK
    --------
    Channel 0 has been used by IIC slave and channel 1 been used by IIC master.
    1) Slave IIC pins
        IIC0 P401  ----> SDA
        IIC0 P400  ----> SCL
    2) Master IIC pins
        IIC1 P206  ----> SDA
        IIC1 P100  ----> SCL

    RA6M1_EK
    --------
    Channel 0 has been used by IIC slave and channel 1 been used by IIC master.
    1) Slave IIC pins
        IIC0 P401  ----> SDA
        IIC0 P400  ----> SCL
    2) Master IIC pins
        IIC1 P101  ----> SDA
        IIC1 P100  ----> SCL

    RA4M1_EK
    --------
    Channel 0 has been used by IIC slave and channel 1 been used by IIC master.
    1) Slave IIC pins
        IIC0 P401  ----> SDA
        IIC0 P400  ----> SCL
    2) Master IIC pins
        IIC1 P206  ----> SDA
        IIC1 P100  ----> SCL

    RA2A1_EK
    --------
    1) Slave IIC pins
        IIC0 P401  ----> SDA
        IIC0 P000  ----> SCL
    2) Master IIC pins
        IIC1 P400  ----> SDA
        IIC1 P109  ----> SCL

    RA4W1_EK
    --------
    1) Slave IIC pins
        IIC0 P407  ----> SDA
        IIC0 P204  ----> SCL
    2) Master IIC pins
        IIC1 P206  ----> SDA
        IIC1 P205  ----> SCL

    RA6M4_EK
    --------
    1) Master IIC pins
        IIC0 P401  ----> SDA
        IIC0 P408  ----> SCL
    2) Slave IIC pins
        IIC1 P206  ----> SDA
        IIC1 P205  ----> SCL

    RA6T1_RSSK
    --------
    1) Master IIC pins
        IIC0 P401  ----> SDA
        IIC0 P400  ----> SCL
    2) Slave IIC pins
        IIC1 P101  ----> SDA
        IIC1 P100  ----> SCL

    RA2L1_EK
    --------
    1) Master IIC pins
        IIC1 P206  ----> SDA
        IIC1 P205  ----> SCL
    2) Slave IIC pins
        IIC0 P407  ----> SDA
        IIC0 P408  ----> SCL

    RA4M3_EK
    --------
    1) Master IIC pins
        IIC1 P511  ----> SDA
        IIC1 P512  ----> SCL
    2) Slave IIC pins
        IIC0 P401  ----> SDA
        IIC0 P408  ----> SCL

    RA4M2_EK
    --------
    1) Master IIC pins
        IIC1 P206  ----> SDA
        IIC1 P205  ----> SCL
    2) Slave IIC pins
        IIC0 P401  ----> SDA
        IIC0 P400  ----> SCL

    RA6M5_EK
    --------
    1) Master IIC pins
        IIC1 P511 (Jumper J4 Pin 39) ----> SDA
        IIC1 P512 (Jumper J4 Pin 38) ----> SCL
    2) Slave IIC pins
        IIC2 P414 (J1 Pin 32) ----> SDA
        IIC2 P415 (J1 Pin 31) ----> SCL

    RA6E1_FPB
    --------
    1) Master IIC pins
        IIC1 P206 (J3 Pin 31) ----> SDA
        IIC1 P205 (J3 Pin 32) ----> SCL
    2) Slave IIC pins
        IIC0 P401 (J3 Pin 2) ----> SDA
        IIC0 P400 (J3 Pin 1) ----> SCL

    RA6T2_MCK
    ---------
    1) Master IIC pins
        IIC1 PB09(CN5:1)  ----> SDA
        IIC1 PB08(CN5:3)  ----> SCL
    2) Slave IIC pins
        IIC0 PB07(CN4:31) ----> SDA
        IIC0 PB06(CN4:33) ----> SCL

Note:
	DTC transfer support is configurable and is enabled in the configurator. DTC is helpful for minimizing interrupts during large transactions.

    For the functioning of IIC Slave on all the boards except for EK-RA6M3/EK-RA6M3G,FPB-RA6E1,EK-RA2L1,external pull up resistors of value
    3.9 or 4.7k ohms are required to be connected on I2C(SDA/SCL) lines.

1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000cc8
   b. Keil:	0x200008a0
   c. IAR: 0x200009a8
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
