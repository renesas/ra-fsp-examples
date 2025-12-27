/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
    	The example project demonstrates typical use of the IIC slave HAL module APIs.
    	The project initializes IIC slave and IIC master module with standard rate and is made interfaced with loop-back
    	mechanism. It performs Slave read and write operation continuously once initialization is successful. On successful
    	I2C transaction (6 bytes), data transceived is compared. LED blinks on data match else it is turned ON as sign of
    	failure. Output message for both corresponding slave operations is displayed on RTT Viewer. Any API/event failure
    	message is also displayed.

2. Hardware Connections:
    	Two jumper wires are required to establish loop back connection along IIC lines within the board with pins as
    	mentioned below.
    	For EK-RA6M3/EK-RA6M3G:
    	--------
    		Channel 0 has been used by IIC slave and channel 2 been used by IIC master
    		1) Slave IIC pins
        		IIC0 P401  ----> SDA
        		IIC0 P408  ----> SCL
    		2) Master IIC pins
        		IIC2 P511  ----> SDA
        		IIC2 P512  ----> SCL

    	For EK-RA6M2:
    	--------
    		Channel 0 has been used by IIC slave and channel 1 been used by IIC master.
    		1) Slave IIC pins
        		IIC0 P401  ----> SDA
        		IIC0 P400  ----> SCL
    		2) Master IIC pins
        		IIC1 P206  ----> SDA
        		IIC1 P100  ----> SCL

    	For EK-RA6M1:
    	--------
    		Channel 0 has been used by IIC slave and channel 1 been used by IIC master.
    		1) Slave IIC pins
        		IIC0 P401  ----> SDA
        		IIC0 P400  ----> SCL
    		2) Master IIC pins
        		IIC1 P101  ----> SDA
        		IIC1 P100  ----> SCL

    	For EK-RA4M1:
    	--------
    		Channel 0 has been used by IIC slave and channel 1 been used by IIC master.
    		1) Slave IIC pins
        		IIC0 P401  ----> SDA
        		IIC0 P400  ----> SCL
    		2) Master IIC pins
        		IIC1 P206  ----> SDA
        		IIC1 P100  ----> SCL

    	For EK-RA2A1:
    	--------
    		1) Slave IIC pins
        		IIC0 P401  ----> SDA
        		IIC0 P000  ----> SCL
    		2) Master IIC pins
        		IIC1 P400  ----> SDA
        		IIC1 P109  ----> SCL

    	For EK-RA4W1:
    	--------
    		1) Slave IIC pins
        		IIC0 P407  ----> SDA
        		IIC0 P204  ----> SCL
    		2) Master IIC pins
        		IIC1 P206  ----> SDA
        		IIC1 P205  ----> SCL

    	For EK-RA6M4:
    	--------
    		1) Master IIC pins
        		IIC0 P401  ----> SDA
        		IC0 P408  ----> SCL
    		2) Slave IIC pins
        		IIC1 P206  ----> SDA
        		IIC1 P205  ----> SCL

    	For RSSK-RA6T1:
    	--------
    		1) Master IIC pins
        		IIC0 P401  ----> SDA
        		IIC0 P400  ----> SCL
    		2) Slave IIC pins
        		IIC1 P101  ----> SDA
        		IIC1 P100  ----> SCL

    	For EK-RA2L1:
    	--------
    		1) Master IIC pins
        		IIC1 P206  ----> SDA
        		IIC1 P205  ----> SCL
    		2) Slave IIC pins
        		IIC0 P407  ----> SDA
        		IIC0 P408  ----> SCL

    	For EK-RA4M3:
    	--------
    		1) Master IIC pins
        		IIC1 P511  ----> SDA
        		IIC1 P512  ----> SCL
    		2) Slave IIC pins
        		IIC0 P401  ----> SDA
        		IIC0 P408  ----> SCL

    	For EK-RA4M2:
    	--------
    		1) Master IIC pins
        		IIC1 P206  ----> SDA
        		IIC1 P205  ----> SCL
    		2) Slave IIC pins
        		IIC0 P401  ----> SDA
        		IIC0 P400  ----> SCL

    	For EK-RA6M5:
    	--------
    		1) Master IIC pins
        		IIC1 P511 (Jumper J4 Pin 39) ----> SDA
        		IIC1 P512 (Jumper J4 Pin 38) ----> SCL
    		2) Slave IIC pins
        		IIC2 P414 (J1 Pin 32) ----> SDA
        		IIC2 P415 (J1 Pin 31) ----> SCL

    	For FPB-RA6E1:
    	--------
    		1) Master IIC pins
        		IIC1 P206 (J3 Pin 31) ----> SDA
        		IIC1 P205 (J3 Pin 32) ----> SCL
    		2) Slave IIC pins
        		IIC0 P401 (J3 Pin 2) ----> SDA
        		IIC0 P400 (J3 Pin 1) ----> SCL

    	For MCK-RA6T2:
    	---------
    		1) Master IIC pins
        		IIC1 PB09(CN5:1)  ----> SDA
        		IIC1 PB08(CN5:3)  ----> SCL
    		2) Slave IIC pins
        		IIC0 PB07(CN4:31) ----> SDA
        		IIC0 PB06(CN4:33) ----> SCL

    	For EK-RA8M1/EK-RA8D1:
    	--------
    		Channel 0 has been used by IIC slave and channel 1 been used by IIC master
    		1) Slave IIC pins
        		IIC0 P409  ----> SDA
        		IIC0 P410  ----> SCL
    		2) Master IIC pins
        		IIC1 P511  ----> SDA
        		IIC1 P512  ----> SCL

    	For EK-RA2A2:
    	--------
    		Channel 0 has been used by IIC slave and channel 1 been used by IIC master
    		1) Slave IIC pins
        		IIC0 P410  ----> SDA
        		IIC0 P411  ----> SCL
    		2) Master IIC pins
        		IIC1 P013  ----> SDA
        		IIC1 P012  ----> SCL

    	For FPB-RA8E1:
    	--------
    		Channel 0 has been used by IIC slave and channel 1 been used by IIC master
    		1) Slave IIC pins
        		IIC0 P409 (J2:11)  ----> SDA
        		IIC0 P410 (J2:12)  ----> SCL
    		2) Master IIC pins
        		IIC1 P511 (J4:4)  ----> SDA
        		IIC1 P512 (J4:3)  ----> SCL

Note:
1) DTC transfer support is configurable and is enabled in the configurator. DTC is helpful for minimizing interrupts
   during large transactions.

2) For the functioning of IIC Slave on all the boards except for EK-RA6M3, EK-RA6M3G, FPB-RA6E1, EK-RA2L1, EK-RA8D1, EK-RA8M1,
   EK-RA2A2, FPB-RA8E1 external pull up resistors of value 3.9 or 4.7k ohms are required to be connected on I2C(SDA/SCL) lines.

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220604ac
   b. Keil:	0x220608b0
   c. IAR: 0x220609b8
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
