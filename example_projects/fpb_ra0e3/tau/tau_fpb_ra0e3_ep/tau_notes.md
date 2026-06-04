# Introduction #
This example project demonstrates the basic usage of TAU driver on Renesas RA MCUs based on Renesas FSP. It consists of 7 independent operations coordinated together to create 3 sequences. They include setting the period for LED on and off times, counting external events, and measuring low-level pulse width. The user will input the required parameters for each sequence to configure the corresponding independent channel operation function of TAU. The EP information and error messages will be printed to the Host PC.

**Note:** To display information, the user can select between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART) with J-Link OB VCOM. By default, the example project information is printed to the host PC via the Serial Terminal. To utilize SEGGER RTT instead of the Serial Terminal, please follow the instructions in "Special Topic" section.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the TAU example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.5.0
* e2 studio: Version 2026-04.2
* SEGGER J-Link RTT Viewer: Version 9.42
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for basic information on setting up and using the above software.

### Hardware ###
Supported RA boards: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1, FPB-RA0E3
* 1 x Renesas RA board.
* 1 x Type-C USB cable for programming and debugging.
* 5 x Jumper wires.
* 1 x Host PC.

### Hardware Connections ###
* Connect the RA board to the host PC via a Type-C USB cable.
* For FPB-RA0E1:     
  * To use J-Link OB VCOM for Serial Terminal (UART): 
    * The soldering bridge E4 must be closed.
    * Populate R10 with a resistor ranging from 0 to 150 ohms.
  * Pin connection:
    * LED1 P008 (J2:30) <--------> TI03 P112 (J2:20)
    * TO01 P110 (J2:19) <--------> TI00 P213 (J1:5)
    * TO00 P212 (J1:6)  &ensp;<--------> TI04 P100 (J2:24)
    * LED2 P009 (J2:29) <--------> TI05 P103 (J2:21)
    * TO06 P102 (J2:22) <--------> TI07 P101 (J2:23)

* For FPB-RA0E2:
  * To use J-Link OB VCOM for Serial Terminal (UART), the user must close E46, E47.
  * To use P103 and P102 for LED1 and LED2, the user must close E28, E29.
  * Pin connection:
    * LED1 P103 (J2:13) <--------> TI03 P112 (J2:5)
    * TO01 P110 (J2:3)  &ensp;<--------> TI00 P213 (J1:9)
    * TO00 P212 (J1:10) <--------> TI04 P408 (J1:15)
    * LED2 P102 (J2:14) <--------> TI05 P201 (J1:26)
    * TO06 P301 (J1:31) <--------> TI07 P111 (J2:4)

* For FPB-RA0L1:
  * To use J-Link OB VCOM for Serial Terminal (UART), the user must close E28, E29.
  * To use the on-board debug functionality, the user must place jumper J9 on pins 1-2.
  * To use P002 and P104 for LED1 and LED2, the user must close E20, E18.
  * Pin connection:
    * LED1 P002 (J2:22) <--------> TI03 P500 (J2:13)
    * TO01 P110 (J2:3) &ensp;<--------> TI00 P213 (J1:7)
    * TO00 P212 (J1:8) &ensp;<--------> TI04 P408 (J1:11)
    * LED2 P104 (J2:8) &ensp;<--------> TI05 P201 (J1:20)
    * TO06 P301 (J1:23) <--------> TI07 P111 (J2:4)

* For FPB-RA0E3:
  * To use the on-board debug functionality, the user must turn OFF S3.
  * To use P010 and P011 for LED1 and LED2, the user must close E48, E49.
  * Pin connection:
    * LED1 P010 (J1:1) &ensp;<--------> TI03 P112 (J2:4)
    * TO01 P110 (J2:3) &ensp;<--------> TI00 P213 (J1:4)
    * TO00 P212 (J1:5) &ensp;<--------> TI04 P100 (J2:7)
    * LED2 P011 (J2:10) <--------> TI05 P201 (J1:8)
    * TO06 P102 (J2:5) &ensp;<--------> TI07 P101 (J2:6)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
High level block diagram of the system is shown below:
 
