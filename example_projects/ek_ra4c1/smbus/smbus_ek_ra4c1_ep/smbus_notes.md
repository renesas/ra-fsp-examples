# Introduction #
This example project demonstrates the basic usage of I2C communication conforming to the SMBus Specification (version 2.0) on Renesas RA MCUs using the Renesas Flexible Software Package (FSP). The project involves writing commands to the MAX31875 sensor and reading the resulting temperature values. The SMBus transmission time is monitored by the GPT module to ensure the MCU acting as a master device complies with the SMBus timeout standard. The Packet Error Check (PEC) byte is calculated in CRC-8 format at both transmission and reception. These temperature values are continuously printed to a terminal. Additionally, status information and error messages are displayed on the host PC. The error LED will turn on if an error occurs.

**Note:**
- To display information, users can choose between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART) with J-Link OB VCOM. It is important to note that the user should only operate a single terminal application at a time to avoid conflicts or data inconsistencies. 
- For instructions on how to switch between these options, please refer to the "Verifying Operation" section in this file.
- By default, EP information is printed to the host PC using the Serial Terminal for boards that support J-Link OB VCOM.
- RA boards support J-Link OB VCOM: EK-RA8M1, EK-RA2A2, EK-RA4E2, EK-RA6E2, EK-RA8D1, FPB-RA2E3, MCK-RA4T1, MCK-RA6T3, MCK-RA8T1, FPB-RA8E1, EK-RA8P1, FPB-RA2T1, EK-RA4C1.

**For the RA boards that do not support J-Link OB VCOM, EP uses the SEGGER J-Link RTT Viewer by default instead.**

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the SMBus example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.3.0
* e2 studio: Version 2025-12
* SEGGER J-Link RTT Viewer: Version 8.92
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
* Serial Terminal Console Application: Tera Term or a similar application (for boards that support J-Link OB VCOM)

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware Requirements ###
Supported RA boards: EK-RA8M1, CK-RA6M5, EK-RA2A1, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, FPB-RA2E3, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, RSSK-RA6T1, FPB-RA8E1, EK-RA8P1, FPB-RA2T1, EK-RA4C1.
* 1 x Renesas RA board.
* 1 x USB cable for programming and debugging.
    - 1 x Type-C USB cable: CK-RA6M5, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1 FPB-RA8E1, EK-RA8P1, FPB-RA2T1, EK-RA4C1
    - 1 X Mini USB cable: RSSK-RA6T1
    - 1 x Micro USB cable: All other boards
* 1 x Evaluation Kit for the MAX31875
    * Link Product: https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31875EVKIT?qs=BZBei1rCqCDTW6E3jJcKjw%3D%3D&_gl=1
    * Manufacturer Product Number: MAX31875EVKIT#
* 1 x Host PC.

**RA boards that do not require external resistors for pull-up:**
* For EK-RA8M1, CK-RA6M5, EK-RA2E1, EK-RA2L1, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, FPB-RA4E1, FPB-RA6E1, FPB-RA8E1, EK-RA8P1, FPB-RA2T1, EK-RA4C1:
  * 4 x Jumper wires (4 - both ends female).

* For EK-RA2A2, FPB-RA2E3, MCK-RA4T1, MCK-RA6T3, MCK-RA8T1:
  * No additional jumper wires required.

**RA boards that require external resistors for pull-up:** EK-RA2A1, EK-RA2E2, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4W1, EK-RA6E2, EK-RA6M1, EK-RA6M2, MCK-RA6T2, RSSK-RA6T1.
* 2 x Resistors (3.3K ohm or 4.7K ohm).
* 1 x Breadboard.
* 7 x Jumper wires.
    * 6 - male to female, 1 - both ends female.
    * 4 - male to female, 3 - both ends male (For RSSK-RA6T1).

### Hardware Connections ###
* EK-RA8M1:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P512 (J56:37) | SCL (J1:3) |
        | SDA1 P511 (J57:38) | SDA (J1:4) |
        | GND (J57:49) | GND (J1:5) |
        | VCC (J56:40) | VDD (J1:6) |

