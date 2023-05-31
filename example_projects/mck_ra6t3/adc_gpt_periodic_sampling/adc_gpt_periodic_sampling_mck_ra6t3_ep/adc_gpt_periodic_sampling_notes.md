# Introduction #

The sample code accompanying this file shows the operation of an ADC GPT Periodic Sampling using the ADC, DTC, GPT, and ELC peripheral on a RA MCU.  
ADC GPT Periodic Sampling allows users to sample analog signals at periodic intervals with a double buffer. The frequency of sampling is
determined by the settings of the GPT linked to the ADC unit by the ELC. The DTC is used to transfer data from the ADC data registers to the circular buffer.  
Using the DTC allows the CPU to be used for other operations. When the first buffer is filled completely, the CPU executes the ADC Interrupt service routine to
switch to the DTC to fill the second buffer with ADC values, Raise a flag/event to allow processing on the data available in the first buffer.

Such functionality is popular for Data signal acquisition applications that require a continuous sampling of Analog signals. 


Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the ADC GPT Periodic Sampling, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit
* Micro USB cable
* Signal Generator
* Breadboard
* M-F Connector wires

Refer to [readme.txt](./readme.txt) on information on how to connect the hardware.

### Software ###
Refer to software described in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![adc_periodic_smapling](images/adc_periodic_high_level.jpg "ADC Periodic Sampling Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| ADC |ADC is used in group scan mode to scan the input sinusoidal signal connected to ADC channels| ADC |
| DTC |DTC is used to transfer data from the ADC data registers to the circular buffer | DTC |
| GPT |GPT is used to generate ADC group scan trigger at periodic interval| GPT |
| ELC |ELC is used to link ADC with GPT module for Hardware triggering| ELC|

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> g_adc0 ADC Driver on r_adc > Settings > Property > Module g_adc0 ADC Driver on R_ADC > General > Mode |   Single Scan   |   Group Scan   |  Specifies the mode that this ADC unit is used in. In group scan mode start conditions can be independently selected for group A, B, allowing A/D conversion of group A, B to be started independently  |
|   configuration.xml -> g_adc0 ADC Driver on r_adc > Settings > Property > Module g_adc0 ADC Driver on R_ADC > Input > Channel Scan Mask  |   NULL  |   Channel 0, Channel 1  |   Configures Channel 0 and 1 for ADC 0 group A.   |
|   configuration.xml -> g_adc0 ADC Driver on r_adc > Settings > Property > Module g_adc0 ADC Driver on R_ADC > Input > Channel Scan Mask  |   NULL  |   Channel 2, Channel 4  |   Configures Channel 2 and 4 for ADC 0 group B.   |
|   configuration.xml -> g_adc0 ADC Driver on r_adc > Settings > Property > Module g_adc0 ADC Driver on R_ADC > Interrupts > Normal/Group A Trigger  |   Software  |   GPT0 AD TRIG A  |   Specifies the trigger type to be used for this unit   |
|   configuration.xml -> g_adc0 ADC Driver on r_adc > Settings > Property > Module g_adc0 ADC Driver on R_ADC > Interrupts > Group B Trigger  |   Disable  |   GPT0 AD TRIG B  |   Specifies the trigger for Group B scanning in group scanning mode  |
|   configuration.xml -> g_adc0 ADC Driver on r_adc > Settings > Property > Module g_adc0 ADC Driver on R_ADC > Interrupts > Callback  |   NULL  |   g_adc0_callback  |   It is called from the interrupt service routine (ISR) each time the ADC scan completes   |
|   configuration.xml -> g_adc0 ADC Driver on r_adc > Settings > Property > Module g_adc0 ADC Driver on R_ADC > Interrupts > Scan End Interrupt Priorty  |   Disable |   Priority 2  |   Select scan end interrupt priority  |
|   configuration.xml -> g_adc0 ADC Driver on r_adc > Settings > Property > Module g_adc0 ADC Driver on R_ADC > Interrupts > Scan End Group B Interrupt Priorty|   Disable  |  Priority 2  |   Select group B scan end interrupt priority   |
|   configuration.xml -> g_timer0 Timer Driver on r_gpt > Settings > Property > Module g_timer0 Timer Driver on R_GPT > General > Period |   0x100000000  |  21  |   Time period for free running timer   |
|   configuration.xml -> g_timer0 Timer Driver on r_gpt > Settings > Property > Module g_timer0 Timer Driver on R_GPT > General > Period Unit |  Raw count  | Microseconds  | Unit of the period specified above  |
|   configuration.xml -> g_timer0 Timer Driver on r_gpt > Settings > Property > Module g_timer0 Timer Driver on R_GPT > Interrupt > Overflow/Crest Interrupt Priority |  Disable | Priority 1  | Select the timer overflow interrupt priority  |
|   configuration.xml -> g_timer0 Timer Driver on r_gpt > Settings > Property > Module g_timer0 Timer Driver on R_GPT > Interrupt > Capture A Interrupt Priority |  Disable | Disable  | Don't select the timer  Capture A interrupt priority  |
|   configuration.xml -> g_transfer_adc0_group_a on Transfer Driver on r_dtc > Settings > Property > Module g_transfer_adc0_group_a Transfer Driver on R_DTC ADC0 Scan End > Mode  |   Normal  |   Repeat  |  One transfer per activation, Repeat Area address reset after Number of Transfers, transfer repeats until stopped   |
|   configuration.xml -> g_transfer_adc0_group_a on Transfer Driver on r_dtc > Settings > Property > Module g_transfer_adc0_group_a Transfer Driver on R_DTC ADC0 Scan End > Destination Address Mode  |   Fixed |   Incremented  |  Select the address mode for the destination. Destination address get incremented after each transfer|
|   configuration.xml -> g_transfer_adc0_group_a on Transfer Driver on r_dtc > Settings > Property > Module g_transfer_adc0_group_a Transfer Driver on R_DTC ADC0 Scan End > Repeat Area |   Source  |   Destination |  Destination address resets to its initial value after completing Number of Transfers   |
|   configuration.xml -> g_transfer_adc0_group_a on Transfer Driver on r_dtc > Settings > Property > Module g_transfer_adc0_group_a Transfer Driver on R_DTC ADC0 Scan End > Number of Transfer  |   0  |   64  |  Specify the number of transfers to be performed |
|   configuration.xml -> g_transfer_adc0_group_a on Transfer Driver on r_dtc > Settings > Property > Module g_transfer_adc0_group_a Transfer Driver on R_DTC ADC0 Scan End > Activation Source  |   Disable  |   ADC0_SCAN_END  |  Select the DTC transfer start event |
|   configuration.xml -> g_timer0 Timer Driver on r_gpt > Settings > Property > Module g_timer0 Timer Driver on R_GPT > Extra Featues > ADC Trigger > ADC A Compare Match |  0  | 2000 | Compare match value that generates a GPTn AD TRIG A event |
|   configuration.xml -> g_timer0 Timer Driver on r_gpt > Settings > Property > Module g_timer0 Timer Driver on R_GPT > Extra Featues > ADC Trigger > ADC B Compare Match |  0  | 1000 | Compare match value that generates a GPTn AD TRIG B event |
|   configuration.xml -> g_timer0 Timer Driver on r_gpt > Settings > Property > Module g_timer0 Timer Driver on R_GPT > Extra Featues > ADC Trigger > Start Event Trigger |  Disable  | Trigger Event A/D Converter Start Request A During Up Counting, Trigger Event A/D Converter Start Request B During Up Counting  |Select which A/D converter start request interrupts to generate and at which point in the cycle to generate them. This value only applies to the GPT32E and GPT32EH variants |

## API Usage ##

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_ADC_Scanstart | This API is used to Start scanning of ADC channel connected to ADC peripheral, when ADC Periodic operation starts the API waits until trigger event are received.|
|R_DTC_Enable| This API is used to enable transfers on this activation source. when enable api is called it transfer data on every group scan complete event|
|R_GPT_Start| This API is used to Start GPT timer in periodic mode , when timer starts it trigger ADC group scan at set periodic interval.|
|R_DTC_Reconfigure| This API is used to reconfigure DTC with chain mode transfer info.|
|R_DTC_Close| This API is used to close the opened DTC instance, when any failure occur.|
|R_GPT_Close| This API is used to close the opened GPT instance, when any failure occur.|
|R_ADC_Close| This API is used to close the opened ADC instance, when any failure occur.|
|R_ELC_Close| This API is used to close the opened ELC instance, when any failure occur.|
 

## Verifying operation ##
### Setting up waveform renedering ###
* To set waveform rendering user needs to first debug the code and then go to memory view and add g_user buffer under memory monitor.
* User can view any of the respective channel by adding specific ADC Unit buffer under memory monitor, for e.g ADC Unit 0 Group A Channel 0 samples can be viewed by selecting &g_buffer_adc[0][0][0].
  This way other ADC Units, groups, and channels can be selected. 
* Then click on New Rendering -> Waveform -> Waveform properties
* Set the waveform properties as Data size to 16 bit, user-specified minimum and maximum values for Y-axis on signal generator, Channel to Mono, Buffer size as 512.
  **Note:** 12 bit ADC will have minimum and maximum value between 0-4095. As Ping-Pong buffer is used, the buffer size is 512 (i.e 256*2) for each channel, where 2 denotes the number of buffers and 256 is the size of the buffer.
    User can add the buffer address of each channel in the memory to view the waveform of each channel connected.
  
### Input for Signal generator ###
* The signal needs to be generated from the Signal Generator for the Input to the ADC (0-3V Sinusoidal) with a frequency set to 800Hz.

Import, Build and Debug the EP(*see section Starting Development* of **FSP User Manual**). After running the EP, open waveform rendering in memory viewer to see the output
of sampled data. One can also check the buffer in real-time by adding it to the expression tab.

Below images showcases the output on waveform rendering of ADC Unit 0 for each channel of group A and B:
* ADC Unit 0 Group A channel 0 (g_buffer_adc[0][0][0]) and channel 1 (g_buffer_adc[0][0][1]):
![ADC_Periodic_output](images/ADC0_GroupA_Channel0.png "Waveform Rendering of ADC Unit 0, Group A, Channel 0 Output")

 ![ADC_Periodic_output](images/ADC0_GroupA_Channel1.png "Waveform Rendering of ADC Unit 0, Group A, Channel 1 Output")


* ADC Unit 0 Group B channel 0 (g_buffer_adc[0][1][0]) and channel 1 (g_buffer_adc[0][1][1]):
![ADC_Periodic_output](images/ADC0_GroupB_Channel0.png "Waveform Rendering of ADC Unit 0, Group B, Channel 0 Output")

 ![ADC_Periodic_output](images/ADC0_GroupB_Channel1.png "Waveform Rendering of ADC Unit 0, Group B, Channel 1 Output")


## Special Topics ##
### Setting up Chain Mode in DTC ###
* The chain mode can be set up by declaring local transfer info structure containing info related to Chian Mode Transfer and configure DTC with the same info.
  The table below lists the transfer info elements required for chain tranfer
  | Transfer info element   | transfer info 0               |       transfer info 1              |
  |-------------|-------------------------------------------|------------------------------------|
  |Transfer mode | Normal mode| Normal mode|
  |Source address control | Fixed | Fixed |
  |Destination address control | Incremented| Incremented|
  |Chain transfer  | Enabled | Disable|
  |Transfer block size | 2 bytes | 2 bytes|
  |Transfer source address | R_ADDR0| &R_ADDR1|
  |Transfer destination address | g_buffer[unit0][Group scan A][channel 0][0][0]| g_buffer[unit0][Group scan A][channel 1][0][0]|
  
**Note:** Above table shows chain transfer for group A of ADC Unit 0 same way this can be updated for other ADC units and groups.
          User can also update their info as per requirement
* After A/D conversion is completed, the DTC is activated by an A/D Scan complete interrupt request.
  Once the DTC is activated, it reads transfer info for transfer_adc_groupA[0] which is a local buffer, and copy values of register ADDR0
  to the destination buffer location of DTC g_buffer_adc[0]. Since chain transfer is enabled, so the DTC successively reads transfer_adc_groupA[1](i.e channel 1 of group A).
* Once DTC operation for transfer_adc_groupA[1] starts it transfers values of registers ADDR1 to the destination buffer g_buffer_adc[1]. Since chain transfer is disabled in transfer info of A[1],
  data transfer by the DTC is completed here.
* DTC transfer completes the operation of one group, an A/D conversion interrupt is generated after DTC transfer is completed for the other ADC group 
  and the same process repeats for group B.
  
![DTC Chain Mode](images/dtc_data.jpg "DTC Chain Mode Block Diagram")

### Calculating Time period and ADC Trigger Raw count ###
* To calculate the Time period of gpt timer use 
  Time period = 1/input frequency (where input sampling frequency is 2 * signal generator input frequency).
  This calculated Time Period can be set as GPT Time period in the properties tab.
* To calculate Raw period count for ADC trigger use
   Raw period count =  (Time period in seconds * (PCLKD / (2^ pre scaler)))
  (where prescaler value is 0 by default so 2^0 = 1)
  **Note :** PCLKD frequency will vary as per MCU variant.
* ADC Trigger value for ADC Compare match value for A and B are set to 50% and 99% duty cycle(Raw period count) So ADC Compare match value for A = (0.5 * Raw period count) and ADC Compare match value for 
  B = (0.99 * Raw period count). ADC Compare A and B Match raw count values are supported for MCU's supporting enhanced gpt timer.
  For MCU's without enhanced gpt timer support, the user only needs to set GPT Time period as ADC group are set to trigger ADC group A at 50% of the cycle and ADC group B at full cycle (GTP overflow interrupt).
  
### Ping Pong Buffer Operation ###
* The DTC is used to transfer data from the ADC data registers to the Ping-Pong buffer.
* Using the DTC allows the CPU to be used for other operations.
* When the first buffer is filled completely(i.e desired sample size), a DTC activation source generates an interrupt to the CPU.
* The CPU executes the ADC Interrupt service routine to:
* Switch to the DTC to fill the second buffer with ADC values, and allow processing on the data available in the first buffer.

![Ping Pong](images/dtc_ping_pong.jpg "Ping Pong buffer operation diagram")
  
