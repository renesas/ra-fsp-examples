# Introduction #

This project demonstrates the basic functionality of USBX Host CDC ACM module on Renesas RA MCUs based on Renesas FSP using AzureRTOS. Board 1 acts as Host device running USBX HCDC-ACM example project and Board 2 acts as Peripheral device running USBX PCDC-ACM example project and connected with Type-C USB cable via OTG. 
The peripheral initiates the communication and once the host reads the data initiated by the peripheral. The host will then send predefined data to the peripheral and the device will echo the data back to the host. 
The application status messages will be displayed on the J-Link RTT Viewer.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the USBX HCDC ACM example project, the following resources are needed.

### Software ###
1. Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware ###
* Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2
  * 2 x Renesas RA boards:
    * 1 x RA board runs the USBX HCDC ACM Example Project.
    * 1 x RA board runs the USBX PCDC ACM Example Project.
  * 1 x OTG cable: Type-C to USB Host (Female).
  * 3 x USB cables.
    * 2 x USB cables for programming and debugging.
    * 1 x Type-C USB cable used to connect the RA board 1 to the RA board 2 through OTG cable.

### Hardware Connections ###
* Connect board 1, running the USBX HCDC ACM example project, through OTG cable to board 2, running the USBX PCDC ACM example project.
* Connect USB debug ports of the two RA boards to USB ports of the host PC via two USB cables.
* For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full Speed):
  * Jumper J12 placement is pins 1-2.
  * Remove Jumper J15 pins.
  * Connect USB device to J11 connector via a micro USB Host cable.

* For EK-RA6M3 (High Speed):
  * Jumper J7: Connect pins 1-2.
  * Remove Jumper J17 pins.
  * Connect USB device to J6 connector via a micro USB Host cable.

* For MCK-RA8T1 (Full Speed):
  * Jumper JP9 placement is pins 1-2.
  * Remove Jumper J10 pins.
  * Connect USB device to CN14 connector via a Type-C USB Host cable.

* For EK-RA4L1 (Full Speed):
  * The user must turn OFF S4-4 to select USB Host Controller mode.
  * Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
  * Connect USB device to J11 connector via a Type-C USB Host cable.

* For EK-RA8E2, EK-RA8P1 (Full Speed):
  * Connect USB device to J11 connector via a Type-C USB Host cable.