* CK-RA6M5:
    * Connect RA board to Host PC by Type-C USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P512 (J3:10) | SCL (J1:3) |
        | SDA1 P511 (J3:9) | SDA (J1:4) |
        | GND (J6:6) | GND (J1:5) |
        | VCC (J6:4) | VDD (J1:6) |

* EK-RA2A1:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P109 (J1:37) | SCL (J1:3) |
        | SDA1 P110 (J1:35) | SDA (J1:4) |
        | GND (J1:17) | GND (J1:5) |
        | VCC (J1:15) | VDD (J1:6) |

* EK-RA2A2, FPB-RA2E3:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect MAX31875PMB1 (JU4:1) to RA board via the PMOD2 (PMOD2:1)

* EK-RA2E1:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P408 (J2:8) | SCL (J1:3) |
        | SDA0 P407 (J2:9) | SDA (J1:4) |
        | GND (J2:40) | GND (J1:5) |
        | VCC (J2:1) | VDD (J1:6) |

* EK-RA2E2:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P400 (J1:2) | SCL (J1:3) |
        | SDA0 P401 (J1:3) | SDA (J1:4) |
        | GND (J1:13) | GND (J1:5) |
        | VCC (J1:1) | VDD (J1:6) |

* EK-RA2L1:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P408 (J1:10) | SCL (J1:3) |
        | SDA0 P407 (J1:11) | SDA (J1:4) |
        | GND (J1:40) | GND (J1:5) |
        | VCC (J1:1) | VDD (J1:6) |

* EK-RA4E2, EK-RA6E2:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P205 (J2:16) | SCL (J1:3) |
        | SDA0 P206 (J2:15) | SDA (J1:4) |
        | GND (J2:40) | GND (J1:5) |
        | VCC (J2:1) | VDD (J1:6) |

* EK-RA4M1:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P400 (J2:37) | SCL (J1:3) |
        | SDA0 P401 (J2:8) | SDA (J1:4) |
        | GND (J2:12) | GND (J1:5) |
        | VCC (J2:10) | VDD (J1:6) |

* EK-RA4M2:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P400 (J1:4) | SCL (J1:3) |
        | SDA0 P401 (J1:3) | SDA (J1:4) |
        | GND (J1:27) | GND (J1:5) |
        | VCC (J1:1) | VDD (J1:6) |

* EK-RA4M3, EK-RA6M4:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P512 (J2:37) | SCL (J1:3) |
        | SDA1 P511 (J2:38) | SDA (J1:4) |
        | GND (J2:39) | GND (J1:5) |
        | VCC (J2:40) | VDD (J1:6) |

* EK-RA4W1:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P205 (CN7:8) | SCL (J1:3) |
        | SDA1 P206 (CN7:7) | SDA (J1:4) |
        | GND (CN7:19) | GND (J1:5) |
        | VCC (CN7:18) | VDD (J1:6) |

* EK-RA6M1:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P400 (J1:1) | SCL (J1:3) |
        | SDA0 P401 (J1:3) | SDA (J1:4) |
        | GND (J1:17) | GND (J1:5) |
        | VCC (J1:15) | VDD (J1:6) |

* EK-RA6M2:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P400 (J4:13) | SCL (J1:3) |
        | SDA0 P401 (J4:11) | SDA (J1:4) |
        | GND (J4:36) | GND (J1:5) |
        | VCC (J4:37) | VDD (J1:6) |

* EK-RA6M3, EK-RA6M3G:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL2 P512 (J1:5) | SCL (J1:3) |
        | SDA2 P511 (J1:21) | SDA (J1:4) |
        | GND (J1:30) | GND (J1:5) |
        | VCC (J1:36) | VDD (J1:6) |

* EK-RA6M5:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P512 (J4:38) | SCL (J1:3) |
        | SDA1 P511 (J4:39) | SDA (J1:4) |
        | GND (J4:40) | GND (J1:5) |
        | VCC (J4:1) | VDD (J1:6) |

