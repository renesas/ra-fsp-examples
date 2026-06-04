# Introduction #
This example project demonstrates the basic usage of SAU SPI driver in master configuration on Renesas RA MCUs. The project writes commands to the slave digital thermometer module, and reads the temperature values using SAU SPI bus configured as master. The temperature values are continuously printed on terminal. Status information and error messages will be printed on terminal during the execution of the project.

**Note:**
* Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer. If supported, it defaults to the serial terminal (UART).
* To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please refer to the instructions provided in the Special Topic.
* RA boards supported for J-Link OB VCOM: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1.
* In addition to communicating with the digital thermometer module, the example project can also communicate with the sau_spi_slave EP. For more information, please refer to the document of the sau_spi_slave EP.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the SAU SPI Master example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.5.0
* e2 studio: Version 2026-04.2
* SEGGER J-Link RTT Viewer: Version 9.42
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
* Terminal Console Application: Tera Term or a similar application

### Hardware ###
Supported RA Boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3
* 1 x Renesas RA board.
* 1 x Type-C USB cable.
* 1 x Digital thermometer module (PMOD MAX31723PMB1)
  * Link product: [PMOD MAX31723PMB1](https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31723PMB1?qs=UmMSjoC1xtH8e742i4OoUA%3D%3D).
  * Manufacturer Product Number: MAX31723PMB1#
* 1 x Host PC.

**Select SPI communication protocol on the PMOD MAX31723PMB1 by configuring the following jumper configurations on JP1:**

![PMOD MAX31723PMB1 configuration](images/sau_spi_pmod_configuration.png "PMOD MAX31723PMB1 configuration")

### Hardware Connections ###
* PMOD MAX31723PMB1:
	* Select SPI communication protocol on the PMOD by configuring the following jumper configurations on JP1:
  * Connect JP1-3 to JP1-5
  * Connect JP1-4 to JP1-6
  * Connect JP1-9 to JP1-10

* For FPB-RA0E1:
  * Connect the RA board to the host PC using a Type-C USB cable.
  * Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port. Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.
  * The soldering bridge E4 must be closed.
  * Populate R10 with a resistor ranging from 0 to 150 ohms.

* For FPB-RA0E2:
  * Connect the RA board to the host PC using a Type-C USB cable.
  * Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port. Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1.
  * The user must close E46, E47 to use J-Link OB VCOM for Serial Terminal (UART).

* For FPB-RA0L1:
  * Connect the RA board to the host PC using a Type-C USB cable.
  * Connect PMOD MAX31723PMB1 (J1) to the RA board via the PMOD1 Port. Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD1 (J19).
  * The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
  * The user must close E28, E29 to use J-Link OB VCOM for Serial Terminal (UART).

* For FPB-RA0E3:
  * Note: the user must turn OFF S3 to use the on-board debug functionality.
  * Connect PMOD MAX31723PMB1 (J1) to the RA board as below:
    * Temperature sensor (J1:1) to P112 (J2:4)
    * Temperature sensor (J1:2) to P213 (J1:4)
    * Temperature sensor (J1:3) to P212 (J1:5)
    * Temperature sensor (J1:4) to P201 (J1:8)
    * Temperature sensor (J1:5) to GND  (J1:3)
    * Temperature sensor (J1:6) to VCC  (J1:6)

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for basic information on setting up and using the above software.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
High level block diagram of the system is as shown below:

![high_level_design](images/SAU_SPI_Master_hld.png "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword |
|-------------|-----------------------------------------------|-----------------------------------------------|
| SAU SPI | SAU SPI is used to communicate with the digital thermometer module. | r_sau_spi |
| I/O Port | I/O Port is used to drive SS_N pin of the digital thermometer module. | r_ioport |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different from those selected by default. 

**Configuration Properties for using SAU SPI**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_sau_spi SPI (r_sau_spi) > Properties > Settings > Property > Common > Enable Single Channel | Disable | 11 | Enable single channel to reduce code size if only channel 11 is to be configured for SAU SPI. |
| configuration.xml > Stacks > g_sau_spi SPI (r_sau_spi) > Properties > Settings > Property > Common > DTC Support | Disable | Enable | Enable DTC Support. |
| configuration.xml > Stacks > g_sau_spi SPI (r_sau_spi) > Properties > Settings > Property > Module g_sau_spi SPI (r_sau_spi) > Channel | 00 | 11 | Use SAU SPI Channel 11 to communicate with the digital thermometer module. |
| configuration.xml > Stacks > g_sau_spi SPI (r_sau_spi) > Properties > Settings > Property > Module g_sau_spi SPI (r_sau_spi) > Callback | sau_spi_callback | sau_spi_callback | A user callback function that is called from the SAU SPI interrupts when a transfer is completed or an error has occurred. |
| configuration.xml > Stacks > g_sau_spi SPI (r_sau_spi) > Properties > Settings > Property > Module g_sau_spi SPI (r_sau_spi) > Transmit End Interrupt Priority | Priority 2 | Priority 2 | Select the transmit end interrupt priority. |

**Configuration Properties for using GPIO**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Pins > Pin Selection > Ports > P1 > P112 > Pin Configuration > Mode | Disable | Output mode (Initial Low) | Use P112 as output to drive SS_N pin of slave device during transfer process. |

## API Usage ##
The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_SAU_SPI_Open | This API is used to initialize the SAU SPI module. |
| R_SAU_SPI_Write | This API is used to configure the digital thermometer module. |
| R_SAU_SPI_WriteRead | This API is used to write command to the digital thermometer module and read temperature values. |
| R_SAU_SPI_Close | This API is used to de-initialize the SAU SPI module. |
| R_IOPORT_PinWrite | This API is used to drive Slave Select Pin during transmission. |
| R_BSP_SoftwareDelay | This API is used to delay a specified period of time. |

## Verifying Operation ##
1. Import, generate and build the example project.
2. Before running the example project, make sure hardware connections are completed.
3. Flash or debug the example project.
4. Open J-Link RTT Viewer to see the output.
  
The images below showcase the output on the J-Link RTT Viewer:

EP Information:

![ep_info](images/sau_spi_master_ep_info.png "EP INFO")

Operation log:

![operation_log](images/sau_spi_master_serial_log.png "Operation log")

## Special Topic ##
**Special notes:**
* FPB-RA0E3 limitations:
	* Due to the limited RAM size (2 KB), Serial Terminal is not supported on FPB-RA0E3.
	* Enabling printing floats to the RTT Viewer is not required and should be disabled to avoid RAM overflow.
	
* Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer; if supported, it defaults to the serial terminal (UART).
  * To configure display options:
    * **For UART via J-Link OB VCOM**: Define `USE_VIRTUAL_COM=1`
    * **For J-Link RTT Viewer**: Define `USE_VIRTUAL_COM=0`
  * It is important to note that the user should only operate a single terminal application (SEGGER J-Link RTT Viewer or a serial terminal (UART)) at a time to avoid conflicts or data inconsistencies. 
  * The board supports J-Link OB VCOM: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1.
    
  Set this in **Project Properties** -> **C/C++ Build** -> **Settings** -> **Tool Settings** -> **GNU ARM Cross C Compiler** -> **Preprocessor**.
