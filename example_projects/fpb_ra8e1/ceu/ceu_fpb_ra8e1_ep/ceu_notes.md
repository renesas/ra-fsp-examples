# Introduction #
This example project demonstrates the basic functionality of CEU driver on Renesas RA MCUs based on FSP. Upon successful initialization of the CEU and camera setup:
* **For EK-RA8D1, EK-RA8P1, EK-RA8D2:**
  * The user is requested to select memory (1. SDRAM and 2. SRAM) to save image.
  * Press '1' to select 'SDRAM' to choose SXGA (1280 x 960) format and store the image in SDRAM.
  * Press '2' to select 'SRAM' to choose VGA (640 x 480) format and store the image in SRAM.
  * By default, the example project configures the camera in test pattern mode, and the image captured is a color bar. The user can get the actual image by modifying the macro CAM_TEST_PATTERN in the camera_sensor.h file with a value of 0U.
* **For FPB-RA8E1:**
  * The user can press any key to capture and store a QVGA (320 x 240) format image in SRAM.
  * By default, the example project configures in actual image test mode. Set CAM_TEST_PATTERN to 1U to enable the test pattern mode.

The captured image can be viewed in e2studio using the 'memory viewer'.

**Note:** This example project does not perform color correction for the camera sensor.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the CEU example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.5.0
* e2 studio: Version 2026-04.2
* LLVM Embedded Toolchain for ARM: Version 21.1.1
* Terminal Console Application: Tera Term or a similar application

