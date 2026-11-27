# Introduction #
The example project demonstrates the basic functionality of ADC interval sampling using the TAU module on Renesas RA MCUs based on Renesas FSP. When ADC interval data sampling is performed, TAU triggers an ADC group scan at a periodic interval. When the group scan completes, the data transfer controller (DTC) triggers a data transfer, copying the scan result to a user buffer which is configured as ping-pong between the two buffers to allow for continuous processing of the acquired data. The sample data can be observed using waveform rendering in the memory viewer in e2studio.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the ADC TAU Interval Sampling, the following resources are needed.

### Software ###
Refer to software described in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware ###
* Supported RA boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1.
  * 1 x Renesas RA board.
  * 1 x USB cable for programming and debugging (USB cable type varies by board model).
  * 1 x Breadboard.
  * 1 x Signal generator (e.g., https://www.keysight.com/us/en/product/DSOX1202G/oscilloscope-70-100-200-mhz-2-analog-channels-waveform-generator.html).
  * Some jumper wires.

### Hardware Connections ###
* Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and debugging.

* The connection from the signal generator to the breadboard:
  * Connect an input signal (e.g., 800 Hz) to a horizontal rail on the breadboard.
  * Insert the pins listed below into the same horizontal row on the breadboard so that all pins are shorted together and connected to the input signal.
  * Connect the GND of the signal generator to the GND of the RA board.
  * Note: All channels are connected to the same input signal for testing. The user can configure the channels with different input frequencies if required.  

* For FPB-RA0E1 (Supporting 16-bit TAU timer):
  * To use P010 and P011 on J2: E26 and E25 must be closed.
  * ADC Unit: AN000 P010 (J2:32), AN001 P011 (J2:31).

* For FPB-RA0E2 (Supporting 16-bit TAU timer):
  * To use P010 and P011 on J2: E49 and E48 must be closed.
  * ADC Unit: AN000 P010 (J2:27), AN001 P011 (J2:26).

* For FPB-RA0L1 (Supporting 16-bit TAU timer):
  * To use the on-board debug functionality, the user must place jumper J9 on pins 1-2.
  * To use P010 and P011 on J2: E54 and E53 must be closed.
  * ADC Unit: AN000 P010 (J2:21), AN001 P011 (J2:20).

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
* [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
* [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #
## System Level Block Diagram ##
![adc_tau_interval_sampling](images/adc_tau_high_level.jpg "ADC TAU Interval Sampling Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|--------|-----------------------------------------------|
| ADC_D | ADC_D is used in group scan mode to scan the input sinusoidal signal connected to ADC channels. | ADC |
| DTC | DTC is used to transfer data from the ADC data registers to the circular buffer. | DTC |
| TAU | TAU is used to generate ADC group scan trigger at periodic interval. | TAU |
| ELC | ELC is used to link ADC with TAU module for Hardware triggering. | ELC |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different from those selected by default.

**Configuration Properties for ADC**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > g_adc0 ADC (r_adc_d) > Properties > Settings > Property > Module g_adc0 ADC (r_adc_d) > General > Conversion operation | One-Shot | One-Shot | Specifies the Conversion operation that this ADC is used in. |
| configuration.xml > Stacks > g_adc0 ADC (r_adc_d) > Properties > Settings > Property > Module g_adc0 ADC (r_adc_d) > General > Operation trigger | Wait | No-wait | Specifies the Operation trigger that this ADC is used in. |
| configuration.xml > Stacks > g_adc0 ADC (r_adc_d) > Properties > Settings > Property > Module g_adc0 ADC (r_adc_d) > Input > Channel Selection Mode | Select | Scan | Configures to scan Channel 0 and 1 for ADC group. |
| configuration.xml > Stacks > g_adc0 ADC (r_adc_d) > Properties > Settings > Property > Module g_adc0 ADC (r_adc_d) > Interrupts > Start trigger source | Timer channel 01 count or capture end interrupt signal (TAU0_TMI01) | TAU0 TMI00 (End of timer channel 00 count or capture) | Specifies the trigger for Group scanning. |
| configuration.xml > Stacks > g_adc0 ADC (r_adc_d) > Properties > Settings > Property > Module g_adc0 ADC (r_adc_d) > Interrupts > Callback | NULL | g_adc0_callback | It is called from the interrupt service routine (ISR) each time the ADC scan completes. |
| configuration.xml > Stacks > g_adc0 ADC (r_adc_d) > Properties > Settings > Property > Module g_adc0 ADC (r_adc_d) > Interrupts > Scan End Interrupt Priority | Disabled | Priority 2 | Select scan end interrupt priority. |

**Configuration Properties for TAU timer**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > g_timer0 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_timer0 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Period | 0x10000 | 21 | Time period for free running timer. |
| configuration.xml > Stacks > g_timer0 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_timer0 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Period Unit | Raw count | Microseconds | Unit of the period specified above. |
| configuration.xml > Stacks > g_timer0 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_timer0 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Interrupt Priority | Disabled | Priority 2 | Select the timer interrupt priority. |

**Configuration Properties for DTC**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Properties > Settings > Property > Module g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Mode | Normal | Repeat | One transfer per activation, Repeat Area address reset after Number of Transfers, transfer repeats until stopped. |
| configuration.xml > Stacks > g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Properties > Settings > Property > Module g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Destination Address Mode | Fixed | Incremented | Select the address mode for the destination. Destination address get incremented after each transfer. |
| configuration.xml > Stacks > g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Properties > Settings > Property > g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Repeat Area (Unused in Normal Mode) | Source | Destination | Destination address resets to its initial value after completing Number of Transfers. |
| configuration.xml > Stacks > g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Properties > Settings > Property > Module g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Number of Transfers | 0 | 64 | Specify the number of transfers to be performed. |
| configuration.xml > Stacks > g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Properties > Settings > Property > Module g_transfer_adc0_group Transfer (r_dtc) ADC0 SCAN END (End of A/D scanning operation) > Activation Source | Disable | ADC0_SCAN_END (End of A/D scanning operation) | Select the DTC transfer start event. |

## API Usage ##
The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_ADC_D_Open | This API is used to open ADC_D Module. |
| R_ADC_D_ScanCfg | This API is used to configure scan setting for ADC Module. |
| R_ADC_D_ScanStart | This API is used to start scanning of ADC channel connected to ADC peripheral, when ADC Periodic operation starts the API waits until trigger event are received. |
| R_ADC_D_Close | This API is used to close the opened ADC Module, when any failure occurred. |
| R_TAU_Open | This API is used to open TAU timer in interval timer function. |
| R_TAU_Start | This API is used to start TAU timer in interval timer function, when timer starts it trigger ADC group scan at set interval timer.|
| R_TAU_Close | This API is used to close the opened TAU Module, when any failure occurred. |
| R_ELC_Open | This API is used to open ELC Module. |
| R_ELC_Enable | This API is used to enable ELC function. |
| R_ELC_Close | This API is used to close the opened ELC Module, when any failure occurred. |
| R_DTC_Open | This API is used to open DTC Module. |
| R_DTC_Enable | This API is used to enable transfers on this activation source. When enable API is called it transfer data on every group scan complete event. |
| R_DTC_Reconfigure | This API is used to reconfigure DTC with chain mode transfer info. |
| R_DTC_Close | This API is used to close the opened DTC Module, when any failure occurred. |

## Verifying Operation ##
1. Set up the hardware connection properly mentioned in [Hardware Connections](#hardware-connections).
2. Import, generate, build, and download project to the RA board.
3. Open a serial terminal application (e.g., Tera Term) to view output, configure the settings as follows:

    * To make sure the received data shows properly, the user needs to set:
      * [Setup] -> [Terminal...] -> [New-line] -> Select [Receive: CR] & [Transmit: CR]. 

    * To echo back characters typed in Tera Term, the user needs to enable it through: 
      * [Setup] -> [Terminal...] -> Check [Local echo]. 

    * The configuration parameters of the serial port on the serial terminal application:
      * Port: The port is provided by the J-Link On-Board.
      * Speed: 115200
      * Data: 8 bit
      * Parity: none
      * Stop bits: 1 bit
      * Flow control: none

### The image below showcases the output log ###

![adc_tau_interval_sampling_output_log](images/adc_tau_interval_sampling_output_log.jpg "ADC TAU Interval Sampling Output Log")

### Setting up waveform rendering ###
* To set up waveform rendering, first start a debug session, then open the memory view and add the g_user buffer under memory monitor.
* The user can view any of the respective channel by adding specific ADC Unit buffer under memory monitor. For example ADC Unit 0 Group A Channel 0 samples can be viewed by selecting &g_buffer_adc[0][0][0].
  This way other ADC Units, groups, and channels can be selected. 
* Then click on New Rendering -> Waveform -> Waveform properties
* The configuration parameters of the Waveform properties are as follows:
  * Select Data Size: 16bit
  * User-specified: Tick on the box
  * Minimum values: 0
  * Maximum values: 4095
  * Channel: Mono
  * Buffer size: 512

**Note:** 12-bit ADC will have minimum and maximum value between 0-4095. As Ping-Pong buffer is used, the buffer size is 512 (i.e., 256*2) for each channel, where 2 denotes the number of buffers and 256 is the size of the buffer. The user can add the buffer address of each channel in the memory to view the waveform of each channel connected.

![Setting_Waveform_Properties](images/setting_waveform_properties.jpg "Set the waveform properties") 

### Input for Signal generator ###
* This example project is tested using an 800 Hz sine-wave input signal with an amplitude range of 0 V to 3.3 V.  
* Import, build and debug the EP (*see section Starting Development* of **FSP User Manual**). After running the EP, open waveform rendering in memory viewer to see the output of sampled data. One can also check the buffer in real-time by adding it to the expression tab. 

Click Real-time Refresh on Memory monitor tab to check buffer in real-time:

![Real_time_setting](images/real_time_setting.jpg "Real-time Refresh")

The images below showcase the output on waveform rendering of ADC for each channel of group:
* ADC Group channel 0 (g_buffer_adc[0][0][0]):

![ADC_Periodic_output](images/adc0_group_channel0.jpg "Waveform Rendering of ADC Group, Channel 0 Output")

* ADC Group channel 1 (g_buffer_adc[0][0][1]):

 ![ADC_Periodic_output](images/adc0_group_channel1.jpg "Waveform Rendering of ADC Group, Channel 1 Output")

## Special Topics ##
### Setting up Chain Mode in DTC ###
* The chain mode can be set up by declaring local transfer info structure containing info related to Chain Mode Transfer and configure DTC with the same info.
  The table below lists the transfer info elements required for chain transfer
  | Transfer info element | transfer info 0 | transfer info 1 |
  |-----------------------|-----------------|-----------------|
  | Transfer mode | Normal mode | Normal mode |
  | Source address control | Fixed | Fixed |
  | Destination address control | Incremented| Incremented |
  | Chain transfer | Enabled | Disable |
  | Transfer block size | 2 bytes | 2 bytes|
  | Transfer source address | &R_ADDR0 | &R_ADDR1 |
  | Transfer destination address | g_buffer[unit 0][Group scan][channel 0][0][0] | g_buffer[unit 0][Group scan][channel 1][0][0] |
  
**Note:** Above table shows chain transfer for group of ADC Unit 0 same way this can be updated for other ADC units and group. The user can also update their info as per requirement
* After A/D conversion is completed, the DTC is activated by an A/D Scan complete interrupt request.
  Once the DTC is activated, it reads transfer info for transfer_adc_group[0] which is a local buffer, and copy values of register ADDR0
  to the destination buffer location of DTC g_buffer_adc[0]. Since chain transfer is enabled, the DTC successively reads transfer_adc_group[1](i.e channel 1 of ADC unit 0 group).
* Once DTC operation for transfer_adc_group[1] starts it transfers values of registers ADDR1 to the destination buffer g_buffer_adc[1]. Since chain transfer is disabled in transfer info of A[1],
  data transfer by the DTC is completed here.
  
![DTC Chain Mode](images/dtc_data.jpg "DTC Chain Mode Block Diagram")

### Calculating Time period and ADC Trigger Raw count ###
* To calculate the time period of TAU timer use:
  * Time period = 1 / Input frequency (where input sampling frequency is 2 * signal generator input frequency).
  * This calculated time period can be set as the TAU Time period in the properties tab.
* To calculate raw period count for ADC trigger, use:
  * Raw period count = (Time period in seconds * (ICLK / (2^ prescaler))) 
    * (where prescaler value is 0 by default, so 2^0 = 1)
  * **Note :**  ICLK frequency will vary depending on the MCU variant.

### Ping Pong Buffer Operation ###
* The DTC is used to transfer data from the ADC data registers to the Ping Pong buffer.
* Using the DTC allows the CPU to be used for other operations.
* When the first buffer is filled completely (i.e., desired sample size), a DTC activation source generates an interrupt to the CPU.
* The CPU executes the ADC Interrupt service routine to:
  * Switch to the DTC to fill the second buffer with ADC values.
  * Allow processing on the data available in the first buffer.

![Ping Pong](images/dtc_ping_pong.jpg "Ping Pong buffer operation diagram")
