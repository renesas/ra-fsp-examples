**Note :** MD file is common for both i3c_master and i3c_slave EP.

# Introduction #
 
The sample code accompanying this file shows the operation of the I3C Driver on Renesas RA MCUs based on Renesas FSP.  
The I3C master on RA board demonstrates operations associated with an I3C slave running on another RA board. Once initialization is successful,
I3C Master device assigns slave address to I3C slave device through DAA (dynamic address assignment) using I3C common command code (CCC).  
Once Dynamic Address Assignment is completed, the Master EP performs write/read operations, and displays slave device information based on user input. 
If the on-board switch is pressed on the slave side, it will initiate an IBI transfer request.
The application status messages will be displayed in the J-Link RTT Viewer and on-board LEDs. Refer **[LED Status](#led-status)** section for LED observation.

Note: EK-RA8D1 supports I3C HDR-DDR mode. The Master EP can performs write/read operations both in the SDR mode and HDR-DDR mode.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the I3C example project, the following resources are needed.

### Hardware ###
* Renesas RA boards - 2x
* USB A to USB Micro B Cable or Type C USB Cable (For MCK-RA4T1, MCK-RA6T3) - 2x
* Jumper wires - 5x
* Pull up resistor of value 1k, 3.9k or 4.7k ohms - 1x 

### Hardware Connections  ###
Supported Boards: EK-RA2E2/EK-RA4E2/EK-RA6E2/MCK-RA4T1/MCK-RA6T3/EK-RA8M1/EK-RA8D1:

* For EK-RA2E2/EK-RA4E2/EK-RA6E2/EK-RA8M1/EK-RA8D1:		
   * Connect micro-USB cable between J10 on I3C master RA board and PC.
   * Connect micro-USB cable between J10 on I3C slave RA board and PC.
* For MCK-RA4T1/MCK-RA6T3:
   * Connect Type C USB cable between CN9 on I3C master RA board and PC.
   * Connect Type C USB cable between CN9 on I3C slave RA board and PC.
   
* External 3V3 pull-up using 1k, 3.9k or 4.7k ohms resistor should be connected on SDA line.
   * Change external 3V3 pull-up using 1K ohms resistor for EK-RA8M1, EK-RA8D1.
* The jumper wires are required to establish loop back connection along I3C lines across the board with pins as mentioned below.

	EK-RA2E2:

		Master Board              Slave Board
		-------------             ------------
		SDA0 Pin P401   ---->     SDA0 Pin 401
		SCL0 Pin P400   ---->     SCL0 Pin 400
		Common GND

	EK-RA4E2/EK-RA6E2:

		Master Board              Slave Board
		-------------             ------------
		SDA0 Pin P101   ---->     SDA0 Pin 101
		SCL0 Pin P100   ---->     SCL0 Pin 100
		Common GND

	MCK-RA4T1/MCK-RA6T3:

		Master Board              Slave Board
		-------------             ------------
		SDA0 Pin P101   ---->     SDA0 Pin 101 (TP-16)
		SCL0 Pin P100   ---->     SCL0 Pin 100 (TP-17)
		Common GND

	EK-RA8M1:

		Master Board              Slave Board
		-------------             ------------
		SDA0 J27:2   	---->     SDA0 J27:2
		SCL0 J27:1   	---->	  SCL0 J27:1
		Common GND
		
	EK-RA8D1:

		Master Board              Slave Board
		-------------             ------------
		SDA0 P401   	---->     SDA0 P401
		SCL0 P400   	---->	  SCL0 P400
		Common GND

### Hardware Configurations ###

* For EK-RA8M1: Install jumper J61 to enable I3C

* For EK-RA8D1: Set the configuration switches (SW1) as below.

| SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
|-------------|-------------|--------------|------------|------------|------------|-------------|-----------|
| OFF | OFF | OFF | OFF | OFF | OFF | OFF | OFF |

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.2.0
* e2 studio: Version 2025-10
* SEGGER J-Link RTT Viewer: Version 8.74
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)


## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
 High level block diagram
  
![I3C](images/highLevel_i3c.jpg "High Level Block Diagram")


