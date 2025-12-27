# Introduction #

The example project demonstrates the typical use of the SCI I2C master HAL module APIs. The project initializes SCI I2C master module with the standard rate and interfaces with PmodACL™ Board for ADXL345.

On power up, after initializing and configuring the sensor, the RA MCU will read the axis acceleration data from the sensor and display it on the RTT Viewer every time new data is detected by an external interrupt 

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the SCI I2C example project, the following resources are needed.

### Hardware ###
* 1 x Renesas RA board
* 1 x PmodACL™ Board
* 1 x Micro USB cable  (For MCK-RA4T1, MCK-RA6T3, MCK-RA8T1: Type C USB cable)
* 2 x Resistor (from 1k to 4.7k ohms)
* Some jumper cables

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.3.0
* e2 studio: Version 2025-12
* SEGGER J-Link RTT Viewer: Version 8.92
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
High level block diagram of the system is as shown below:
 
![sci_i2c](images/sci_i2c_hld.png "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| SCI I2C Master | SCI I2C Master is used to communicate with the sensor | r_sci_i2c |
| External IRQ | External IRQ is used to detect sensor data is ready | r_icu |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

The SCI channel and ICU channel depend on the board type, refer to [readme.txt](./readme.txt) for information on how to select the correct channel.

**Configuration Properties for using SCI I2C Master**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > g_sci_i2c_master I2C Master (r_sci_i2c) > Settings > Property > Common > DTC on Transmission and Reception | Disabled | Enabled | Enable DTC for transmission and reception. |
| configuration.xml > g_sci_i2c_master I2C Master (r_sci_i2c) > Settings > Property > Module g_sci_i2c_master I2C Master (r_sci_i2c) > Channel | 0 | 0 | Use SCI I2C Master Channel 0 to communicate with the sensor. |
| configuration.xml > g_sci_i2c_master I2C Master (r_sci_i2c) > Settings > Property > Module g_sci_i2c_master I2C Master (r_sci_i2c) > Rate | Standard | Standard | Select communication speed with the sensor. |
| configuration.xml > g_sci_i2c_master I2C Master (r_sci_i2c) > Settings > Property > Module g_sci_i2c_master I2C Master (r_sci_i2c) > Slave Address | 0x00 | 0x3C | Specific slave address of the PmodACL ADXL345. |
| configuration.xml > g_sci_i2c_master I2C Master (r_sci_i2c) > Settings > Property > Module g_sci_i2c_master I2C Master (r_sci_i2c) > Address Mode | 7-Bit | 7-Bit | Specific address mode of the PmodACL ADXL345. |
| configuration.xml > g_sci_i2c_master I2C Master (r_sci_i2c) > Settings > Property > Module g_sci_i2c_master I2C Master (r_sci_i2c) > Callback | sci_i2c_master_callback | sci_i2c_master_callback | It is called from the interrupt service routine (ISR) upon IIC transaction completion reporting the transaction status. |
| configuration.xml > g_sci_i2c_master I2C Master (r_sci_i2c) > Settings > Property > Module g_sci_i2c_master I2C Master (r_sci_i2c) > Interrupt Priority Level | Priority 12 | Priority 12 | Select the SCI I2C master interrupt priority level. |
| configuration.xml > g_sci_i2c_master I2C Master (r_sci_i2c) > Settings > Property > Module g_sci_i2c_master I2C Master (r_sci_i2c) > RX Interrupt Priority Level [Only used when DTC is enabled] | Disabled | Priority 12 | Select the SCI I2C master RX interrupt priority level. |


**Configuration Properties for using External IRQ**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > g_external_irq External IRQ (r_icu) > Settings > Property > Module g_external_irq External IRQ (r_icu) > Channel | 0 | 2 | Use External IRQ channel 2 to detect sensor data is ready. |
| configuration.xml > g_external_irq External IRQ (r_icu) > Settings > Property > Module g_external_irq External IRQ (r_icu) > Trigger | Rising | Rising | Detect data ready by rise edge. |
| configuration.xml > g_external_irq External IRQ (r_icu) > Settings > Property > Module g_external_irq External IRQ (r_icu) > Digital Filtering | Disabled | Enabled | Noise filter. |
| configuration.xml > g_external_irq External IRQ (r_icu) > Settings > Property > Module g_external_irq External IRQ (r_icu) > Callback | NULL | external_irq_callback | It is called from the interrupt service routine (ISR) upon a rising edge is detected on the IRQ pin. |
| configuration.xml > g_external_irq External IRQ (r_icu) > Settings > Property > Module g_external_irq External IRQ (r_icu) > Pin Interrupt Priority | Priority 12 | Priority 12 | Select the External IRQ interrupt priority. |

## API Usage ##
The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
  | R_ICU_ExternalIrqOpen | This API is used to initialize an IRQ input pin for use with the external interrupt interface. |
| R_ICU_ExternalIrqEnable | This API is used to enable external interrupt for specified channel. |
| R_ICU_ExternalIrqClose | This API is used to de-initialize the external interrupt channel. |
| R_SCI_I2C_Open | This API is used to initialize the SCI I2C Master module. |
| R_SCI_I2C_Write | This API is used to perform a write operation to the sensor register. |
| R_SCI_I2C_Read | This API is used to perform a read operation from the sensor register. |
| R_SCI_I2C_Close | This API is used to de-initialize the SCI I2C Master module. |
| R_BSP_SoftwareDelay | This API is used to delay a specified period of time. |

## Verifying operation ##
1. Import, generate and build the EP project.
2. Connect Pmod ACL module to RA board.
3. Connect RA board to Host PC.
4. Flash or debug the EP project.
5. Open the RTT Viewer and view output log.
  
  ![rtt_log](images/sci_i2c_rtt_log.png "J-Link RTT Viewer")

## Special Topics ##
* The files sci_i2c_ep.c and sci_i2c_ep.h contain the API for initialization, configuration and communication of the PmodACL module.
* Some MCUs do not support r_sci_i2c module but can use r_sci_b_i2c module instead.
* For the functioning of the SCI I2C Master:
1. SDA pin and SCL pin must configure output type as n-ch open drain.
2. SDA pin and SCL pin must be connected to external pull-up resistors.
3. The value of the external pull-up resistor depends on the I2C bus speed. In this EP project, the I2C bus is configured at standard speed, so the recommended external pull-up resistor value is from 1k to 4.7k ohms.
For MCK-RA8T1: Value of pull-up resistor is from 1k to 3.9k ohms.