* EK-RA8D1:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P512 (J57:4) | SCL (J1:3) |
        | SDA1 P511 (J57:2) | SDA (J1:4) |
        | GND (J57:39) | GND (J1:5) |
        | VCC (J57:5) | VDD (J1:6) |

* FPB-RA4E1, FPB-RA6E1:
    * Connect RA board to Host PC by Micro USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P400 (J3:1) | SCL (J1:3) |
        | SDA0 P401 (J3:2) | SDA (J1:4) |
        | GND (J3:12) | GND (J1:5) |
        | VCC (J3:15) | VDD (J1:6) |

* FPB-RA8E1:
    * Connect RA board to Host PC by Type-C USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P512 (J24:10) | SCL (J1:3) |
        | SDA1 P511 (J24:9) | SDA (J1:4) |
        | GND (J18:6) | GND (J1:5) |
        | VCC (J18:4) | VDD (J1:6) |     

* MCK-RA4T1, MCK-RA6T3:
    * Connect RA board to Host PC by Type-C USB cable.
    * Connect MAX31875PMB1 (JU4:1) to RA board via the PMOD1 (PMOD1:1)
    * Jumper J10 short pin2-3 to select SCL Pin
    * Jumper J11 short pin1-2 to select SDA Pin

* MCK-RA6T2:
    * Connect RA board to Host PC by Type-C USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 PB06 (CN4:33) | SCL (J1:3) |
        | SDA0 PB07 (CN4:31) | SDA (J1:4) |
        | GND (CN4:25) | GND (J1:5) |
        | VCC (CN4:23) | VDD (J1:6) |

* MCK-RA8T1:
    * Connect RA board to Host PC by Type-C USB cable.
    * Connect MAX31875PMB1 (JU4:1) to RA board via the PMOD6A (PMOD6A:1)

* RSSK-RA6T1:
    * Connect RA board to Host PC by Mini USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P101 (CN8:2) | SCL (J1:3) |
        | SDA1 P100 (CN8:3) | SDA (J1:4) |
        | GND (CN8:5) | GND (J1:5) |
        | VCC (CN8:6) | VDD (J1:6) |

* EK-RA8P1:
    * Connect RA board to Host PC by Type-C USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P512 (J1:4) | SCL (J1:3) |
        | SDA1 P511 (J1:2) | SDA (J1:4) |
        | GND (J1:40) | GND (J1:5) |
        | VCC (J1:5) | VDD (J1:6) |

* FPB-RA2T1:
	* The user must Close E7 to use P914 for LED2.
    * Connect RA board to Host PC by Type-C USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL0 P400 (J3:1) | SCL (J1:3) |
        | SDA0 P401 (J3:2) | SDA (J1:4) |
        | GND (J2:6) | GND (J1:5) |
        | VCC (J2:4) | VDD (J1:6) |

* EK-RA4C1:  
    * The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use the on-board debug functionality.  
    * Connect RA board to Host PC by Type-C USB cable.
    * Connect RA board to MAX31875PMB1 as below:

        | RA board | MAX31875PMB1 |
        |----------|---------------|
        | SCL1 P400 (J1:3) | SCL (J1:3) |
        | SDA1 P401 (J1:4) | SDA (J1:4) |
        | GND (J1:25) | GND (J1:5) |
        | VCC (J1:1) | VDD (J1:6) |

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
High level block diagram of the system is shown below:
 
