# Introduction #

The sample code accompanying this file shows the operation of a USBX Peripheral Printer on a RA MCU using Azure RTOS. 
An RA MCU board will be connected to the PC where the RA board will act as a peripheral printer and the PC (Windows System)
will act as a host device. A print command for a test file will be sent from the host PC, and printing data will be 
transferred from the host PC to MCU. The RA MCU will display Host PC print information, settings and print data on the J-Link RTT Viewer.
Error and info messages will be printed on J-Link RTT Viewer.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of the operation.

## Required Resources ##
To build and run the USBX Peripheral Printer example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit
* 2x Micro USB cables
* Host PC (Windows 10)

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![USBX PPRN High Level Block Diagram](images/USBX_PPRN_HLD.png "USBX PPRN Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
| :---------: | :---------------: | :------------: |
| USBX PPRN   | This module provides USBX Peripheral Printer class support on RA device. | pprn |

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

**Common Configuration Properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml > BSP > Properties > Settings > Property > Main Stack Size (bytes)| 0x400 |0x400| Main Program thread stack is configured to store the local variables of different functions in the code. |
|   configuration.xml > BSP > Properties > Settings > Property > Heap Size (bytes)| 0 |0x400| Heap size is required for standard library functions to be used. |
|   configuration.xml > USBX PPRN Thread > Properties > Settings > Property > Common > Timer > Timer Ticks Per Second |100 | 1000 |The default ticks should be 1000 indicating 1 tick per millisecond.  |
|   configuration.xml > USBX PPRN READ Thread > Properties > Settings > Property > Common > Timer > Timer Ticks Per Second |100 | 1000 |The default ticks should be 1000 indicating 1 tick per millisecond.  |
|   configuration.xml > USBX PPRN Thread > Settings > Property > Thread > priority  |   1   |   15   |   USBX PPRN Thread priority is lowered to allow the other USB operations to be serviced at a faster rate.   |
|   configuration.xml > USBX PPRN Read Thread > Settings > Property > Thread > priority  |   1   |   15   |   USBX PPRN Read Thread priority is lowered to allow the other USB operations to be serviced at a faster rate.   |
|   configuration.xml > RTT Thread > Settings > Property > Thread > priority  |   1   |   16   |   RTT thread priority is lowered to allow the internal thread taking priority. |
|   configuration.xml > RTT Thread > Properties > Settings > Property > Stack Size (bytes)| 1024 | 1024 | The size of the RTT Thread Stack is default to prevent memory overflow. |


**Configuration Properties if USB Speed as High Speed**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > USBX PPRN Thread Stacks > g_basic0 USB > Properties > Settings > Property > Module g_basic0 USB Driver on r_usb_basic > USB Speed| Full Speed | Hi Speed | USB Speed is configured as **Hi Speed**. |
| configuration.xml > USBX PPRN Thread Stacks > g_basic0 USB > Properties > Settings > Property > Module g_basic0 USB Driver on r_usb_basic > USB Module Number| USB_IP0 Port | USB_IP1 Port | This property is used to specify USB module number to be used as per configured USB speed.|

**Configuration Properties if USB Speed as Full Speed.**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > USBX PPRN Thread Stacks > g_basic0 USB > Properties > Settings > Property > Module g_basic0 USB Driver on r_usb_basic > USB Speed| Full Speed | Full Speed | USB Speed is configured as **Full Speed**. |
| configuration.xml > USBX PPRN Thread Stacks > g_basic0 USB > Properties > Settings > Property > Module g_basic0 USB Driver on r_usb_basic > USB Module Number| USB_IP0 Port | USB_IP0 Port | This property is used to specify USB module number to be used as per configured USB speed.|

## API Usage ##

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|[ux_system_initialize](https://docs.microsoft.com/en-us/azure/rtos/usbx/usbx-device-stack-2#initialization-of-usbx-resources)| This API is used to initialize the USBX system. |
|[ux_device_stack_initialize](https://docs.microsoft.com/en-us/azure/rtos/usbx/usbx-device-stack-4#ux_device_stack_initialize)| This API is used to initialize the USBX device stack. |
|[ux_device_stack_class_register](https://docs.microsoft.com/en-us/azure/rtos/usbx/usbx-device-stack-4#ux_device_stack_class_register)| This API is used by the application to register a new USB device class. |
|[ux_device_class_printer_read](https://renesas.github.io/fsp/group___u_s_b_x.html)| This API reads from the Printer class and saves the received data in a buffer. |
|[usb_peri_usbx_initialize](https://renesas.github.io/fsp/group___u_s_b_x.html)| This API is used to process USB peripheral initialization. |
|[R_USB_Open](https://renesas.github.io/fsp/group___u_s_b_x.html)| This API opens the USB basic driver. |


Refer to [USBX Device Class Considerations](https://docs.microsoft.com/en-us/azure/rtos/usbx/usbx-device-stack-5#usb-device-cdc-acm-class) for more detailed information on the APIs used.


## Verifying operation ##

If the USB speed configuration needs to be changed, import the EP and perform necessary changes in the project configuration. Please see the Module Configuration Notes section for more details. Furthermore, the following updates are needed in the example project.
Note: The EP is set to support USB High Speed by default.

  **For High Speed USB on EK-RA6M5** 
    
	Uncomment the macro HIGH_SPEED_CONFIG in usbx_pprn_ep.h 
	
  **For Full Speed USB on EK-RA6M5**
  
    Comment the macro HIGH_SPEED_CONFIG in usbx_pprn_ep.h
    
1. Generate, build, and download the USBX PPRN project to the RA board.

2. Check the printer device status in Device Manager or in Windows settings.
   
    Below images show the device enumeration of the printer in device manager and in Windows settings respectively:

![usbx_pprn_device_printer](images/usbx_pprn_device_printer.png "USBX_PPRN_Printer")

![usbx_pprn_device_printer_settings](images/usbx_pprn_device_printer_settings.png "USBX_PPRN_Printer in Windows Settings")

3. Open J-Link RTT Viewer to verify the initialization status.

    Below images show the output on J-Link RTT Viewer:

![usbx_pprn_rtt_output](images/usbx_pprn_rtt_log_1.png "USBX PPRN EP Info and Initialization")

![usbx_pprn_rtt_output](images/usbx_pprn_rtt_log_2.png "USBX PPRN Device Initialization")

4. Once the USB initialization is complete, open any file (txt, docx, pdf, xlsx (Text Only)) to print the file using Generic / Text Only printer. After opening, give the print command or press *Ctrl+P* on the keyboard. This will invoke the print window.

5. Select the **Generic / Text Only** device as the printer, and click on Print, as shown in the below image:

![usbx_pprn_printer](images/usbx_pprn_printer_name.png "USBX PPRN Print Dialog box")

Below image shows the printing data on the J-Link RTT Viewer after a print command is given:

![usbx_pprn_rtt_output_print_data](images/usbx_pprn_rtt_print.png "USBX PPRN Print File Data")

## Special Topics ##

**Developing Descriptor**

A USB device provides information about itself in data structures called USB descriptors. 	

The host obtains descriptors from an attached device by sending various standard control requests to the default endpoint. Those requests specify the type of descriptor to retrieve. In response to such requests, the device sends descriptors that include information about the device, its configurations, interfaces, and the related endpoints.

* Refer **Descriptor** section in [Device Class (rm_usbx_port)](https://renesas.github.io/fsp/group___u_s_b_x.html) for developing the descriptor. We can take the template file (**rm_usbx_pcdc_descriptor.c.template**) of the required USB combination from **ra/fsp/src/rm_usbx_port** folder and use the same in the source folder by removing the **.template** file extension.