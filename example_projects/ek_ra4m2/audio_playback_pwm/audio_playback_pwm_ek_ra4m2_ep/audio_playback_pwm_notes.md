# Introduction #

This project demonstrates Audio Playback with PWM module's functionality. It uses ADPCM module to decode the PCM data. The Decoded PCM data is in signed 16 bit format. The Application converts this signed PCM data into unsigned PCM data and provide the converted unsigned data to the audio playback with PWM module.The converted unsigned data is provided as input and output is generated as PWM signal.
The application status messages will be displayed on the Jlink RTT Viewer.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the Audio Playback example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit - 1x
* Digilent PMOD AMP2 (SKU: 410-233) - 1x
* Wired Headsets with 3.5mm audio jack) - 1x
* Connection wires 

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
1. Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)


## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)


# Project Notes #

## System Level Block Diagram ##
 High level block diagram of the system is as shown below:
 
 
![audio](images/Block_Diagram.jpg "High Level Block Diagram")


## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
|Audio Playback PWM Driver|The Audio Playback with PWM middleware is used to play audio streams at user selected playback rate using PWM hardware on  AGT timers|Audio Playback PWM Driver
|ADPCM Decoder|The ADPCM Audio Decoder is used to decodes 4-bit ADPCM input data into 16-bit PCM output data|ADPCM Decoder
|DTC |DTC is used to transfer the audio data at faster rate without using the CPU|r_dtc
|AGT |AGT is used to perform PWM mode operation|r_agt



## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

**Configuration Properties for using Audio Playback PWM**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Audio Playback PWM Stacks > g_rm_audio_playback Instance > Properties > Settings > Property > g_rm_audio_playback > General > Playback Speed(Hz) | 44100 | 16000 | Playback speed is set to 16KHz to match the sampling rate used to encode data used as input for playback.|


**Configuration Properties for using DTC**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Transfer Driver Stacks > g_transfer0 Instance > Properties > Settings > Property > Module g_transfer0 on r_dtc > Activation Source| Disabled| AGTx INT(AFT Interrupt) | This is enabled to interrupt CPU and notify application that all audio samples have been output. |


**Configuration Properties for using AGT**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Transfer Driver Stacks > g_timer0 Instance > Properties > Settings > Property > Common > Pin Output Support| Disabled| Enabled | Configures AGT pin for output |
| configuration.xml > Transfer Driver Stacks > g_timer0 Instance > Properties > Settings > Property > Module g_timer0 Timer Driver on r_agt > Output > AGTOA Output| Disabled| Start Level High | Configures AGTOA for output |
| configuration.xml > Transfer Driver Stacks > g_timer0 Instance > Properties > Settings > Property > Module g_timer0 Timer Driver on r_agt > Interrupt > Underflow Interrupt Priority| Disabled| Priority 12 |Priority set for optimum functioning of MCU |

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|RM_ADPCM_DECODER_Open|This API is used to initializes ADPCM audio decoder device |
|RM_ADPCM_DECODER_Close|This API is used to close the ADPCM decoder device |
|RM_ADPCM_DECODER_Decode|This API is used to decode 4bit ADPCM data to 16bit PCM data |
|RM_AUDIO_PLAYBACK_PWM_Open|This API is used to open and configure the Audio Playback with PWM driver  |
|RM_AUDIO_PLAYBACK_PWM_Close|This API is used to close the module driver  |
|RM_AUDIO_PLAYBACK_PWM_Start|This API starts the  audio playback PWM module |
|RM_AUDIO_PLAYBACK_PWM_Play|This API is used to play a single audio buffer by input samples   |
|RM_AUDIO_PLAYBACK_PWM_Stop|This API is used to stop the audio playback PWM module  |


* Callback:  
g_rm_audio_playback_callback function is called to identify the audio playback events and perform operation as per the received events, Also there are only one events available in the EP, they are used to signal when the audio play is complete.
1. When event AUDIO_PLAYBACK_EVENT_PLAYBACK_COMPLETE is received the audio playback application sets the playback started and completed flag


## Verifying operation ##
1. Import, generate and build the project EP.
2. Flash Audio Playback binary to the RA MCU.
3. Open RTT viewer and perform operation as per MENU Option.
4. Hear the audio playback sound using the wired headset after selecting play audio from MENU option.


1. Below images showcases the output on JLinkRTT_Viewer for Audio Playback:
  
  Below image shows menu options
 ![audio_rtt_log](images/AUDIO_RTT_log1.jpg "RTT output")
  
  Below image shows when start and stop is selected
 ![audio_rtt_log](images/AUDIO_RTT_log2.jpg "RTT output")
 
## Special Topics ##
### Input Data ###
1. Get a 16Khz wav file.
2. Encode the wav file into ADPCM data using [online wav to ADPCM converter tool](https://convertio.co).
3. With #2 step encoded ADPCM binary data will be generated.
4. This binary data can be converted into hex data format(.txt file) using [online bin-to-hex converter tool](http://tomeko.net/online_tools/file_to_hex.php?lang=en).
5. For the example project the converted ADPCM data from #4 is stored in the audio_playback_pwm_ep.c file.