![SMBus](images/smbus_high_level_design.png "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| SMBus Communication Device | SMBus is used to communicate with the Evaluation Kit for the MAX31875 | rm_comms_smbus |

**Configuration Properties for SMBus Communication Device**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_comms_smbus0 SMBus Communication Device (rm_comms_smbus) > Properties > Settings > Property > Module g_comms_smbus0 SMBus Communication Device (rm_comms_smbus) > Callback | comms_smbus_callback | comms_smbus_callback | Provide a user callback function. |
| configuration.xml > Stacks > g_comms_smbus0 SMBus Communication Device (rm_comms_smbus) > Properties > Settings > Property > Module g_comms_smbus0 SMBus Communication Device (rm_comms_smbus) > Slave Address | 0x00 | 0x48 | Specify the slave address. |
| configuration.xml > Stacks > g_comms_smbus0 SMBus Communication Device (rm_comms_smbus) > Properties > Settings > Property > Module g_comms_smbus0 SMBus Communication Device (rm_comms_smbus) > CRC support | Enable | Enable | Enable PEC support and use CRC-8 algorithm to generate PEC byte for SMBus communication. |
| configuration.xml > Stacks > g_i2c_master0 I2C Master (r_iic_master) > Properties > Settings > Property > Common > DTC on Transmission and Reception | Disabled | Disabled | DTC support must be disabled. |
| configuration.xml > Stacks > g_comms_smbus_rsc_ctrl0 SMBus Resources control (rm_comms_smbus) > Properties > Settings > Property > Module g_comms_smbus_rsc_ctrl0 SMBus Resources control (rm_comms_smbus) > IIC channel | 0 | 1 | Select IIC channel 1. |
| configuration.xml > Stacks > g_comms_smbus_rsc_ctrl0 SMBus Resources control (rm_comms_smbus) > Properties > Settings > Property > Module g_comms_smbus_rsc_ctrl0 SMBus Resources control (rm_comms_smbus) > IIC variant | IIC | IIC | Select IIC as IIC variant be used under I2C Shared Bus stack. |

**Configuration Properties for SMBus timer (GPT)**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > General > Compare Match > Compare Match A > Status | Enabled | Enabled | Enable Compare Match A. |
| configuration.xml > Stacks > g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > General > Compare Match > Compare Match A > Compare match value | 0xa | 0xa | Set compare match A value is 10ms. |
| configuration.xml > Stacks > g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > General > Mode | One-Shot | One-Shot | Set GPT mode is One-Shot. |
| configuration.xml > Stacks > g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > General > Period | 0x19 | 0x19 | Set overflow value is 25ms. |
| configuration.xml > Stacks > g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > General > Period Unit | Milliseconds | Milliseconds | Set period unit is milliseconds. |
| configuration.xml > Stacks > g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > Input > Stop Source | GTETRGA Rising Edge | GTETRGA Rising Edge | Select GTETRGA Rising Edge as external source that will stop the timer. |
| configuration.xml > Stacks > g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_comms_smbus_rsc_ctrl0_timer Timer, General PWM (r_gpt) > Interrupts > Overflow/Crest Interrupt Priority | Disable | Priority 2 | Select the overflow interrupt priority. |

**Configuration Properties for using the Serial Terminal (UART)**   
|   Configure interrupt event path   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > SCI > SCI4 > SCI4 RXI (Receive data full) | empty | sci_uart_rxi_isr | Assign the UART receive ISR (Receive data full) to the interrupt vector table. |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > SCI > SCI4 > SCI4 TXI (Transmit data empty) | empty | sci_uart_txi_isr | Assign the UART transfer ISR (Transmit data empty) to the interrupt vector table. |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > SCI > SCI4 > SCI4 TEI (Transmit end) | empty | sci_uart_tei_isr | Assign the UART transfer ISR (Transmit end) to the interrupt vector table. |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > SCI > SCI4 > SCI4 ERI (Receive error) | empty | sci_uart_eri_isr | Assign the UART receive ISR (Receive error) to the interrupt vector table. |

## API Usage ##
The table below lists the FSP provided API used at the application layer in this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_IIC_MASTER_Open | This API is used to initialize IIC master module to communicate with sensor. |
| R_GPT_Open | This API is used to initialize GPT module to handle timeout for SMBus standard. |
| R_ELC_Open | This API is used to initialize ELC module to link IIC event to GPT. |
| RM_COMMS_SMBus_Open | This API is used to initialize SMBus Middleware driver. |
| RM_COMMS_SMBus_Write | This API is used to write data to specific register of sensor via SMBus. |
| RM_COMMS_SMBus_WriteRead | This API is used to read data from specific register of sensor via SMBus. |
| R_IIC_MASTER_Close | This API is used to de-initialize IIC master module. |
| R_GPT_Close |	This API is used to de-initialize GPT module. |
| R_ELC_Close |	This API is used to de-initialize ELC module. |
| RM_COMMS_SMBus_Close | This API is used to de-initialize SMBus Middleware driver. |
| R_BSP_SoftwareDelay | This API is used to delay a specified period of time. |

**For RA boards use IIC_B version:**

| API Name    | Usage                                                                                |
|-------------|--------------------------------------------------------------------------------------|
| R_IIC_B_MASTER_Open | This API is used to initialize IIC master module to communicate with sensor. |
| R_IIC_B_MASTER_Close | This API is used to de-initialize IIC master module.                        |

**API use for the Serial Terminal (UART):**

* **For RA boards use SCI_B version:**

| API Name    | Usage                                                     |
|-------------|-----------------------------------------------------------|
| R_SCI_B_UART_Open | This API is used to initialize SCI UART module.     |
| R_SCI_B_UART_Write | This API is used to perform a write operation.     |
| R_SCI_B_UART_Close | This API is used to de-initialize SCI UART module. |

* **For RA boards use SCI version:**

| API Name    | Usage                                                   |
|-------------|---------------------------------------------------------|
| R_SCI_UART_Open | This API is used to initialize SCI UART module.     |
| R_SCI_UART_Write | This API is used to perform a write operation.     |
| R_SCI_UART_Close | This API is used to de-initialize SCI UART module. |

## Verifying Operation ##
1. Import the example project.   
    By default, the EP supports Serial terminal for RA boards that support J-link OB VCOM
    * Define USE_VIRTUAL_COM = 1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> GNU Arm Cross C Compiler -> Preprocessor -> Defined symbols (-D)

    To use SEGGER J-Link RTT Viewer, please follow the instructions below:
    * Define USE_VIRTUAL_COM = 0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> GNU Arm Cross C Compiler -> Preprocessor -> Defined symbols (-D)

2. Before running the example project, make sure hardware connections are completed.
3. Download the example project to the RA board using the USB Debug Port.
4. For using the Serial Terminal (UART):  
- The configuration parameters of the serial port on the terminal application are as follows:
    * Baud rate: 115200 bps
	* Data length: 8-bits  
	* Parity: none
	* Stop bit: 1-bit
	* Flow control: none
  
- To show the degree Celsius symbol (°C), the user needs to select the "Consolas" font through: [Setup] -> [Font...] -> [Font:]
5. Open the terminal application and view output log.  
- The below images showcase the output on the Serial terminal application (Tera Term):  

![serial_log: SMBus EP information](images/serial_smbus_ep_info.png "SMBus EP information")  

![serial_log: SMBus operation](images/serial_smbus_operation.png "SMBus operation")  

- The images below showcase the output on the SEGGER J-Link RTT Viewer: 

![rtt_log: SMBus EP information](images/rtt_smbus_ep_info.png "SMBus EP information")  

![rtt_log: SMBus operation](images/rtt_smbus_operation.png "SMBus operation")

## Special Topics ##
The SMBus transaction time is monitored by the GPT module to ensure the MCU acting as a master device complies with the SMBus timeout standard.
| Communication     | Handle by                                                          |
|-------------|--------------------------------------------------------------------------|
| From start condition to acknowledge bit do not exceed 10 ms. | GPT compare match A |
| Between acknowledge bits do not exceed 10 ms. | GPT compare match A |
| From acknowledge bit to stop condition do not exceed 10 ms. | GPT compare match A |
| The total of above values do not exceed 25 ms. | GPT overflow |

The error LED information:
* The RA Boards use Red LED as error LED: EK-RA8M1, CK-RA6M5, EK-RA2A2, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4E2, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8P1, EK-RA4C1.
* The RA Boards use LED 1 as error LED: EK-RA2A1, EK-RA4M1, EK-RA4W1, EK-RA6M1, EK-RA6M2.
* The RA Boards use LED 2 as error LED: FPB-RA2E3, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T2,  MCK-RA6T3, MCK-RA8T1, RSSK-RA6T1, FPB-RA8E1, FPB-RA2T1.
