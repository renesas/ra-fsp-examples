# Introduction #

This Example Project demonstrates the basic functionality of USB PCDC driver on Renesas RA MCUs based on Renesas FSP using FreeRTOS. In this example,
user will be able to use the RA board as a CDC to UART converter by connecting through a COM port from from the host PC.
The converter performs the data reception and data transmission asynchronously. The user can connect any UART device to receive the data typed in Tera Term,
and to send any data to the converter to display on Tera Term. The user can also change the baud rate, stop bit and parity bit configuration at run time
through Tera Term. The status of the application will be displayed on JLink RTT Viewer.
         
Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the usb example project, the following resources are needed.

### Hardware ###
* Renesas RA board: Renesas RA boards
* 2 x Micro USB cable
* Host PC with at least 2 USB ports
* Connecting/Jumper wires for shorting pins

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
* Renesas Flexible Software Package (FSP): Version 5.7.0
* e2 studio: Version 2024-10
* SEGGER J-Link RTT Viewer: Version 8.10f
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
* Terminal Console Application: Tera Term or a similar application

Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![USB PCDC with FreeRTOS](images/usb_pcdc_freeRTOS.jpg "USB PCDC with FreeRTOS Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| USB PCDC | USB Peripheral Communications Device Class (PCDC) driver transfers data to and from USB host, responds to CDC class requests.| pcdc |
| UART | SCI_UART module supports data reception and transmission, bitrate modulation, baud rate change , and flow control. | uart |

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

**Common Configuration Properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml > BSP > Property > Heap Size(bytes)  |  0   | 0x400  |  Change heap size to accomodate functions usage in the EP and avoid any runtime errors |
|   configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Common  >  DMA Support | Disabled | Enabled | Enable DMA support for the module |

**Configuration Properties if USB Speed as High Speed**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Common > DMA Source Address| DMA Disabled | HS Address | USB basic driver is configured **USB Speed** as **Hi Speed**. Accordingly, DMA Source Address should provide with **HS Address**.|
| configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Common > DMA Destination Address| DMA Disabled | HS Address | USB basic driver is configured **USB speed** as **Hi Speed**. Accordingly, DMA Destination Address should provided with **HS Address**.|
| configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Module g_basic0 USB Driver on r_usb_basic > USB Speed| Full Speed | Hi Speed | USB Speed is configured as **Hi Speed**.|
| configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Module g_basic0 USB Driver on r_usb_basic > USB Module Number| USB_IP0 Port | USB_IP1 Port | This property is used to specify USB module number to be used as per configured USB speed.|
| configuration.xml > g_transfer0 Transfer (r_dmac) > Settings > Property > Module g_transfer0 Transfer driver > Transfer Size| 2 | 4 |The transfer size of DMA for **High Speed** requires **4 Bytes**.|
| configuration.xml > g_transfer0 Transfer (r_dmac) > Settings > Property > Module g_transfer0 Transfer driver > Activation Source| No ELC Trigger | USBHS FIFO 1 (DMA Transfer request 1)  |This is an event trigger for DMA transfer 0 instance for destination pointer address. |
| configuration.xml > g_transfer1 Transfer (r_dmac) > Settings > Property > Module g_transfer1 Transfer driver > Transfer Size| 2 | 4 |The transfer size of DMA for **High Speed** requires **4 Bytes**.
| configuration.xml > g_transfer1 Transfer (r_dmac) > Settings > Property > Module g_transfer1 Transfer driver > Activation Source| No ELC Trigger | USBHS FIFO 0 (DMA Transfer request 0)  |This is an event trigger for DMA transfer 1 instance for source pointer address.|

**Configuration Properties if USB Speed as Full Speed.**

Below settings should be adjusted after resetting all modules values to the default values.

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Common >DMA Source Address| DMA Disabled | FS Address | USB basic driver is configured **USB Speed** as **Full Speed**. Accordingly, DMA Source Address should provide with **FS Address**.|
| configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Common >DMA Destination Address| DMA Disabled | FS Address | USB basic driver is configured **USB speed** as **Full Speed**. Accordingly, DMA Destination Address should provided with **FS Address**.|
| configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Module g_basic0 USB Driver on r_usb_basic > USB Speed| Full Speed | Full Speed | USB Speed is configured as **Full Speed**.|
| configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Module g_basic0 USB Driver on r_usb_basic > USB Module Number| USB_IP0 Port | USB_IP0 Port | This property is used to specify USB module number to be used as per configured USB speed.|
| configuration.xml > USBX paud Thread Stacks > g_transfer0 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer0 Transfer driver > Transfer Size| 2 | 2 |The transfer size of DMA for **Full Speed** requires **2 Bytes**.|
| configuration.xml > USBX paud Thread Stacks > g_transfer0 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer0 Transfer driver > Activation Source| No ELC Trigger | USBFS FIFO 1 (DMA Transfer request 1)  |This is an event trigger for DMA transfer 0 instance for destination pointer address. |
| configuration.xml > USBX paud Thread Stacks > g_transfer1 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer1 Transfer driver > Transfer Size| 2 | 2 |The transfer size of DMA for **Full Speed** requires **2 Bytes**.
| configuration.xml > USBX paud Thread Stacks > g_transfer1 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer1 Transfer driver > Activation Source| No ELC Trigger | USBFS FIFO 0 (DMA Transfer request 0)  |This is an event trigger for DMA transfer 1 instance for source pointer address.|

**Common Module Configuration Properties**

| configuration.xml > g_basic USB (r_usb_basic) > Settings > Property > Module g_basic USB >  USB RTOS Callback | NULL | usb_pcdc_callback | Defined user callback for sending events |

## API Usage ##

The table below lists the USB basic APIs used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_USB_Open| Open USB instance. |
|R_USB_Read| API is used to read data |
|R_USB_PeriControlDataGet| Receives data sent by control transfer |
|R_USB_PeriControlDataSet| Sets class request |
|R_USB_PeriControlStatusSet| Set the USB status |
|R_USB_Close| Stops USB module and terminates power |
|R_SCI_UART_Open| Open UART module |
|R_SCI_UART_BaudCalculate| Calculate Baud rate rgister settings |
|R_SCI_UART_Write| Writes data to UART Tx pin |
|R_SCI_UART_Close| Closed UARt driver and any lower level transfer drivers if used |

## Verifying operation ##
* Import, Build and Debug the EP (see section Starting Development of **FSP User Manual**).
* Connect USB cables, connecting wires as mentioned in the readme file [readme.txt](./readme.txt)
* Run the EP and then open JLink RTT Viewer to see the output status of the EP.
* Open two serial terminals (preferably Tera Term) on the HOST PC and connect to COM port as shown below:
	One COM for USB PCDC and another COM for external UART device.

	![com_ports](images/com_ports.jpg "COM_PORTs")
	
* Configure baud rate, stop bit and parity bit settings on the serial terminal (Tera Term) for external UART device:  

	![uart_com_port_setting](images/uart_com_port_settings.jpg "UART_COM_PORT_SETTINGS")

* Configure baud rate, stop bit and parity bit settings on the serial terminal (Tera Term) for USB PCDC based on the settings of the external UART device connected to converter.
  Note that only CTS/RTS flow control is tested with TeraTerm.
  
	![usb_pcdc_port_settings](images/usb_pcdc_com_port_settings.jpg "USB_PCDC_COM_PORT_SETTINGS")

* Enable "local echo" in the serial terminal if you would like to view the data sent by the serial terminal.
* Send data using serial terminal (Tera term) to external UART device.
* View the data received from the external UART device via the converter through the serial terminal (Tera Term).
* The status messages will be displayed in Jlink RTTViewer as depicted below:

	![rtt_log_1](images/rtt_1.jpg "RTT_LOG 1")
	
	![rtt_log_2](images/rtt_2.jpg "RTT_LOG 2")

* The sent and received data in the serial terminals can be observed as shown below:
	
	![tera_term_log](images/tera_term.jpg "TERA TERM LOG")
	
### Special Topics 
**Developing Descriptor**

A USB device provides information about itself in data structures called USB descriptors. 	

The host obtains descriptors from an attached device by sending various standard control requests to the default endpoint.
Those requests specify the type of descriptor to retrieve. In response to such requests, the device sends descriptors
that include information about the device, its configurations, interfaces and the related endpoints.
* Refer **Descriptor** section in FSP User manual (https://renesas.github.io/fsp/group___u_s_b___p_c_d_c.html) for developing the descriptor.

	

	