## FSP Modules Used ##
List of important modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
|I3C | Driver for the I3C peripheral to demonstrate Master and Slave mode on RA2E2/RA4E2/RA6E2/RA4T1/RA6T3/RA8M1/RA8D1 MCUs | i3c|
|AGT | Driver for the AGT peripheral is used for demonstrating timeout condition. | agt|
|ICU | The external_irq interface is for configuring interrupts to fire when a trigger condition is detected on an external IRQ pin | icu|

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

**I3C Master Configuration properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Common > Slave Support |Enable | Disable | As only master mode is required, disable slave support to decrease code size. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Bitrate Setting > Standard Mode > Push-Pull > Logic High Period (ns) | 167 | 167 | Set 167ns as the Logic High Period of SCL during Standard Mode Push-Pull transfers. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Bitrate Setting > Standard Mode > Push-Pull > Frequency | 3400000 | 3400000 | Set 3.4MHz as the frequency of SCL during Standard Mode Push-Pull transfers. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Master Mode > ACK Hot-Join Requests |Disable | Enable | Enable Hot-Join Requests acknowledgment. If a Hot-Join event is received, then the master will initiate the dynamic address assignment. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Master Mode > Notify Rejected Hot-Join Requests |Disable | Enable | Generate a callback when an IBI Hot-Join Request is rejected. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Master Mode > Notify Rejected Mastership Requests |Disable | Enable | Generate a callback when an IBI Mastership Request is rejected. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Master Mode > Notify Rejected Interrupt Requests |Disable | Enable | Generate a callback when an IBI Interrupt Request is rejected. |  
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Device Type | Slave | Main Master | Set the role of the I3C instance on the I3C bus. |

**For EK-RA8D1:** 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Bitrate Setting > Standard Mode > Push-Pull > Logic High Period (ns) | 167 | 45 | Set 45ns as the Logic High Period of SCL during Standard Mode Push-Pull transfers. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Bitrate Setting > Standard Mode > Push-Pull > Frequency | 3400000 | 12500000 | Set 12.5MHz as the frequency of SCL during Standard Mode Push-Pull transfers. |

**I3C Slave Configuration properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Common > Master Support |Enable | Disable | If only slave mode is required, disable master support to decrease code size. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Slave Mode > Command Response Info > ENEC/DISEC > In-Band Interrupts |Disable | Enable | Enable the In-Band Interrupts capability, so that the slave should issue IBI requests. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Slave Mode > Command Response Info > ENEC/DISEC > Hot-Join Requests |Disable | Enable | Enable the Hot-Join Requests capability, so that the slave should issue Hot-Join requests. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Slave Mode > Command Response Info > SETMWL/GETMWL > Max Write Length | 65535 | 64 | Set the Device Max Write Length. In this EP, the write buffer is 64-byte. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Slave Mode > Command Response Info > SETMRL/GETMRL > Max Read Length | 65535 | 64 | Set the Device Max Read Length. In this EP, the read buffer is 64-byte. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Slave Mode > Command Response Info > SETMRL/GETMRL > Max IBI Payload Length | 0 | 32 | Set the Device Max IBI Payload Length. In this EP, the IBI read buffer is 32-byte. |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Device Type | Slave | Slave | Set the role of the I3C instance on the I3C bus. |

**For EK-RA8D1:** 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml -> HAL/Common -> Properties > Settings > Property > Module I3C > Slave Mode > Command Response Info > GETCAP/GETHDRCAP > HDR-DDR (Mode 0) | Disable | Enable | Enable HDR-DDR mode. |

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_I3C_Open|This API is used to Open the I3C instance. |
|R_I3C_DeviceCfgSet|This API is used to set the configuration for this device  |
|R_I3C_MasterDeviceTableSet|This API configures an entry in the master device table. |
|R_I3C_Enable|This API is used to Enable the I3C device.  |
|R_I3C_IbiRead|This API is used to set the read buffer for storing received IBI data. Used in only master mode.  |
|R_I3C_DynamicAddressAssignmentStart|This API is used to start the Dynamic Address Assignment Process. |
|R_I3C_Write|This API is used to set the write buffer for the transfer. In master mode, start the transfer. |
|R_I3C_Read|This API is used set the read buffer for the transfer. In master mode, start the transfer. |
|R_I3C_SlaveStatusSet|This API is used to set the status returned to the master in response to a GETSTATUS command. |
|R_I3C_IbIWrite|This API is used to initiate an IBI write operation. Used in only Slave mode. |
|R_I3C_Close|This API is used to close the I3C instance |
|R_AGT_Open|This API is used to initialize the AGT module instance. |
|R_AGT_Stop|This API is used to stop the timer. |
|R_AGT_Reset|This API is used to reset the counter value. |
|R_AGT_Start|This API is used to Start timer. |
|R_AGT_Close|This API is used to close the AGT module instance. |
|R_ICU_ExternalIrqOpen|This API is used to prepare an external IRQ pin for operation. |
|R_ICU_ExternalIrqEnable|This API is used to enable external interrupt for specified channel at NVIC. |
|R_ICU_ExternalIrqClose|This API is used to close the external interrupt channel. |
|R_IOPORT_PinRead |This API is used to read the current status of GPIO Pin. |
|R_IOPORT_PinWrite |This API is used to write the value on GPIO Pin. |