![tau](images/TAU_High_Level_Design.png "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| TAU | Operating 16-bit timer with 7 modes: Interval Timer, Delay Counter, Square Wave Output, Divider, Input Pulse Interval Measurement, External Event Counter, Low Level Pulse Width Measurement. | r_tau |
| I/O Port | I/O Port is used to control LEDs onboard. | r_ioport|

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different from those selected by default. 

**Configuration Properties for using TAU**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Common > Pin Output Support | Disabled | Enabled | Enable output for both square wave and divider output. |
| configuration.xml > Stacks > g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Common > Pin Input Support | Disabled | Enabled | Enable input for pulse width measurement, level width measurement, event counting or divider functions. |
| configuration.xml > Stacks > g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Function | Interval Timer | Interval Timer | The timer array unit can be used as a reference timer to generate TAU0_TMI0n (timer interrupt) at fixed intervals. |
| configuration.xml > Stacks > g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Operation Clock | CK00 | CK00 | Specify the selection of operation clock. |
| configuration.xml > Stacks > g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Callback | NULL | tau_interval_timer_callback | A user callback function. If this callback function is provided, it is called from the interrupt service routine (ISR) each time the timer period. |
| configuration.xml > Stacks > g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_interval_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Interrupt Priority | Disable | Priority 2 | Timer interrupt priority. |
| configuration.xml > Stacks > g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Function | Interval Timer | Delay Counter | It is possible to start counting down when the valid edge of the TI0n pin input is detected (an external event), and then generate TAU0_TMI0n (a timer interrupt) after any specified interval. |
| configuration.xml > Stacks > g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Operation Clock | CK00 | CK00 | Specify the selection of operation clock. |
| configuration.xml > Stacks > g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Input Source | No input signal | Input signal of TImn Pin | Select input source from TImn Pin. |
| configuration.xml > Stacks > g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Trigger Edge | Trigger Edge Rising | Trigger Edge Rising | Select the trigger edge. |
| configuration.xml > Stacks > g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Callback | NULL | tau_delay_timer_callback | A user callback function. If this callback function is provided, it is called from the interrupt service routine (ISR) each time the timer period. |
| configuration.xml > Stacks > g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_delay_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Interrupt Priority | Disable | Priority 2 | Timer interrupt priority. |
| configuration.xml > Stacks > g_square_wave_output_sequence2 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_square_wave_output_sequence2 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Function | Interval Timer | Square Wave Output | This function can use TO0n to perform a toggle operation as soon as TAU0_TMI0n has been generated, and outputs a square wave with a duty factor of 50%. |
| configuration.xml > Stacks > g_square_wave_output_sequence2 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_square_wave_output_sequence2 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Operation Clock | CK00 | CK01 | Specify the selection of operation clock. |
| configuration.xml > Stacks > g_square_wave_output_sequence2 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_square_wave_output_sequence2 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Output > Initial Output | Disable | Start Level Low | Set the initial output level. |
| configuration.xml > Stacks > g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Function | Interval Timer | Divider | The timer array unit can be used as a frequency divider that divides a clock input to the TI0n pin and outputs the result from the TO0n pin. |
| configuration.xml > Stacks > g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Operation Clock | CK00 | CK01 | Specify the selection of operation clock. |
| configuration.xml > Stacks > g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Input Source | No input signal | Input signal of TImn Pin | Select input source from TImn Pin. |
| configuration.xml > Stacks > g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Trigger Edge | Trigger Edge Rising | Trigger Edge Both | Select the trigger edge. |
| configuration.xml > Stacks > g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_divider_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Output > Initial Output | Disable | Start Level Low | Set the initial output level. |
| configuration.xml > Stacks > g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Function | Interval Timer | Input Pulse Interval Measurement | The count value can be captured at the TI0n valid edge and the interval of the pulse input to TI0n can be measured. |
| configuration.xml > Stacks > g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property >Module g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Operation Clock | CK00 | CK01 | Specify the selection of operation clock. |
| configuration.xml > Stacks > g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property >Module g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Input Source | No input signal | Input signal of TImn Pin | Select input source from TImn Pin. |
| configuration.xml > Stacks > g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property >Module g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Trigger Edge | Trigger Edge Rising | Trigger Edge Rising | Select the trigger edge. |
| configuration.xml > Stacks > g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property >Module g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Callback | NULL | tau_input_capture_timer_callback | A user callback function. If this callback function is provided, it is called from the interrupt service routine (ISR) each time the timer period. |
| configuration.xml > Stacks > g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property >Module g_input_capture Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Interrupt Priority | Disable | Priority 2 | Timer interrupt priority. |
| configuration.xml > Stacks > g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Function | Interval Timer | External Event Counter | The timer array unit can be used as an external event counter that counts the number of times the valid input edge (external event) is detected in the TI0n pin. When a specified count value is reached, the event counter generates an interrupt. |
| configuration.xml > Stacks > g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Operation Clock | CK00 | CK01 | Specify the selection of operation clock. |
| configuration.xml > Stacks > g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Period | 0x10000 | 5 | Specify the timer period based on the selected units. |
| configuration.xml > Stacks > g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Period Unit | Raw Counts | Raw Counts | Unit of the period. |
| configuration.xml > Stacks > g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Input Source | No input signal | Input signal of TImn Pin | Select input source from TImn Pin. |
| configuration.xml > Stacks > g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Trigger Edge | Trigger Edge Rising | Trigger Edge Rising | Select the trigger edge. |
| configuration.xml > Stacks > g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Callback | NULL | tau_input_capture_timer_callback | A user callback function. If this callback function is provided, it is called from the interrupt service routine (ISR) each time the timer period. |
| configuration.xml > Stacks > g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_external_event_counter_timer Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Interrupt Priority | Disable | Priority 2 | Timer interrupt priority. |
| configuration.xml > Stacks > g_square_wave_output_sequence3 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_square_wave_output_sequence3 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Function | Interval Timer | Square Wave Output | This function can use TO0n to perform a toggle operation as soon as TAU0_TMI0n has been generated, and outputs a square wave with a duty factor of 50%. |
| configuration.xml > Stacks > g_square_wave_output_sequence3 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_square_wave_output_sequence3 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Operation Clock | CK00 | CK01 | Specify the selection of operation clock. |
| configuration.xml > Stacks > g_square_wave_output_sequence3 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_square_wave_output_sequence3 Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Output > Initial Output | Disable | Start Level Low | Set the initial output level. |
| configuration.xml > Stacks > g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Function | Interval Timer | Measure Low Level Pulse Width | By starting counting at one edge of the TI0n pin input and capturing the number of counts at another edge, the low-level pulse width of TI0n can be measured. |
| configuration.xml > Stacks > g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > General > Operation Clock | CK00 | CK01 | Specify the selection of operation clock. |
| configuration.xml > Stacks > g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Input Source | No input signal | Input signal of TImn Pin | Select input source from TImn Pin. |
| configuration.xml > Stacks > g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Input > Trigger Edge | Trigger Edge Rising | Trigger Edge Both | Select the trigger edge. |
| configuration.xml > Stacks > g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Callback | NULL | tau_measure_low_level_pulse_width_timer_callback | A user callback function. If this callback function is provided, it is called from the interrupt service routine (ISR) each time the timer period. |
| configuration.xml > Stacks > g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Properties > Settings > Property > Module g_measure_low_level_pulse_width Timer, Independent Channel, 16-bit and 8-bit Timer Operation (r_tau) > Interrupts > Interrupt Priority | Disable | Priority 2 | Timer interrupt priority. |

## API Usage ##
The table below lists the FSP provided API used at the application layer in this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_TAU_Open | This API is used to initialize the timer module and applies configurations. |
| R_TAU_Start| This API is used to start timer. |
| R_TAU_Enable | This API is used to enable external event triggers that start, stop, clear, or capture the counter. |
| R_TAU_Reset | This API is used to reset the counter value to the period minus one. |
| R_TAU_PeriodSet | This API is used to set period value provided. |
| R_TAU_Close | This API is used to stop counter, disables output pins, and clears internal driver data. |
| R_IOPORT_PinWrite | This API is used to control LED state. |
| R_BSP_SoftwareDelay | This API is used to delay a specified period of time. |

## Verifying Operation ##
1. Import, generate and build the example project.
2. Before running the example project, make sure hardware connections are completed.
3. Flash or debug the example project.
4. Open J-Link RTT Viewer to see the output.

For using the SEGGER J-Link RTT Viewer:  

  ![rtt_log_1](images/tau_rtt_log1.png "Interval timer and delay counter operation")

  ![rtt_log_2](images/tau_rtt_log2.png "External event counter operation")

  ![rtt_log_3](images/tau_rtt_log3.png "Low level pulse width measurement operation")

## Special Topic ##
**Special Notes:**
* FPB-RA0E3 limitations:
    * Due to the limited RAM (2 KB), code flash (16 KB), and TAU‑timer and OB‑VCOM pin conflict, the Serial Terminal is not supported on FPB-RA0E3.
    * Enabling printing floats to the RTT Viewer is not required and should be disabled to avoid RAM overflow.
* Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer; if supported, it defaults to the serial terminal (UART).
  * To configure display options:
    * **For UART via J-Link OB VCOM**: Define `USE_VIRTUAL_COM=1`
    * **For J-Link RTT Viewer**: Define `USE_VIRTUAL_COM=0`
  * It is important to note that the user should only operate a single terminal application (SEGGER J-Link RTT Viewer or a serial terminal (UART)) at a time to avoid conflicts or data inconsistencies. 
  * The settings for the serial terminal are as follows: The COM port is provided by the J-Link onboard, with a baud rate of 115200 bps, a data length of 8 bits, no parity check, one stop bit, and no flow control.
  * To echo back what was typed in Tera Term, the user needs to enable it through [Setup] -> [Terminal...] -> Check [Local echo].
  * The board supports J-Link OB VCOM: FPB-RA0E1, FPB-RA0E2, FPB-RA0L1.
    
  Set this in **Project Properties** -> **C/C++ Build** -> **Settings** -> **Tool Settings** -> **GNU ARM Cross C Compiler** -> **Preprocessor**.
