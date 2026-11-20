# Introduction #
The example project demonstrates the basic functionalities of the 2D Drawing Engine (DRW) device driver on Renesas RA MCUs based on Renesas FSP. On successful initialization of the DRW and GLCDC modules, the EP draws and fills various shapes with animation and alpha blending effects on an external LCD connected to the RA MCU. Additionally, the EP information and any failure messages will be displayed on the terminal.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the DRW example project, the following resources are needed:

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.6.0
* e2 studio: Version 2026-07
* LLVM Embedded Toolchain for ARM: Version 22.1.0
* Terminal Console Application: Tera Term or a similar application

Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware ###
* Supported RA boards: EK-RA6M3G, EK-RA8D1, EK-RA8E2, EK-RA8P1, EK-RA8D2.
	* 1 x Renesas RA Board.
	* 1 x Graphics Expansion Board:
		* For EK-RA6M3G: Graphics Expansion Board (P/N: RTK7EK6M3B00001BU).
		* For EK-RA8D1: MIPI Graphics Expansion Board.
		* For EK-RA8E2: Graphics Expansion Board (P/N: RTKAPPLCDPS02001BE).
		* For EK-RA8P1, EK-RA8D2: Graphics Expansion Board (P/N: RTKLCDPAR1S00001BE).
	* 1 x USB cable for programming and debugging (USB cable type varies by board model).

### Hardware Connections ###
* Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and debugging.

* For EK-RA6M3G:
	* Connect Graphics Expansion Board to the RA board.

* For EK-RA8D1:
	* Connect the MIPI Graphics Expansion Board to J58 of the RA board.
	* Set the configuration switches (SW1) on the EK-RA8D1 board as below:

		|   SW1-1 PMOD1   |   SW1-2 TRACE   |   SW1-3 CAMERA  |   SW1-4 ETHA   |   SW1-5 ETHB   |   SW1-6 GLCD   |   SW1-7 SDRAM   |   SW1-8 I3C   |
		| :-------------: | :-------------: | :-------------: | :------------: | :------------: | :------------: | :-------------: | :-----------: |
		|       OFF       |        OFF      |        OFF      |       OFF      |       OFF      |       OFF      |       ON        |       OFF     |

* For EK-RA8E2:
	* Connect J1 on Graphic Expansion Board to J1 of the RA board. Please carefully align pin 2 on the Graphic Expansion Board with pin 1 on J1 of the RA board.

* For EK-RA8P1, EK-RA8D2:
	* Connect J1 on Graphics Expansion Board to J1 of the RA board. Please carefully align pin 1 on the Graphic Expansion Board with pin 1 on J1 of the RA board.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
