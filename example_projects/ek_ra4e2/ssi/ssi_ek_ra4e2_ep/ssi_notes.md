# Introduction #
This example project demonstrates audio streaming using the SSI module with a double-buffer mechanism. A sine wave is generated and continuously streamed via a loopback connection between the SSITX and SSIRX pins. The sine wave data is stored in two buffers, alternating between them to ensure continuous and uninterrupted streaming. The transmitted and received audio data are compared to verify the successful completion of the SSI transfer. The results will be displayed on the RTT viewer. The sample data can be observed using waveform rendering in the memory viewer of e2studio.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the SSI example project, the following resources are needed.

### Software Requirements ###
* Renesas Flexible Software Package (FSP): Version 6.0.0
* e2 studio: Version 2025-04.1
* SEGGER J-Link RTT Viewer: Version 8.44a
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware Requirements ###
* Supported RA boards: EK-RA4E2, EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8E2, EK-RA8M1, FPB-RA6E1, FPB-RA8E1.
* 1 x Renesas RA board.
* 1 x Micro USB cable or Type-C USB cable (for EK-RA4L1, EK-RA8E2, FPB-RA8E1).
* 1 x Host PC.
* 1 x Jumper wire.

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Hardware Connections: ###
Pin and Jumper Settings:  
* For EK-RA4E2:
   * Connect P301 (J2:24) (SSITXD pin)	<--> P302 (J2:23) (SSIRXD pin)
	* P105 (J2:34) (GPT1)		<--> P100 (J2:39) (AUDIO_CLK)

* For EK-RA4L1:
   * Connect P405 (J2:42) (SSITXD pin)	<--> P406 (J2:38) (SSIRXD pin)

* For EK-RA4M1:
   * Connect P405 (J2:31) (SSITXD pin)	<--> P406 (J2:15) (SSIRXD pin)
		
* For EK-RA4M2:
   * Connect P405 (J1:7) (SSITXD pin)	<--> P406 (J1:10) (SSIRXD pin)

* For EK-RA6E2:
   * Connect P301 (J2:24) (SSITXD pin)	<--> P302 (J2:23) (SSIRXD pin)

* For EK-RA6M1:
   * Connect P405 (J1:16) (SSITXD pin)	<--> P406 (J1:18) (SSIRXD pin)

* For EK-RA6M2:
   * Connect P405 (J1:6) (SSITXD pin)	<--> P406 (J1:8) (SSIRXD pin)

* For EK-RA6M3, EK-RA6M3G:
   * Connect P405 (J3:13) (SSITXD pin)	<--> P406 (J3:16) (SSIRXD pin)
   * Cut E14 to get signal output for SSITXD pin and SSIRXD pin.

* For EK-RA4M3, EK-RA6M4:
   * Connect P405 (J1:7) (SSITXD pin)	<--> P406 (J1:8) (SSIRXD pin)

* For EK-RA6M5:
   * Connect P405 (J1:8) (SSITXD pin)	<--> P406 (J3:9) (SSIRXD pin)
   * Cut E22 to get signal output for SSITXD pin and SSIRXD pin.

* For EK-RA8D1:
   * Connect P405 (J51:8) (SSITXD pin)	<--> P406 (J51:5) (SSIRXD pin)
	* Turn OFF SW1-3 and SW1-5 to get signal output for SSITXD pin and SSIRXD pin.

* For EK-RA8E2:
   * Connect P114 (J14:10) (SSITXD pin)	<--> P115 (J14:8) (SSIRXD pin)

* For EK-RA8M1:
   * Connect P405 (J56:29) (SSITXD pin)	<--> P406 (J56:27) (SSIRXD pin)
	* Install jumper J61 to get signal output for SSITXD pin and SSIRXD pin.

* For FPB-RA6E1:
   * Connect P405 (J3:6) (SSITXD pin)	<--> P406 (J3:7) (SSIRXD pin)
	* P103 (J4:22) (GPT2)	<--> P402 (J3:3) (AUDIO_CLK)

* For FPB-RA8E1:
   * Connect P405 (J4:14) (SSITXD pin)	<--> P406 (J4:13) (SSIRXD pin)

## Related Collateral References ##
Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

# Project Notes #

## System Level Block Diagram ##
High level block diagram

![ssi_block_diagram](images/ssi_block_diagram.png "SSI Block Diagram")

## FSP Modules Used ##
List of all the various modules that are used in this ssi_ep example project. Refer to the FSP User Manual for more details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| I2S | SSI driver | r_ssi |
| General PWM | Provide an audio clock for SSI communication | r_gpt |


## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

**Common Configuration Properties for SSI**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes)  |  0x400  | 0x400  |  Change stack size to accommodate function usage in the EP and avoid any runtime errors. |
|   configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes)  |  0   | 0x400  |  Change heap size to accommodate function usage in the EP and avoid any runtime errors. |
|   configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Common  >  DTC Support | Disabled | Enabled | Enable DTC support for the module. |

