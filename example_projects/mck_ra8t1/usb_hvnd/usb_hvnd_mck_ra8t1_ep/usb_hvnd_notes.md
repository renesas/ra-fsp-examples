# Introduction #

The sample code accompanying with this file shows operation of a Host Vendor class using the USB FS host on a RA MCU.
In this example, the application will configure the USB IP on the MCU as a Vendor Host device. This Host device will be connected to another RA MCU operating as USB Peripheral servicing Vendor class requests.
After the enumeration is completed, the Vendor Host board will write a fixed amount (default: 15 Bytes) of data to the Vendor Peripheral board and which is relayed back by the Peripheral.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the usb host vendor example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit
* Micro USB cables  (For MCK-RA8T1: Type C cable)
* OTG cable

Refer to [readme.txt](./readme.txt) on information on how to connect the hardware  

### Software ###
Refer to software described in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![usb_vendor_host](images/usb_vendor_class.jpg "USB Vendor Block Diagram")  
In the image, show an example of the two boards connected. For e.g. Host is EK-RA6M3 Peripheral is EK-RA4M1.

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| USB Host Vendor |USB Host Vendor class works by combining r_usb_basic module. | USB hvnd |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> g_basic USB Driver on R_USB_Basic > Settings > Property > Module g_basic USB Driver on R_USB_Basic > USB RTOS Callback  |   NULL   |   usb_host_vendor_callback   |   As RTOS is used, so the callback function is set and this callback function will notify user about occurrence of usb events.   |
|   configuration.xml -> g_basic USB Driver on R_USB_Basic > Settings > Property > Module g_basic USB Driver on R_USB_Basic > USB Speed  |   Full Speed   |   Hi Speed   |   This is changed to showcase Hi Speed functionality. Applicable only for EK_RA6M3/G board.   |
|   configuration.xml -> g_basic USB Driver on R_USB_Basic > Settings > Property > Module g_basic USB Driver on R_USB_Basic > USB Module Number  |   USB_IP0   |   USB_IP1   |   This is changed to USB_IP1,when USB Hi Speed is selected for EK_RA6M3/G board.   |
|   configuration.xml -> Host Thread > Settings > Property > Thread > Stack Size  |   1024   |   4096   |   This is changed to handle its worst-case function call nesting and local variable usage.   |
|   configuration.xml -> Host Thread > Settings > Property > Thread > Dynamic Allocation support  |   Disabled   |   Enabled   |   RTOS objects can be created using RAM that is automatically allocated from the FreeRTOS heap.   |
|   configuration.xml -> Host Thread > Settings > Property > Common > Total Heap Size  |   0   |   20000   |   This is changed because Dynamic Allocation support is enabled, so application makes use of amount of RAM available in the FreeRTOS heap.   |
|   configuration.xml -> Host Thread > Settings > Property > Common > Memory Allocation  |   static   |   Dynamic   |   This is changed to allocate memory for this object from a FreeRTOS heap.   |
|   configuration.xml -> g_queue Queue > Settings > Property > Queue Memory Allocation |   static   |   Dynamic   | This is changed to allocate memory for this object from a FreeRTOS heap.   |
|   configuration.xml -> g_queue Queue > Settings > Property > Queue Length |  20   |   10   | Queue length is assigned. |

## API Usage ##

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_USB_PipeRead | This API is used to Read data from peripheral, when USB Write complete event occur. |
|R_USB_PipeWrite| This API is used to Write data back to peripheral, when USB READ complete event occur.|
|R_USB_HostControlTransfer| This API is used to request control transfer.|

## Verifying operation ##
To Import, Build and Debug the EP, *see section Starting Development* of **FSP User Manual**. After running the EP, open RTT viewer to see the output.
