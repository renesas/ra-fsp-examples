# Introduction #

The sample code accompanying with this file shows operation of a Peripheral Vendor class using the USB FS peripheral on a RA MCU.
In this example, the application will configure the USB IP on the MCU as a Vendor Peripheral device. This Peripheral device will be connected to another RA MCU operating as USB Host servicing Vendor class requests.
After the enumeration is completed, the Vendor Peripheral board will write a fixed amount (default: 15 Bytes) of data to the Vendor Host board which is relayed back by the Host.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the usb peripheral vendor example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit.
* Micro USB cables.
* OTG cable.

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
![usb_vendor_peri](images/usb_vendor_class.jpg "USB Vendor Block Diagram")  
In the image, show an example of the two boards connected. For e.g. Host is EK-RA6M3 Peripheral is EK-RA4M1.

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| USB Peripheral Vendor |USB Peripheral Vendor class works by combining r_usb_basic module. | USB pvnd |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> g_basic USB Driver on R_USB_Basic > Settings > Property > Module g_basic USB Driver on R_USB_Basic > USB RTOS Callback  |   NULL   |   usb_peri_vendor_callback   |   As RTOS is used, so the callback function is set and this callback function will notify user about occurrence of usb events.   |
|   configuration.xml -> g_basic USB Driver on R_USB_Basic > Settings > Property > Module g_basic USB Driver on R_USB_Basic > USB Speed  |   Full Speed   |   Hi Speed   |   This is changed to showcase Hi Speed functionality. Applicable only for EK_RA6M3/G board.   |
|   configuration.xml -> g_basic USB Driver on R_USB_Basic > Settings > Property > Module g_basic USB Driver on R_USB_Basic > USB Module Number  |   USB_IP0   |   USB_IP1   |   This is changed to USB_IP1,when USB Hi Speed is selected for EK_RA6M3/G board.   |
|   configuration.xml -> Peri Thread > Settings > Property > Thread > Stack Size  |   1024   |   4096   |   This is changed to handle its worst-case function call nesting and local variable usage.   |
|   configuration.xml -> Peri Thread > Settings > Property > Thread > Dynamic Allocation support  |   Disabled   |   Enabled   |   RTOS objects can be created using RAM that is automatically allocated from the FreeRTOS heap.   |
|   configuration.xml -> Peri Thread > Settings > Property > Common > Total Heap Size  |   0   |   40000   |   This is changed because Dynamic Allocation support is enabled, so application makes use of amount of RAM available in the FreeRTOS heap.   |
|   configuration.xml -> Peri Thread > Settings > Property > Common > Memory Allocation  |   static   |   Dynamic   |   This is changed to allocate memory for this object from a FreeRTOS heap.   |
|   configuration.xml -> g_queue Queue > Settings > Property > Queue Memory Allocation |   static   |   Dynamic   | This is changed to allocate memory for this object from a FreeRTOS heap.   |
|   configuration.xml -> g_queue Queue > Settings > Property > Queue Length |  20   |   10   | Queue length is assigned. |

## API Usage ##

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_USB_PipeRead | This API is used to Read data from host, when USB Write complete event occur. |
|R_USB_PipeWrite| This API is used to Write data back to host, when USB READ complete event occur.|
|R_USB_PeriControlDataGet| This API is used to get data length from host.|
|R_USB_PeriControlDataSet| This API is used to set data length in peripheral.|
|R_USB_PeriControlStatusSet| This API is used to set the USB status as ACK response.|

## Verifying operation ##
To Import, Build and Debug the EP, *see section Starting Development* of **FSP User Manual**. After running the EP, open RTT viewer to see the output.

## Special Topics ##

### Developing Descriptor ###
Refer **Descriptor** section of [usb_peripheral_vendor_descriptor](https://renesas.github.io/fsp/group___u_s_b___p_v_n_d.html) for developing a descriptor.
The template file provided can be placed in the src folder after removing the .template file extension.
