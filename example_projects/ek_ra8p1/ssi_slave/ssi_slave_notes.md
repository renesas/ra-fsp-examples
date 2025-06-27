# Introduction #
This project demonstrates the basic functionalities of SSI driver and streaming example of SSI Slave on Renesas RA MCUs based on Renesas FSP. The project configures two RA boards, the first board with one channel as the SSI Master and the second board with one channel as the SSI Slave. The EP is configured with two options, is displayed on both the SSI Master and the SSI Slave EPs. The selection of which can be done from the J-Link RTT Viewer.
1. Master Write - Slave Read
2. Slave Write - Master Read

On Selecting Master Write - Slave Read on both Master and Slave, the Master starts writing data, and the Slave reads the data. On Selecting Slave Write - Master Read on both Master and Slave, the Slave starts writing data, and the Master reads the data. The two example projects ssi_master_ep and ssi_slave_ep are provided for the SSI Slave EP, one intended for Master and other for Slave.
 
Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the SSI example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.0.0
* e2 studio: Version 2025-04.1
* SEGGER J-Link RTT Viewer: Version 8.44a
* LLVM Embedded Toolchain for ARM: Version 18.1.3

Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware ###
* Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA4E2, EK-RA6E2, EK-RA8D1, EK-RA8M1, FPB-RA8E1, EK-RA4L1, EK-RA8E2, EK-RA2L2, EK-RA8P1.
   * 2 x Renesas RA boards.
   * 2 x Type-C USB cables for programming and debugging.
   * 1 x Host PC with at least 2 USB ports (1 for Master and 1 for Slave).
   * Some jumper wires.

### Hardware Connections: ###
* Connect the debug port of the RA board to the USB port of the host PC via a Type-C USB cable.
* Pins and Jumper Settings:
   * For EK-RA6M2, EK-RA6M3:
      * SSISCK P112 (Master) -----> SSISCK P112 (Slave)
      * SSIWS  P113 (Master) -----> SSIWS  P113 (Slave)
      * SSITXD P115 (Master) -----> SSIRXD P114 (Slave)
      * SSIRXD P114 (Master) -----> SSITXD P115 (Slave)
      * GND         (Master) -----> GND 	(Slave)
	   
   * For EK-RA6M1:
      * SSIBCK  P403 (Master) -----> SSIBCK  P403 (Slave)
      * SSILRCK P404 (Master) -----> SSILRCK P404 (Slave)
      * SSITXD  P405 (Master) -----> SSIRXD  P406 (Slave)
      * SSIRXD  P406 (Master) -----> SSITXD  P405 (Slave)
      * GND          (Master) -----> GND  	  (Slave)
	   
   * For EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5:
      * SSIBCK0  P112 (Master) -----> SSIBCK0  P112 (Slave)
      * SSILRCK0 P113 (Master) -----> SSILRCK0 P113 (Slave)
      * SSITXD0  P115 (Master) -----> SSIRXD0  P114 (Slave)
      * SSIRXD0  P114 (Master) -----> SSITXD0  P115 (Slave)
      * GND           (Master) -----> GND	    (Slave)

   * For EK-RA4E2, EK-RA6E2:
      * SSIBCK0  P102 (Master) -----> SSIBCK0  P102 (Slave)
      * SSILRCK0 P103 (Master) -----> SSILRCK0 P103 (Slave)
      * SSITXD0  P109 (Master) -----> SSIRXD0  P110 (Slave)
      * SSIRXD0  P110 (Master) -----> SSITXD0  P109 (Slave)
      * GND           (Master) -----> GND	    (Slave)
      * For EK-RA4E2 (Master):
         * GPT1 P105 <--> AUDIO_CLK P100

   * For EK-RA8D1:
      * SSIBCK0  P403 (Master) -----> SSIBCK0  P403 (Slave)
      * SSILRCK0 P404 (Master) -----> SSILRCK0 P404 (Slave)
      * SSITXD0  P405 (Master) -----> SSIRXD0  P406 (Slave)
      * SSIRXD0  P406 (Master) -----> SSITXD0  P405 (Slave)
      * GND           (Master) -----> GND	    (Slave)
      * The user needs to turn OFF SW1-3 and SW1-5 to use SSI.

   * For EK-RA8M1:
      * SSIBCK0  P112 (Master) -----> SSIBCK0  P112 (Slave)
      * SSILRCK0 P113 (Master) -----> SSILRCK0 P113 (Slave)
      * SSITXD0  P405 (Master) -----> SSIRXD0  P114 (Slave)
      * SSIRXD0  P114 (Master) -----> SSITXD0  P405 (Slave)
      * GND           (Master) -----> GND 	    (Slave)
      * EK-RA8M1: Remove Jumper J61 to use P112, P114, P405 for SSI.

   * For FPB-RA8E1:
      * SSIBCK0  P112 (J3:18) (Master)  -----> SSIBCK0  P112 (J3:18) (Slave)
      * SSILRCK0 P113 (J3:17) (Master)  -----> SSILRCK0 P113 (J3:17) (Slave)
      * SSITXD0  P405 (J4:14) (Master)  -----> SSIRXD0  P114 (J3:15) (Slave)
      * SSIRXD0  P114 (J3:15) (Master)  -----> SSITXD0  P405 (J4:14) (Slave)
      * GND           (J3:20) (Master)  -----> GND           (J3:20) (Slave)

   * For EK-RA4L1:
      * SSIBCK0  P113 (J2:41) (Master)  -----> SSIBCK0  P113 (J2:41) (Slave)
      * SSILRCK0 P404 (J2:44) (Master)  -----> SSILRCK0 P404 (J2:44) (Slave)
      * SSITXD0  P405 (J2:42) (Master)  -----> SSIRXD0  P406 (J2:38) (Slave)
      * SSIRXD0  P406 (J2:38) (Master)  -----> SSITXD0  P405 (J2:42) (Slave)
      * GND 	      (J4:25) (Master)  -----> GND           (J4:25) (Slave)
		
   * For EK-RA8E2:
      * SSIBCK0  P112 (J14:24) (Master)  -----> SSIBCK0  P112 (J14:24) (Slave)
      * SSILRCK0 P113 (J14:22) (Master)  -----> SSILRCK0 P113 (J14:22) (Slave)
      * SSITXD0  P115 (J14:8)  (Master)  -----> SSIRXD0  P114 (J14:10) (Slave)
      * SSIRXD0  P114 (J14:10) (Master)  -----> SSITXD0  P115 (J14:8)  (Slave)
      * GND           (J14:26) (Master)  -----> GND           (J14:26) (Slave)

   * For EK-RA2L2:
		* SSIBCK0  P411 (J2:5)  (Master)  -----> SSIBCK0  P411 (J2:5)  (Slave) 
		* SSILRCK0 P410 (J2:6)  (Master)  -----> SSILRCK0 P410 (J2:6)  (Slave)
		* SSITXD0  P409 (J2:7)  (Master)  -----> SSIRXD0  P408 (J2:8)  (Slave) 
		* SSIRXD0  P408 (J2:8)  (Master)  -----> SSITXD0  P409 (J2:7)  (Slave) 
		* GND 	       (J2:40) (Master)  -----> GND 	       (J2:40) (Slave)

   * For EK-RA8P1:
      * SSIBCK0  P112 (J37:23) (Master)  -----> SSIBCK0  P112 (J37:23) (Slave)
      * SSILRCK0 P113 (J14:24) (Master)  -----> SSILRCK0 P113 (J14:24) (Slave)
      * SSITXD0  P115 (J37:22) (Master)  -----> SSIRXD0  P114 (J37:21) (Slave)
      * SSIRXD0  P114 (J37:21) (Master)  -----> SSITXD0  P115 (J37:22) (Slave)
      * GND           (J17:19) (Master)  -----> GND           (J17:19) (Slave)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