### Hardware ###
* Supported RA boards: EK-RA8D1, FPB-RA8E1, EK-RA8P1, EK-RA8D2.
  *	1 x Renesas RA board.
  *	1 x USB cable for programming and debugging (USB cable type varies by board model).
  * For EK-RA8D1, EK-RA8E1:
    * 1 x OV3640 camera module. (Part Number: ArduCam B0156 - https://www.arducam.com/product/b0156-1-4-3-mega-pixel-m12-mount-ov3640-camera-module-with-jpeg-output/).
  * For EK-RA8P1, EK-RA8D2:
    * 1 x OV5640 camera module. (Part Number: CU450_OV5640).

### Hardware Connections ###
* Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and debugging.

* For EK-RA8D1:  
  * Connect the camera module to the camera connector J59 on the RA board.
  * Set the configuration switches on the RA board as below:
      |     SW1     | | Status      |
      |-------------|-|-------------|
      |SW1-1 PMOD1	|-|     OFF     |
      |SW1-2 TRACE	|-|     OFF     |
      |SW1-3 CAMERA |-|     ON      |
      |SW1-4 ETHA   |-|     OFF     |  
      |SW1-5 ETHB   |-|     OFF     |  
      |SW1-6 GLCD   |-|     OFF     |  
      |SW1-7 SDRAM	|-|     ON      | 
      |SW1-8 I3C    |-|     OFF     | 
      
    ![connection_RA8D1_with_OV3640](images/connection_RA8D1_with_OV3640.png "Connection RA8D1 with OV3640")

* For FPB-RA8E1:
  * Connect the camera module to the camera connector J4 on the RA board.  
    ![connection_RA8E1_with_OV3640](images/connection_RA8E1_with_OV3640.png "Connection RA8E1 with OV3640")
  
* For EK-RA8D2, EK-RA8P1:
  * Connect the camera module to the Camera Expansion Port J35 on the RA board.
  * Open J41 to use P405, P406 for Camera function.
  * Set the configuration switches on the RA board as below:
      |     SW4         | | Status      |
      |-----------------|-|-------------|
      |SW4-1 PMOD1_SEL0	|-|     OFF     |
      |SW4-2 PMOD1_SEL1	|-|     OFF     |
      |SW4-3 OSPI       |-|     OFF     |
      |SW4-4 ARDUINO    |-|     OFF     |  
      |SW4-5 I3C        |-|     OFF     |  
      |SW4-6 MIPI       |-|     ON      |  
      |SW4-7 USBFS_ROLE	|-|     OFF     | 
      |SW4-8 USBHS_ROLE |-|     OFF     |
  * Detail connection: Connect the camera module to the Camera Expansion Port J35 on the back side of the EK-RA8D2 board using the Camera FFC cable.  
    ![connection_RA8D2_with_OV5640](images/connection_RA8D2_with_OV5640.png "Connection RA8D2 with OV5640")

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
* [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
* [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
 High level block diagram of the system is as shown below:
 
![ceu_high_level_design](images/ceu_high_level_design.png "High Level Block Diagram")

**Note:**  
**For FPB-RA8E1:** The EP menu does not contain multiple options because this board only supports for SRAM memory and the image resolution is QVGA (320 x 240).

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-------|---------------------|
| CEU | CEU is used to capture images from the camera. | r_ceu |
| I2C Master | I2C Master is used to configure the camera. | r_iic_master |
| GPT | GPT is used to provide the clock for the camera. | r_gpt |
| IO Port | IO Port is used to set power mode for the camera. | r_ioport |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different from those selected by default.

**Configuration Properties for using g_ceu_vga instance**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_ceu_vga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_vga Capture Engine Unit (r_ceu) > Input > Capture Specifications > Horizontal capture resolution | 640 | 640 | Width of image resolution VGA. |
| configuration.xml > Stacks > g_ceu_vga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_vga Capture Engine Unit (r_ceu) > Input > Capture Specifications > Vertical capture resolution | 480 | 480 | Height of image resolution VGA. |
| configuration.xml > Stacks > g_ceu_vga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_vga Capture Engine Unit (r_ceu) > Output > Byte Swapping > Swap 16-bit units | ☐ | ☑ | The image data is different in endian between the camera and the MCU. |
| configuration.xml > Stacks > g_ceu_vga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_vga Capture Engine Unit (r_ceu) > Output > Byte Swapping > Swap 32-bit units | ☐ | ☑ | The image data is different in endian between the camera and the MCU. |
| configuration.xml > Stacks > g_ceu_vga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_vga Capture Engine Unit (r_ceu) > Interrupts > Callback | g_ceu0_user_callback | ceu_vga_callback | It is called from the interrupt service routine (ISR) each time the CEU capture operation completes. |
| configuration.xml > Stacks > g_ceu_vga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_vga Capture Engine Unit (r_ceu) > Interrupts > CEU Interrupt Priority | Priority 12 | Priority 12 | Select the CEU interrupt priority. |

**Configuration Properties for using g_ceu_sxga instance**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_ceu_sxga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_sxga Capture Engine Unit (r_ceu) > Input > Capture Specifications > Horizontal capture resolution | 640 | 1280 | Width of image resolution SXGA. |
| configuration.xml > Stacks > g_ceu_sxga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_sxga Capture Engine Unit (r_ceu) > Input > Capture Specifications > Vertical capture resolution | 480 | 960 | Height of image resolution SXGA. |
| configuration.xml > Stacks > g_ceu_sxga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_sxga Capture Engine Unit (r_ceu) > Output > Byte Swapping > Swap 16-bit units | ☐ | ☑ | The image data is different in endian between the camera and the MCU. |
| configuration.xml > Stacks > g_ceu_sxga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_sxga Capture Engine Unit (r_ceu) > Output > Byte Swapping > Swap 32-bit units | ☐ | ☑ | The image data is different in endian between the camera and the MCU. |
| configuration.xml > Stacks > g_ceu_sxga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_sxga Capture Engine Unit (r_ceu) > Interrupts > Callback | g_ceu0_user_callback | ceu_sxga_callback | It is called from the interrupt service routine (ISR) each time the CEU capture operation completes. |
| configuration.xml > Stacks > g_ceu_sxga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_sxga Capture Engine Unit (r_ceu) > Interrupts > CEU Interrupt Priority | Priority 12 | Priority 12 | Select the CEU interrupt priority. |

**For FPB-RA8E1:**  
**Configuration Properties for using g_ceu_qvga instance**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_ceu_qvga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_qvga Capture Engine Unit (r_ceu) > Input > Capture Specifications > Horizontal capture resolution | 640 | 320 | Width of image resolution QVGA. |
| configuration.xml > Stacks > g_ceu_qvga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_qvga Capture Engine Unit (r_ceu) > Input > Capture Specifications > Vertical capture resolution | 480 | 240 | Height of image resolution QVGA. |
| configuration.xml > Stacks > g_ceu_qvga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_qvga Capture Engine Unit (r_ceu) > Output > Byte Swapping > Swap 8-bit units | ☐ | ☑ | The image data is different in endian between the camera and the MCU. |
| configuration.xml > Stacks > g_ceu_qvga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_qvga Capture Engine Unit (r_ceu) > Output > Byte Swapping > Swap 16-bit units | ☐ | ☑ | The image data is different in endian between the camera and the MCU. |
| configuration.xml > Stacks > g_ceu_qvga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_qvga Capture Engine Unit (r_ceu) > Output > Byte Swapping > Swap 32-bit units | ☐ | ☑ | The image data is different in endian between the camera and the MCU. |
| configuration.xml > Stacks > g_ceu_qvga Capture Engine Unit (r_ceu) > Properties > Settings > Property > Module g_ceu_qvga Capture Engine Unit (r_ceu) > Interrupts > Callback | g_ceu0_user_callback | ceu_qvga_callback | It is called from the interrupt service routine (ISR) each time the CEU capture operation completes. |
| configuration.xml > Stacks > g_ceu_qvga Capture Engine Unit (r_ceu) > Settings > Property > Module g_ceu_qvga Capture Engine Unit (r_ceu) > Interrupts > CEU Interrupt Priority | Priority 12 | Priority 12 | Select the CEU interrupt priority. |

**Configuration Properties for using I2C Master**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_i2c_master_for_cam I2C Master (r_iic_master) > Properties > Settings > Property > Module g_i2c_master_for_cam I2C Master (r_iic_master) > Channel | 0 | 1 | Use I2C Master Channel 1 to control the camera module. |
| configuration.xml > Stacks > g_i2c_master_for_cam I2C Master (r_iic_master) > Properties > Settings > Property > Module g_i2c_master_for_cam I2C Master (r_iic_master) > Rate | Standard | Fast-mode | Optimize communication speed with the camera. |
| configuration.xml > Stacks > g_i2c_master_for_cam I2C Master (r_iic_master) > Properties > Settings > Property > Module g_i2c_master_for_cam I2C Master (r_iic_master) > Slave Address | 0x00 | 0x3C | Specific slave address of the OV3640 camera. |
| configuration.xml > Stacks > g_i2c_master_for_cam I2C Master (r_iic_master) > Properties > Settings > Property > Module g_i2c_master_for_cam I2C Master (r_iic_master) > Address Mode | 7-Bit | 7-Bit | Specific address mode of the OV3640 camera. |
| configuration.xml > Stacks > g_i2c_master_for_cam I2C Master (r_iic_master) > Properties > Settings > Property > Module g_i2c_master_for_cam I2C Master (r_iic_master) > Callback | NULL | i2c_master_for_cam_callback | It is called from the interrupt service routine (ISR) upon IIC transaction completion reporting the transaction status. |
| configuration.xml > Stacks > g_i2c_master_for_cam I2C Master (r_iic_master) > Properties > Settings > Property > Module g_i2c_master_for_cam I2C Master (r_iic_master) > Interrupt Priority Level | Priority 12 | Priority 12 | Select the IIC master interrupt priority. |

**Configuration Properties for using GPT**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_timer_periodic Timer, General PWM (r_gpt) > Properties > Settings > Property > Common > Pin Output Support | Disabled | Enabled | Provide clock source for camera. |
| configuration.xml > Stacks > g_timer_periodic Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer_periodic Timer, General PWM (r_gpt) > General > Channel | 0 | 3 | Use GPT Channel 3 to generate the input clock for the camera module. |
| configuration.xml > Stacks > g_timer_periodic Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer_periodic Timer, General PWM (r_gpt) > General > Mode | Periodic | Periodic | Set the GPT in periodic mode to generate the clock source. |
| configuration.xml > Stacks > g_timer_periodic Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer_periodic Timer, General PWM (r_gpt) > General > Period | 0x10000 | 24000 | Set the GPT frequency to 24 Mhz, to be in specification for the camera's clock source [6 ~ 27MHz]. |
| configuration.xml > Stacks > g_timer_periodic Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer_periodic Timer, General PWM (r_gpt) > General > Period Unit | Raw Counts | Kilohertz | Set the GPT frequency to 24 MHz. |
| configuration.xml > Stacks > g_timer_periodic Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer_periodic Timer, General PWM (r_gpt) > Output > GTIOCA Output Enabled | False | True | Provide a 24 MHz clock frequency output pin. |

## API Usage ##
The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_CEU_Open | This API is used to initialize CEU module. |
| R_CEU_CaptureStart | This API is used to start a capture image. |
| R_CEU_Close | This API is used to stop and close CEU module. |
| R_GPT_Open | This API is used to initialize the GPT timer. |
| R_GPT_Start | This API is used to start GPT timer in periodic mode, when timer starts, it provides a 24 MHz clock frequency for the camera. |
| R_GPT_Close | This API is used to de-initialize GPT timer. |
| R_IIC_MASTER_Open | This API is used to initialize the I2C Master module. |
| R_IIC_MASTER_Write | This API is used to perform a write from the camera register. |
| R_IIC_MASTER_Read | This API is used to perform a read from the camera register. |
| R_IIC_MASTER_Close | This API is used to de-initialize the I2C Master module. |
| R_BSP_PinWrite | This API is used to set the output level for a PWR pin of the camera. |
| R_BSP_SoftwareDelay | This API is used to delay a specified period of time. |

## SDRAM Configuration ##
The table below describes the configuration properties for using SDRAM.

**For EK-RA8D1:**

  | Properties | Setting |
  |------------|---------|
  | Clock source | 120 MHz |
  | Bus width | 16-bit |
  | Endian mode | Little endian |
  | Auto-refresh | Enable |
  | Self-refresh | Disable |
  | Continuous access | Enable |

**For EK-RA8P1, EK-RA8D2:**

  | Properties | Setting |
  |------------|---------|
  | Clock source | 125 MHz |
  | Bus width | 32-bit |
  | Endian mode | Little endian |
  | Address Multiplex Shift | 9-bit shift |
  | Continuous access | Enable |

## Verifying Operation ##
1. Import, generate, and build the example project.
2. Before running the EP, make sure the hardware connections are set up properly as described in the [Hardware Connections](#hardware-connections) section.
3. Debug the EP.
4. On debug session.
* Open memory view.

  ![open_memory_view](images/memory_view_step1.png "Open Memory View")

* Add image buffer under memory monitor. **For FPB-RA8E1:** Enter address or expression to monitor: **&s_image_qvga_sram**.

  ![add_buffer](images/memory_view_step2.png "Add Buffer")

* Open Raw Image Rendering.

  ![raw_image_rendering](images/memory_view_step3.png "Raw Image Rendering")

* Set Raw Image Format:
  * For VGA resolution images:
    * Width: 320
    * Height: 240
    * Encoding: YCbCr 16bpp (4:2:2) (Cb,y0,Cr,y1)
    * Line alignment: 4 bytes
    * Start Position: Top

  ![qvga_resolution_setting](images/memory_view_step4.png "QVGA Resolution Setting")

  ![qvga_resolution_setting](images/memory_view_step5.png "QVGA Resolution Setting")

5. Connect to a serial terminal application (e.g., Tera Term) to view output log, configure the settings as shown below:
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

The images below show the EP operation:

* EP information:

  ![ep_information](images/ep_information.png "EP Information")

**Actual image test mode (Default):** Define CAM_TEST_PATTERN (0U) macro in camera_sensor.h.
* Serial log:

  ![serial_qvga_sram_log](images/serial_qvga_sram.png "Serial QVGA SRAM Log")

* QVGA image stored in SRAM:

  ![raw_image_qvga_sram](images/image_qvga_sram.png "Raw Image QVGA in SRAM")

**Pattern test mode:** Define CAM_TEST_PATTERN (1U) macro in camera_sensor.h.
* Serial log:

  ![serial_test_pattern_log](images/serial_test_pattern_color_bars.png "Serial Test Pattern Log")

* Camera test pattern image stored in SRAM:

  ![raw_image_test_pattern_color_bars](images/image_test_pattern_color_bars.png "Raw Image Test Pattern Color Bars")

## Special Topics ##
* The files camera_sensor.c and camera_sensor.h contain the API for initialization and configuration of both OV3640 and OV5640 camera modules.
* The camera sensor is configured in YUV422 output format, and the image resolution is QVGA, SXGA, or VGA.
* g_ceu_qvga is the CEU instance configured in Data Synchronous Fetch mode at QVGA resolution.
* g_ceu_sxga is the CEU instance configured in Data Synchronous Fetch mode at SXGA resolution.
* g_ceu_vga is the CEU instance configured in Data Synchronous Fetch mode at VGA resolution.
* CAM_TEST_PATTERN in camera_sensor.h is used to enable/disable using test pattern of camera sensor. It is enabled by default and a matching result between captured data and test pattern will be shown in the terminal for this case. Disable it to capture live data from the camera.
