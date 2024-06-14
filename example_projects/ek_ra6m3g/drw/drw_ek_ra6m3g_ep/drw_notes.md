# Introduction #

This Example Project demonstrates the basic functionalities of 2D Drawing Engine (DRW) device driver on Renesas RA MCUs based on Renesas FSP. On successful initialization
of DRW and GLCDC modules, the EP will draw various shapes and shape fills across the LCD screen, as well as use Alpha Blending on the images rendered
to cover the entire screen of an external LCD connected to the RA MCU. API Failure messages will also be displayed on RTT Viewer.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the DRW example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit: EK-RA6M3G
* Micro USB cable
* PC with Windows 10 

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
* Renesas Flexible Software Package (FSP)
* e2 studio: Version 2024-04
* SEGGER J-Link RTT Viewer: Version 7.96a
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![drw](images/DRW_HLD.jpg "DRW Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| r_drw | This module is a port of D/AVE 2D.| DRW |
| r_glcdc | With GLCDC driver, user can reposition, alpha blend, color correct, dither and convert to and from a wide variety of pixel formats.| GLCD |

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> BSP > Property > Heap Size(bytes)  |  0   | 0x2000  |  Heap size is required for DRW library functions to be used. |
|   configuration.xml -> BSP > Property > Heap Size(bytes)  |  0   | 0x2000  |  Heap size is required for DRW library functions to be used. |
|   configuration.xml -> D/AVE 2D Port Interface(r_drw) > Property > Common > Allow Indirect mode  |  Enabled   | Enabled  | Mode selected for functioning of d2 driver functions |

Please refer GLCDC EP notes for GLCDC stack related properties.

## API Usage ##

The table below lists the GLCDC and DRW APIs used at the application layer in this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_GLCDC_Open| Open GLCDC module. |
|R_GLCDC_Start| Start GLCDC module. |
|R_GLCDC_BufferChange| Changes the framebuffer pointer |
|R_GLCDC_Stop| Stop GLCDC module |
|R_GLCDC_Close| Close GLCDC module |
|d2_opendevice| Creates a new device handle |
|d2_inithw| Initialize D/AVE2D module. |
|d2_flushframe| Wait for current rendering to end.|
|d2_framebuffer| Defines framebuffer's memory and layout |
|d2_setalpha| Set alpha value |
|d2_setalphamode| Choose alpha source|
|d2_setblendmode| Choose blendmode for RGB|
|d2_clear| Clear the framebuffer|
|d2_setantialiasing| Globally disable or enable antialiasing.|
|d2_setlinecap| Specify line end style.|
|d2_setlinejoin| Specify polyline connection style|
|d2_selectrendermode| Set render mode |
|d2_setcolor| Set desired color registers |
|d2_rendercircle| Render a circle or circle ring. |
|d2_rendertri| Render a triangle |
|d2_renderbox| Render a rectangle. |
|d2_renderline| Render a wide line. |
|d2_endframe| Mark the end of a frame. |
|d2_startframe|Beginning of a frame.|
|d2_deinithw| Unlink hardware currently bound by specified device|
|d2_closedevice| Close/Destroy the device handler |
|d2_geterrorstring| Returns string with human readable error description|


## Verifying operation ##
Import, Build and Debug the EP(see section Starting Development of [FSP User Manual](https://renesas.github.io/fsp/)). After running the EP, open the RTT viewer to view status or
check any error messages.
User can view the rendered images and animation on the LCD screen.
The animating output can be seen in LCD screen.
Refer the below steps for hardware connections :
* Connect RA MCU debug port to the host PC via a micro USB cable
* Connect LCD to the RA board

The below image showcase the output on J-Link RTT Viewer:
![drw_log_](images/RTT_LOG.jpg "RTT_LOG")

The animating shapes can be captured on Graphical LCD and memory monitor of e2studio when EP is paused as shown below respectively:

![drw_lcd_image](images/LCD_image.jpg "DRW_LCD_IMAGE")


![monitor_image](images/mem_monitor_image.jpg "MONITOR_IMAGE")

## Special Topics ##
In this example project, the BLEN (backlight enable) pin, P603, is set to GPIO mode and is initially high in the Pin configuration.
However, using the P603 as a PWM output function (GPT7's GTIOCA) is recommended, to control the brightness of the LCD backlight and reduce power consumption.