High level block diagram

![ssi_slave](images/ssi_slave_block_diagram.JPG "SSI Slave Block Diagram")

## FSP Modules Used ##
List of all the various modules that are used in this ssi_master_ep example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| I2S | SSI driver for Master mode. | r_ssi |
| General PWM | Master configures General PWM for SSI Audio Clock. | r_gpt |

List of all the various modules that are used in this ssi_slave_ep example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| I2S | SSI driver for Slave mode. | r_ssi|

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

**Common Configuration Properties for SSI Master and SSI Slave**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes)  |  0x400  | 0x800  |  Change stack size to accommodate functions usage in the EP and avoid any runtime errors. |
|   configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes)  |  0   | 0x400  |  Change heap size to accommodate functions usage in the EP and avoid any runtime errors. |
|   configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Common  >  DTC Support | Disabled | Enabled | Enable DTC support for the module. |

**Configuration Properties for SSI Master**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > Channel | 0 | 0 | SSI Channel. |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > Operating Mode (Master/Slave) | Master Mode | Master Mode | SSI Master Configuration. |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > Bit Depth | 16 Bits | 32 bits | Bit Depth. |
| configuration.xml > Stacks >g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > Word Length | 16 Bits | 32 bits | This property is used to specify Word Length. |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > WS Continue Mode | Disabled | Disabled | This property is used to enable/disable WS Continue Mode. |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > Bit Clock Source(available only in Master mode) | External AUDIO_CLK | Internal AUDIO_CLK | This property is used to set Bit Clock Source. |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) >  Bit Clock Divider(available only in Master mode) | Audio Clock / 1 | Audio Clock/64 | This property is used to set Bit clock Divider. |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > Callback | NULL | ssi_example_callback | Defined user callback for SSI events. |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > Transmit Interrupt Priority | Disabled | Priority 2 | This property is used to set Transmit Interrupt Priority. |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > Receive Interrupt Priority | Disabled | Priority 2 | This property is used to set Receive Interrupt Priority. |
| configuration.xml > Stacks > g_i2s_master I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_master I2S (r_ssi) > Idle/Error Interrupt Priority | Disabled | Priority 2 | This property is used to set Idle/Error Interrupt Priority. |

