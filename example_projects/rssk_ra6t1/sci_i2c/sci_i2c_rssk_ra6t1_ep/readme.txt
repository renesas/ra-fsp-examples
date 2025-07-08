/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    This Example Project demonstrates SCI_I2C Master operation through loop-back with IIC Slave driver.
    6 bytes of data will be transmitted and received continuously on successful initialization. 
    The transmitted data is compared with the received data. If the data matches, on-board LED
    starts blinking. On a data mismatch, LED stays ON.
    Failure messages and status is displayed on RTTViewer.

    LED output Status on Master TX and RX data mismatch(failure) and data match(success)
    a) Failure  - Led is set as ON
    b) Success  - Led blinks for each transaction

2. Hardware Settings:
    Two jumper wires are required to establish loop back connection along IIC lines within the board with pins as mentioned below.

    RA6M3_EK & RA6M3G_EK
    --------
    Channel 0 has been used by SCI_I2C Master and IIC Slave.
    1) SCI_I2C Master pins
        SCI0 P411 (Jumper J3 Pin 36) ----> SDA
        SCI0 P410 (Jumper J3 Pin 35) ----> SCL
    2) Slave IIC pins
        IIC0 P401 (Jumper J3 Pin 09) ----> SDA
        IIC0 P408 (Jumper J3 Pin 37) ----> SCL

    RA6M2_EK
    --------
    Channel 0 has been used by SCI_I2C Master and IIC Slave.
    1) SCI_I2C Master pins
        SCI0 P411 (Jumper J2 Pin 20) ----> SDA
        SCI0 P410 (Jumper J2 Pin 02) ----> SCL
    2) Slave IIC pins
        IIC0 P401 (Jumper J4 Pin 11) ----> SDA
        IIC0 P400 (Jumper J4 Pin 13) ----> SCL

    RA6M1_EK
    --------
    Channel 0 has been used by SCI_I2C Master and IIC Slave.
    1) SCI_I2C Master pins
        SCI0 P411 (Jumper J2 Pin 20) ----> SDA
        SCI0 P410 (Jumper J4 Pin 14) ----> SCL
    2) Slave IIC pins
        IIC0 P401 (Jumper J1 Pin 03) ----> SDA
        IIC0 P400 (Jumper J1 Pin 01) ----> SCL

    RA6M4_EK
    --------
    Channel 7 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
    1) SCI_I2C Master pins
        SCI7 P613 (Jumper J3 Pin 16) ----> SDA
        SCI7 P614 (Jumper J3 Pin 17) ----> SCL
    2) Slave IIC pins
        IIC1 P511 (Jumper J2 Pin 38) ----> SDA
        IIC1 P512 (Jumper J2 Pin 37) ----> SCL

    RA6T1_RSSK
    --------
    Channel 0 has been used by SCI_I2C Master and IIC Slave.
    1) SCI_I2C Master pins
        SCI0 P101 (CN8- pin 2)   ----> SDA
        SCI0 P100 (CN8- pin 3)   ----> SCL
    2) Slave IIC pins
        IIC0 P401 (CN6- pin 1)   ----> SDA
        IIC0 P400 (CN9- pin 1)   ----> SCL

    RA4W1_EK
    --------
    Channel 0 has been used by SCI_I2C Master and IIC Slave.
    1) SCI_I2C Master pins
        SCI0 P101 (Header CN7 pin 26) ----> SDA
        SCI0 P100 (Header CN7 pin 27) ----> SCL
    2) Slave IIC pins
        IIC0 P407 (Header CN7 Pin 01) ----> SDA
        IIC0 P204 (Header CN7 Pin 09) ----> SCL

    RA4M1_EK
    --------
    Channel 0 has been used by SCI_I2C Master and IIC Slave.
    1) SCI_I2C Master pins
        SCI0 P411 (Jumper J2 Pin 04) ----> SDA
        SCI0 P410 (Jumper J2 Pin 02) ----> SCL
    2) Slave IIC pins
        IIC0 P401 (Jumper J2 Pin 08) ----> SDA
        IIC0 P400 (Jumper J2 Pin 37) ----> SCL

    RA2A1_EK
    --------
    Channel 0 has been used by SCI_I2C Master and IIC Slave.
    1) SCI_I2C Master pins
        SCI0 P302 (Jumper J2 Pin 04) ----> SDA
        SCI0 P301 (Jumper J2 pin 02) ----> SCL
    2) Slave IIC pins
        IIC0 P401 (Jumper J1 Pin 03) ----> SDA
        IIC0 P000 (Jumper J1 Pin 01) ----> SCL
		
    RA2L1_EK
    --------
    Channel 0 has been used by SCI_I2C Master and IIC Slave.
    1) SCI_I2C Master pins
        SCI0 P205 (Jumper J1 Pin 15) ----> SDA
        SCI0 P206 (Jumper J1 pin 14) ----> SCL
    2) Slave IIC pins
        IIC0 P401 (Jumper J4 Pin 08) ----> SDA
        IIC0 P400 (Jumper J4 Pin 07) ----> SCL

    RA4M3_EK
    --------
    Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
    1) SCI_I2C Master pins
        SCI9 P602 (Jumper J3 Pin 28) ----> SDA
        SCI9 P601 (Jumper J3 Pin 29) ----> SCL
    2) Slave IIC pins
        IIC1 P511 (Jumper J2 Pin 38) ----> SDA
        IIC1 P512 (Jumper J2 Pin 37) ----> SCL

    RA4M2_EK
    ---------
   Channel 9 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
   1) SCI_I2C Master pins
        SCI9 P602 (Jumper J3 Pin 28) ----> SDA
        SCI9 P601 (Jumper J3 Pin 29) ----> SCL 
   2) Slave IIC pins
        IIC0 P401 (J1 Pin 03) ----> SDA
        IIC0 P400 (J1 Pin 04) ----> SCL
   
    RA2E1_EK
    ---------
   Channel 2 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
   1) SCI_I2C Master pins
        SCI2 P302 (Jumper J27 Pin 02) ----> SDA
        SCI2 P301 (Jumper J27 Pin 01) ----> SCL 
   2) Slave IIC pins
        IIC0 P407 (J2 Pin 09) ----> SDA
        IIC0 P408 (J2 Pin 08) ----> SCL

   RA6M5_EK
   ---------
   Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
   1) SCI_I2C Master pins
        SCI9 P602 (Jumper J3 Pin 29) ----> SDA
        SCI9 P601 (Jumper J3 Pin 30) ----> SCL 
   2) Slave IIC pins
        IIC1 P511  (Jumper J4 Pin 39)  ----> SDA
        IIC1 P512  (Jumper J4 Pin 38)  ----> SCL

   RA6E1_FPB
   ----------
   Channel 9 has been used by SCI_I2C Master and Channel 1 has been used by IIC Slave.
   1) SCI_I2C Master pins
        SCI9 P602 (Jumper J4 Pin 15) ----> SDA
        SCI9 P601 (Jumper J4 Pin 16) ----> SCL
   2) Slave IIC pins
        IIC1 P206 (Jumper J3 Pin 31)  ----> SDA
        IIC1 P205 (Jumper J3 Pin 32)  ----> SCL

   RA4E1_FPB
   -----------
   Channel 4 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
   1) SCI_I2C Master pins
        SCI4 P206 (Jumper J3 Pin31)  ----> SCL
        SCI4 P205 (Jumper J3 Pin 32) ----> SDA
   2) Slave IIC pins
        IIC0 - P400 (Jumper J3 Pin1) ----> SCL 
        IIC0 - P401 (Jumper J3 Pin 2)----> SDA

   RA6T2_MCK
   -----------
   Channel 1 has been used by SCI_I2C Master and Channel 0 has been used by IIC Slave.
   1) SCI_I2C Master pins
        SCI1 PB08 (CN5:3)  ----> SCL
        SCI1 PB09 (CN5:1)  ----> SDA
   2) Slave IIC pins
        IIC0 PB06 (CN4:33) ----> SCL 
        IIC0 PB07 (CN4:31) ----> SDA
    

Note:
* For the functioning of SCI_I2C Master and IIC Slave on all the boards except for EK-RA6M3/EK-RA6M3G,FPB-RA6E1,FPB-RA4E1, external pull up
  resistors of value 3.9 or 4.7k ohms are required to be connected on I2C(SDA/SCL) lines.

1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x1ffe0c4c
   b. Keil: Not Available
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