* [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
* [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![drw_hld](images/drw_hld.jpg "DRW Block Diagram")

For EK-RA8D1:

![drw_hld_ek_ra8d1](images/drw_hld_ek_ra8d1.jpg "DRW Block Diagram for EK-RA8D1")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| D/AVE 2D Port Interface (r_drw) | This module is a port of D/AVE 2D. | r_drw |
| Graphics LCD (r_glcdc) | With GLCDC driver, the user can reposition, alpha blend, color correct, dither and convert to and from a wide variety of pixel formats. | r_glcdc |

**For EK-RA8D1:**
| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| D/AVE 2D Port Interface (r_drw) | This module is a port of D/AVE 2D. | r_drw |
| Graphics LCD (r_glcdc) | With GLCDC driver, the user can reposition, alpha blend, color correct, dither and convert to and from a wide variety of pixel formats. | r_glcdc |
| MIPI Display (r_mipi_dsi)| MIPI DSI is used to get data from GLCDC and transmit it to MIPI PHY. | r_mipi_dsi |
| MIPI Physical Layer (r_mipi_phy)| MIPI PHY is used to send data to MIPI LCD. | r_mipi_phy |

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x1000 | Main stack size is required for DRW library functions to be used. |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x1500 | Heap size is required for DRW library functions to be used. |
| configuration.xml > Stacks > g_display Graphics LCD (r_glcdc) > Properties > Setting > Property > Module g_display Graphics LCD (r_glcdc) > Interrupts > Callback Function | NULL | glcdc_callback | Define user callback function. |

Please refer GLCDC EP notes for GLCDC stack related properties.

**For EK-RA8D1:**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > g_display Graphics LCD (r_glcdc) > Properties > Setting > Property > Module g_display Graphics LCD (r_glcdc) > Input > Graphics Layer 1 > General > Color format | RGB56 (16-bit) | RGB888 (32-bit) | To use 32 bits Color format input. |
| configuration.xml > Stacks > g_display Graphics LCD (r_glcdc) > Properties > Setting > Property > Module g_display Graphics LCD (r_glcdc) > Output > Format > Color format | 24bits RGB888 | 24bits RGB888 | To use 32bits Color format. |
| configuration.xml > Stacks > g_mipi_dsi0 MIPI Display (r_mipi_dsi) > Properties > Setting > Property > Module g_mipi_dsi0 MIPI Display (r_mipi_dsi) > Interrupts > Callback | mipi_dsi0_callback | mipi_dsi_callback | Define user callback function. |

**For EK-RA8E2, EK-RA8P1, EK-RA8D2:**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > g_display Graphics LCD (r_glcdc) > Properties > Setting > Property > Module g_display Graphics LCD (r_glcdc) > Input > Graphics Layer 1 > General > Color format | RGB56 (16-bit) | RGB888 (32-bit) | To use 32 bits Color format input. |
| configuration.xml > Stacks > g_display Graphics LCD (r_glcdc) > Properties > Setting > Property > Module g_display Graphics LCD (r_glcdc) > Output > Format > Color format | 24bits RGB888 | 24bits RGB888 | To use 32bits Color format. |

## API Usage ##

The table below lists the GLCDC and DRW APIs used at the application layer in this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_GLCDC_Open | This API is used to open GLCDC module. |
|R_GLCDC_Start | This API is used to start GLCDC module. |
|R_GLCDC_BufferChange | This API is used to change the frame buffer pointer. |
|R_GLCDC_Stop | This API is used to stop GLCDC module. |
|R_GLCDC_Close | This API is used to close GLCDC module. |
|d2_opendevice | This API is used to create a new device handle. |
|d2_inithw | This API is used to initialize D/AVE2D module. |
|d2_flushframe | This API is used to wait for current rendering to end.|
|d2_framebuffer | This API is used to define frame buffer's memory and layout. |
|d2_setalpha | This API is used to set alpha value. |
|d2_setalphamode | This API is used to choose alpha source.|
|d2_setblendmode | This API is used to choose blend mode for RGB.|
|d2_clear | This API is used to clear the frame buffer. |
|d2_setantialiasing | This API is used to globally disable or enable antialiasing. |
|d2_setlinecap | This API is used to specify line end style. |
|d2_setlinejoin | This API is used to specify polyline connection style. |
|d2_selectrendermode | This API is used to set render mode. |
|d2_setcolor | This API is used to set desired color registers. |
|d2_rendercircle | This API is used to render a circle or circle ring. |
|d2_rendertri | This API is used to render a triangle. |
|d2_renderbox | This API is used to render a rectangle. |
|d2_renderline | This API is used to render a wide line. |
|d2_endframe | This API is used to mark the end of a frame. |
|d2_startframe | This API is used to begin of a frame. |
|d2_deinithw | This API is used to unlink hardware currently bound by specified device. |
|d2_closedevice | This API is used to close/destroy the device handler. |
|d2_geterrorstring | This API is used to return string with a human-readable error description |

**For EK-RA8D1:** MIPI API additional used at the application layer in this example project.
| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_MIPI_DSI_Command| This API is used to send a command to the peripheral device. |

## Verifying operation ##
* Import, build, and debug the EP (see section Starting Development of [FSP User Manual](https://renesas.github.io/fsp/)). Before running the EP, make sure the hardware connections are set up properly as described in the [Hardware Connection](#hardware-connections) section.
* After running the EP, open a serial terminal application (e.g., Tera Term) to view output log, configure the settings as shown below:
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
* The user can view the rendered images and animations on the LCD screen.

The EP information:

![ep_information](images/ep_information.jpg "EP Information")

Stop rendering and disable the GLCDC module:

![disable_glcdc](images/disable_glcdc.jpg "Disable GLCDC")

The rendered animated shapes can be observed on the graphical LCD and in the e2studio Memory Monitor when the EP is paused, as shown below:

![drw_lcd_image](images/lcd_image.jpg "LCD Image")

![memory_image](images/memory_image.jpg "Memory Image")

To view the "Raw Image" in memory monitor of the e2studio IDE using the address of **g_framebuffer** please follow step as below:  
* On Debug the project EP by e2studio IDE, open memory view.

![open_memory_view](images/memory_view_step1.jpg "Open Memory View")

* Add image buffer under memory monitor.

![add_buffer](images/memory_view_step2.jpg "Add Buffer")

* Open Raw Image Rendering 

![raw_image_rendering](images/memory_view_step3.jpg "Raw Image Rendering")
* Set Raw Image Format:
	* Width: 1024
	* Height: 600
	* Encoding to RGB: 32bpp (8:8:8:8)
	* Line alignment: 4 bytes
	* Start Position: Top

![glcdc_resolution](images/memory_view_step4.jpg "GLCDC Resolution")

## Special Topics ##
In this example project, the BLEN (backlight enable) pin is set to GPIO mode and is initially high in the Pin configuration.
However, using the BLEN pin as a PWM output function (GPT's GTIOCA or GTIOCB) is recommended, to control the brightness of the LCD backlight and reduce power consumption.
| Board            |BLEN pin         |
|------------------|-----------------|
| EK-RA6M3G        | P603            |
| EK-RA8D1         | P404            |
| EK-RA8E2         | P404            |
| EK-RA8P1         | P514            |
| EK-RA8D2         | P514            |