**Configuration Properties for SSI Slave**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Channel | 0 | 0 | SSI Channel. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Operating Mode (Master/Slave) | Master Mode | Slave Mode | SSI Slave Configuration. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Bit Depth | 16 Bits | 32 bits | Bit Depth. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Word Length | 16 Bits | 32 bits | This property is used to specify Word Length. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > WS Continue Mode | Disabled | Disabled | This property is used to enable/disable WS Continue Mode. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Bit Clock Source(available only in Master mode) | External AUDIO_CLK | External AUDIO_CLK | This property is used to set Bit Clock Source. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Bit Clock Divider(available only in Master mode) | Audio Clock / 1 | Audio Clock/64 | This property is used to set Bit clock Divider. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Callback | NULL | ssi_example_callback | Defined user callback for SSI events. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Transmit Interrupt Priority | Disabled | Priority 2 | This property is used to set Transmit Interrupt Priority. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Receive Interrupt Priority | Disabled | Priority 2 | This property is used to set Receive Interrupt Priority. |
| configuration.xml > Stacks > g_i2s_slave I2S (r_ssi) > Properties > Settings > Property > Module g_i2s_slave I2S (r_ssi) > Idle/Error Interrupt Priority | Disabled | Priority 2 | This property is used to set Idle/Error Interrupt Priority. |

## API Usage ##

The table below lists the SSI basic APIs used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_SSI_Open| This API is used to open the SSI instance. |
|R_SSI_Stop| This API is used to stop SSI. |
|R_SSI_Write| This API is used to write data buffer to SSI. |
|R_SSI_Read| This API is used to read data into the provided buffer. |
|R_SSI_Close| This API is used to close the SSI instance. |
|R_GPT_Open| This API is used to initialize the General PWM configured as SSI Audio Clock. |
|R_GPT_Start| This API is used to start SSI Audio Clock. |
|R_GPT_Close| This API is used to stop the SSI Audio Clock. |

## Verifying operation ##
* Import, Build and Debug the Example Projects for Master and Slave (see section Starting Development of **FSP User Manual**).
* Connect Type-C USB cables and connect wires as mentioned in the **Hardware Connections** section above.
* Run the Example Projects on Master and Slave respectively and then open two instances of J-Link RTT Viewer to see the output status of the Example Projects for Master and Slave.
* Press "1", on both the EPs (Master and Slave sequentially). For the Master Write - Slave Read operation, after the transaction is completed, turn off the Slave first by pressing any key, and then for the Master press any key to terminate the process.
* Press "2", on both the EPs (Master and Slave sequentially). For the Master Read - Slave Write operation, after the transaction is completed, turn off the Master first by pressing any key, and then for the Slave press any key to terminate the process.
* The transaction/status messages will be displayed in J-Link RTT Viewer as depicted below:

Master Write RTT Viewer Log

![master_write_rtt_log](images/master_write_rtt_log.JPG "Master Write RTT Log")

Slave Read RTT Viewer Log

![slave_read_rtt_log](images/slave_read_rtt_log.JPG "Slave Read RTT Log")

Master Read RTT Viewer Log

![master_read_rtt_log](images/master_read_rtt_log.JPG "Master Read RTT Log")

Slave Write RTT Viewer Log

![slave_write_rtt_log](images/slave_write_rtt_log.JPG "Slave Write RTT Log")

### Special Topics 
#### Callback Handlers
1. **ssi_example_callback()** reloads FIFO's and handles errors.

#### SSI Master Clock Settings 
| No.   | Board       | SSI Master Clock Channel settings                                              |
|-------|-------------|--------------------------------------------------------------------------------|
|  1    |EK_RA6M1     | 1                                                                              |
|  2    |EK-RA6M2     | 1                                                                              |
|  3    |EK-RA6M3     | 1                                                                              |
|  4    |EK-RA6M4     | 2                                                                              |
|  5    |EK-RA6M5     | 2                                                                              |
|  6    |EK-RA4M2     | 2                                                                              |
|  7    |EK-RA4M3     | 2                                                                              |
|  8    |EK-RA4E2     | 1                                                                              |
|  9    |EK-RA6E2     | 2                                                                              |
|  10   |EK-RA8D1     | 2                                                                              |   
|  11   |EK-RA8M1     | 2                                                                              |
|  12   |FPB-RA8E1    | 2                                                                              |   
|  13   |EK-RA4L1     | 2                                                                              |   
|  14   |EK-RA8E2     | 2                                                                              |
|  15   |EK-RA2L2     | 4                                                                              |
|  16   |EK-RA8P1     | 2                                                                              |