* For MCK-RA8T2 (Full Speed): 
  * Jumper JP6 placement is pins 1-2.
  * Remove Jumper JP7 pins.  
  * Connect USB device to CN18 connector via a Type-C USB Host cable.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [Microsoft Azure USBX Host Class API ](https://docs.microsoft.com/en-us/azure/rtos/usbx/usbx-host-stack-5)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)
- [USBX HCDC Limitation](https://renesas.github.io/fsp/group___u_s_b_x.html?pos=151,4,246,4,311,7,398,4,413,4,1546,4,1665,4,5295,4)

# Project Notes #

## System Level Block Diagram ##
 High level block diagram
 
![usbx_hcdc_acm](images/usbx_hcdc_blockdiagram.jpg "High Level Block Diagram")

 Data transfer flow diagram of USBX HCDC
 
![usbx_hcdc_acm_flow](images/usbx_hcdc_flow_diagram.jpg "USBX HCDC data flow diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
|USBX HCDC | USBX HCDC is used for data transfer to and from a CDC peripheral device. | HCDC |
|USB Basic Driver | USB driver operates in combination with the device class drivers provided by Renesas to form a complete USB stack. | r_usb_basic |
|DMAC Transfer | DMAC is used to transfer the usb data for faster rate without using the CPU.  | r_dmac |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default.

**Common Configuration properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > HCDC ACM Thread > Properties > Settings > Property > Common > Timer > Timer Ticks Per Second |100 | 1000 | The default UX_PERIODIC_RATE ticks should be 1000 indicating 1 tick per millisecond. |
| configuration.xml > Stacks > HCDC ACM Thread > Properties > Settings > Property > Thread > Priority | 1 | 14 | HCDC ACM thread priority is lowered to allow the other USB operations at faster rate. |
| configuration.xml > Stacks > RTT Thread > Properties > Settings > Property > Thread > Priority | 1 | 15 | RTT thread priority is lowered to allow the USBX HCDC ACM data process at the fastest rate possible. |
| configuration.xml > Stacks > HCDC ACM Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Common > DMA/DTC Support | DMA Disable | DMA Enable | DMA is enabled for faster data processing. |

**Configuration Properties for USB Speed as High-Speed**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > HCDC ACM Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Module g_basic0 USB (r_usb_basic) > USB Speed | Full-Speed | Hi-Speed | This property is used to configure USB speed. |
| configuration.xml > Stacks > HCDC ACM Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Common > DMA/DTC Source Address| DMA Disabled | HS Address | USB basic driver is configured USB Speed as High Speed. Accordingly, DMA Source Address is provided with HS Address. |
| configuration.xml > Stacks > HCDC ACM Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Common > DMA/DTC Destination Address | DMA Disabled | HS Address | USB basic driver is configured USB speed as High Speed (Default). Accordingly, DMA Destination Address is provided with HS Address. |
| configuration.xml > Stacks > HCDC ACM Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Module g_basic0 USB (r_usb_basic) > USB Module Number | USB_IP0_Port | USB_IP1_Port | This property is used to specify USB module number to be used as per configured USB speed. |
| configuration.xml > Stacks > HCDC ACM Thread > g_transfer0 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer0 Transfer (r_dmac) > Activation Source | No ELC Trigger | USBHS FIFO 1 (DMA/DTC Transfer request 1)  |This is an event trigger for DMA transfer 0 instance for destination pointer address. |
| configuration.xml > Stacks > HCDC ACM Thread > g_transfer1 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer1 Transfer (r_dmac) > Activation Source | No ELC Trigger | USBHS FIFO 0 (DMA/DTC Transfer request 0)  |This is an event trigger for DMA transfer 1 instance for source pointer address. |
| configuration.xml > Stacks > HCDC ACM Thread > g_transfer1 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer1 Transfer (r_dmac) > Transfer Size | 2 Bytes | 4 Bytes  | This is used to select DMAC transfer size for High Speed. |
| configuration.xml > Stacks > HCDC ACM Thread > g_transfer0 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer0 Transfer (r_dmac) > Transfer Size | 2 Bytes | 4 Bytes  | This is used to select DMAC transfer size for High Speed. |

**Configuration Properties for USB Speed as Full-Speed**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > HCDC ACM Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Common > DMA/DTC Source Address | DMA Disabled | FS Address | USB basic driver is configured USB Speed as Full Speed. Accordingly, DMA Source Address is provided with FS Address. |
| configuration.xml > Stacks > HCDC ACM Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Common > DMA/DTC Destination Address | DMA Disabled | FS Address | USB basic driver is configured USB speed as Full Speed (Default). Accordingly, DMA Destination Address is provided with FS Address. |
| configuration.xml > Stacks > HCDC ACM Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Module g_basic0 USB (r_usb_basic) > USB Module Number | USB_IP0_Port | USB_IP0_Port | This property is used to specify USB module number to be used as per configured USB speed. |
| configuration.xml > Stacks > HCDC ACM Thread > g_transfer0 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer0 Transfer (r_dmac) > Activation Source | No ELC Trigger | USBFS FIFO 1 (DMA/DTC Transfer request 1)  |This is an event trigger for DMA transfer 0 instance for destination pointer address. |
| configuration.xml > Stacks > HCDC ACM Thread > g_transfer1 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer1 Transfer (r_dmac) > Activation Source | No ELC Trigger | USBFS FIFO 0 (DMA/DTC Transfer request 0)  |This is an event trigger for DMA transfer 1 instance for source pointer address. |
| configuration.xml > Stacks > HCDC ACM Thread > g_transfer1 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer1 Transfer (r_dmac) > Transfer Size | 2 Bytes | 2 Bytes  | This is used to select DMAC transfer size for Full Speed. |
| configuration.xml > Stacks > HCDC ACM Thread > g_transfer0 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer0 Transfer (r_dmac) > Transfer Size | 2 Bytes | 2 Bytes  | This is used to select DMAC transfer size for Full Speed. |


The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| _ux_system_initialize | This API is used to initialize the USBX system. |
| ux_host_stack_initialize | This API is used to initialize the USBX device stack. |
| ux_host_class_cdc_acm_write | This API is used to write to the CDC class. |
| ux_host_class_cdc_acm_read | This API is used to read from the CDC class. |
| R_USB_Open|This API is used to open the USB basic driver. |

**Callback:**  
The ux_host_usr_event_notification callback function is called to identify the USB events and perform operation as per the received events. Also, there are only two events available in the EP, they are used to signal the HCDC that USB communication can be started.
1. When the event UX_FSP_DEVICE_INSERTION is received the USBX_HCDC checks for the CDC class interface and sets the event flag to perform USBX_HCDC operations (e.g., write/read).
2. When the event UX_FSP_DEVICE_REMOVAL is received the USBX_HCDC will clear the event flag and wait for USB_INSERTION event.

## Verifying operation ##
1. Import, generate and build the USBX PCDC ACM and USBX HCDC ACM EPs.
2. Now, flash USBX PCDC ACM code on RA board (e.g., board 2) which will act as peripheral device.
3. Open J-Link RTT Viewer for board 2.
4. Next, flash USBX HCDC ACM code on another RA board (e.g., board 1) which will act as host device.
5. Open J-Link RTT Viewer for board 1 and verify host device operation.

**Note:** Both USBX HCDC ACM and USBX PCDC ACM need to be running at High Speed or Full Speed for the EP functionality to work correctly.

1. The output on **J-Link RTT Viewer** for USBX HCDC:

 ![usb_hcdc_acm_rtt_log](images/USBX_HCDC_RTT_log.jpg "RTT output")

## Special Topics ##
* How to Switch between USB FS and USB HS for MCU supporting both USB speed:
1. Select the USB Speed and USB Module Number from the configuration property as per supported USB speed.
2. Configure the DMA/DTC Source Address and DMA/DTC Destination Address, according to the USB speed selected.  

![usb_hcdc_acm_FS_HS](images/FS_HS_change.jpg "FS_HS_change")

3. When using DMAC, Configure the transfer size as per the selected USB Speed (e.g., For USB FS: 2 bytes and USB HS: 4 bytes).

![usb_hcdc_acm_transfer_size](images/dmac_transfer_size.jpg "dmac_transfer_size")