## Verifying operation ##
1. Import, generate and build both i3c_master and i3c_slave EPs in different workspaces.
   Before running the example project, make sure hardware connections are done on both master and slave side.
2. Download i3c_master EP to one RA MCU and run the project.
3. Download i3c_slave EP to another RA MCU and run the project.
4. Now open one J-Link RTT Viewer and connect to the master board and open another/second J-Link RTT Viewer and connect it to slave board.
5. User can perform Menu option operations on the master side and check corresponding results for the Master and Slave EPs on the respective J-Link RTT Viewer.
6. Also on slave side user can perform IBI transfer request using Push button(S2) and verify result on J-Link RTT Viewer.  
   For MCK-RA4T1/MCK-RA6T3: The user needs to use Toggle switch(S1) instead.
   
   Below images showcases the I3C Master output on J-Link RTT Viewer:

   ![I3C Master banner snapshot](images/i3c_master_banner_info.jpg "Banner Info")

   ![I3C Master menu snapshot](images/i3c_master_menu.jpg "Menu")

   ![I3C Master init DDA snapshot](images/i3c_master_init_DAA.jpg "Initialize DDA")
 
   ![I3C Display slave info snapshot](images/i3c_master_display_slave_info.jpg "display slave info")
 
   ![I3C Master write/read in SDR mode snapshot](images/i3c_master_write_read_sdr_mode.jpg "Write/Read in SDR mode")

   ![I3C Master write/read in HDR-DDR mode snapshot](images/i3c_master_write_read_hdr_ddr_mode.jpg "Write/Read in HDR-DDR mode")

   ![I3C Master IBI receive](images/i3c_master_ibi_receive.jpg "IBI receive")

   Below images showcases the I3C Slave output on J-Link RTT Viewer:

   ![I3C Slave banner snapshot](images/i3c_slave_banner_info.jpg "Banner Info")

   ![I3C Slave dynamic address snapshot](images/i3c_slave_dynamic_address.jpg "Dynamic address")

   ![I3C Slave write/read in SDR mode snapshot](images/i3c_slave_write_read_sdr_mode.jpg "Write/Read in SDR mode")

   ![I3C Slave write/read in HDR-DDR mode snapshot](images/i3c_slave_write_read_hdr_ddr_mode.jpg "Write/Read in HDR-DDR mode")

   ![I3C Slave IBI transfer request snapshot](images/i3c_slave_ibi_transfer_request.jpg "IBI transfer request")

## LED Status ##
Below are the observations of the LED operation on slave board :
* When Dynamic Address Assignment completes, the onboard Blue LED status will be toggled.
* When write transfer completes, the onboard Green LED status will be toggled.
* When read trannsfer completes, the onboard Red LED status will be toggled.
* When IBI write transfer completes, the onboard Green LED status will be toggled.

For MCK-RA4T1 and MCK-RA6T3, below are the observations of the LED operation on slave board :  
* When Dynamic Address Assignment completes, both onboard LED1 and LED2 status will be toggled.
* When write transfer completes, the onboard LED1 status will be toggled.
* When read transfer completes, the onboard LED2 status will be toggled.
* When IBI write transfer completes, the onboard LED1 status will be toggled.