**Configuration Properties for SSI**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > Channel| 0 | 0 | SSI Channel. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > Operating Mode (Master/Slave)| Master Mode | Master Mode| SSI Configuration. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > Bit Depth | 16 Bits | 16 bits| Bit Depth. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > Word Length | 16 Bits| 16 bits | This property is used to specify Word Length. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > WS Continue Mode | Disabled | Disabled | This property is used to enable/disable WS Continue Mode. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > Bit Clock Source (available only in Master mode) | External AUDIO_CLK| Internal AUDIO_CLK | This property is used to set Bit Clock Source. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) >  Bit Clock Divider (available only in Master mode)  | Audio Clock / 1| Audio Clock/2 | This property is used to set Bit Clock Divider. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > Callback | NULL | i2s_callback | Defined user callback for SSI events. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > Transmit Interrupt Priority | Disabled | Priority 2| This property is used to set Transmit Interrupt Priority. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > Receive Interrupt Priority  | Disabled  | Priority 2 | This property is used to set Receive Interrupt Priority. |
| configuration.xml > g_i2s I2S (r_ssi) > Properties > Settings > Property > Module g_i2s I2S (r_ssi) > Idle/Error Interrupt Priority  | Disabled  | Priority 2 | This property is used to set Idle/Error Interrupt Priority. |

**Configuration Properties for GPT**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > g_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer Timer, General PWM (r_gpt) > General > Channel | 0 | 1 | Channel of GPT to get audio clock for SSI communication |
| configuration.xml > g_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer Timer, General PWM (r_gpt) > General > Period | 0x10000 | 1411200 | Period to get audio clock for SSI communication |
| configuration.xml > g_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer Timer, General PWM (r_gpt) > General > Period Unit | Raw Counts | Hertz | Unit of the period specified above |

## API Usage ##

The table below lists the SSI basic APIs used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_SSI_Open| This API is used to open the SSI instance. |
|R_SSI_Stop| This API is used to stop SSI. |
|R_SSI_WriteRead| This API is used to writes from source buffer and reads data into destination buffer. |
|R_SSI_Close| This API closes the SSI instance. |
|R_GPT_Open| This API is used to initialize the General PWM configured as SSI Audio Clock. |
|R_GPT_Start| This API is used to start SSI Audio Clock. |
|R_GPT_Close| This API is used to stop the SSI Audio Clock. |

## Verifying operation ##
* Connect USB cables and connecting wires as mentioned in the readme file [readme.txt](./readme.txt)
* Import, Build and Debug the EP(*see section Starting Development* of **FSP User Manual**). After running the EP, open waveform rendering in memory viewer to see the output of sampled data. One can also check the buffer by adding it to the expression tab.

### Setting up waveform rendering ###
* While debugging the project, enter the memory view and add "g_src_buff" (transmitted data) and "g_dest_buff" (received data) as variables to monitor.
* Then click on New Rendering -> Waveform -> Waveform properties
* Set the waveform properties as Data size to 16 bit, user-specified minimum and maximum values for Y-axis on signal generator, Channel to Mono, Buffer size as 2048.

![waveform_properties](images/waveform_properties.png "Waveform Properties for SSI")
   
   **Note:** User can add the buffer address of each channel in the memory to view the waveform of each channel connected.
  
The images below showcase the output of waveform rendering for SSI:

* Waveform Rendering of SSI source buffer (g_src_buff):

![waveform_src_buffer](images/waveform_src_buffer.png "Waveform Rendering of SSI source buffer")
* Waveform Rendering of SSI destination buffer (g_dest_buff):
   * Note: 
      * The `g_dest_buff` buffer is cleared before new incoming data is stored, which may cause differences between the waveform viewed in **Memory View** and the capture image below. This is expected behavior due to the double-buffering mechanism used in streaming.
      * For accurate validation, the user should set a breakpoint at **line 143** (This code line used memcmp function to compare `g_src_buff` and `g_dest_buff`) in `...\e2studio\src\ssi_ep.c` to validate the waveform.

![waveform_dest_buffer](images/waveform_dest_buffer.png "Waveform Rendering of SSI destination buffer")

* Open an instance of J-Link RTT Viewer to see the output status of the Example Projects.
* The transaction/status messages will be displayed in J-Link RTT Viewer as depicted below:

SSI RTT Viewer Log

![ssi_rtt_log](images/ssi_rtt_log.png "SSI RTT Viewer Log")


## Special Topics ##
### Calculate Audio Clock Frequency ###
* The audio clock source comes from an internal connection to the GPT timer output, so the user can change the "Period" to get a different audio sample rate.
* The bit clock frequency is the product of the sampling frequency, number of channels and bits per system word:
   * bit_clock (Hz) = sampling_frequency (Hz) * channels * system_word_bits
* I2S data always has 2 channels.
* For example, the bit clock for transmitting 2 channels of 16-bit data (using a 16-bit system word) at 22050 Hz would be:
   * 22050 * 2 * 16 = 705600Hz
* The audio clock frequency is used to generate the bit clock frequency. It must be a multiple of the bit clock frequency. Refer to the Bit Clock Divider configuration for divider options. The input audio clock frequency must be:
   * audio_clock (Hz) = desired_bit_clock (Hz) * bit_clock_divider
   * To get a bit clock of 705600Hz from an audio clock of 1411200Hz, select the divider Audio Clock/2.



	












