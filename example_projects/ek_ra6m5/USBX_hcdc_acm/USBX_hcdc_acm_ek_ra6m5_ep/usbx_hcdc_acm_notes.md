# Introduction #

This project demonstrates the basic functionality of USBX Host CDC ACM module on Renesas RA MCUs based on Renesas FSP using AzureRTOS. Board 1 acts as Host device running USBX HCDC-ACM example project and Board 2 acts as Peripheral device running USBX PCDC-ACM example project and connected with micro USB cable via OTG. 
The peripheral initiates the communication and once the host reads the data initiated by the peripheral. The host will then send predefined data to the peripheral and the device will echo the data back to the host. 
The application status messages will be displayed on the Jlink RTT Viewer.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the USBx HCDC ACM example project, the following resources are needed.

### Hardware ###
* Two Renesas RA™ MCU kit are needed one for peripheral and one for host.
* USB A to USB Micro B Cable (For MCK-RA8T1: Type C cable) - 3 nos.
* OTG cable: Micro to USB Host (For MCK-RA8T1: Type C to USB host).

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.


### Software ###
1. Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)


 
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
|USBX HCDC| USBX HCDC is used for data transfer to and from a CDC peripheral device. | HCDC |
|USB Basic Driver | USB driver operates in combination with the device class drivers provided by Renesas to form a complete USB stack.|r_usb_basic|
|DMAC Transfer |DMAC is used to transfer the usb data for faster rate without using the CPU.  | r_dmac  |



## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

**Common Configuration properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml -> HCDC ACM Thread -> Properties > Settings > Property >Common > Timer> Timer Ticks Per Second |100 | 1000 |The default UX_PERIODIC_RATE ticks should be 1000 indicating 1 tick per millisecond.|
| configuration.xml -> HCDC ACM Thread -> Properties > Settings > Property >Thread > Priority| 1 | 14 |HCDC ACM thread priority is lowered to allow the other usb operations at faster rate.|
| configuration.xml -> RTT Thread -> Properties > Settings > Property >Thread > Priority| 1 | 15 | RTT thread priority is lowered to allow the USBX HCDC ACM data process at the fastest rate possible.|
| configuration.xml -> HCDC ACM Thread Stacks -> g_basic0 Instance > Properties > Settings > Property > Common >DMA Support| Disable | Enable | DMA is enabled for faster data processing.|

**Configuration Properties for USB Speed as High-Speed**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml -> HCDC ACM Thread Stacks -> g_basic0 Instance > Properties > Settings > Property > g_basic0 > USB Speed| Full-Speed | Hi-Speed | This property is used to configure USB speed. |
| configuration.xml -> HCDC ACM Thread Stacks -> g_basic0 Instance > Properties > Settings > Property > Common >DMA Source Address| DMA Disabled | HS Address | USB basic driver is configured USB Speed as High Speed. Accordingly, DMA Source Address is provided with HS Address.|
| configuration.xml -> HCDC ACM Thread Stacks -> g_basic0 Instance > Properties > Settings > Property > Common >DMA Destination Address| DMA Disabled | HS Address | USB basic driver is configured USB speed as High Speed (Default). Accordingly, DMA Destination Address is provided with HS Address. |
| configuration.xml -> HCDC ACM Thread Stacks -> g_basic0 Instance > Properties > Settings > Property > g_basic0 > USB Module Number | USB_IP0_Port | USB_IP1_Port | This property is used to specify USB module number to be used as per configured USB speed. |
| configuration.xml -> HCDC ACM Thread Stacks -> g_transfer0 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer0 Transfer driver > Activation Source| No ELC Trigger | USBHS FIFO 1 (DMA Transfer request 1)  |This is an event trigger for DMA transfer 0 instance for destination pointer address.|
| configuration.xml -> HCDC ACM Thread Stacks -> g_transfer1 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer1 Transfer driver > Activation Source| No ELC Trigger | USBHS FIFO 0 (DMA Transfer request 0)  |This is an event trigger for DMA transfer 1 instance for source pointer address.|
| configuration.xml -> HCDC ACM Thread Stacks -> g_transfer1 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer1 Transfer driver > Transfer Size| 2 Bytes | 4 Bytes  | This is used to select DMAC transfer size for High Speed. |
| configuration.xml -> HCDC ACM Thread Stacks -> g_transfer0 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer0 Transfer driver > Transfer Size| 2 Bytes | 4 Bytes  | This is used to select DMAC transfer size for High Speed. |

**Configuration Properties for USB Speed as Full-Speed**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml -> HCDC ACM Thread Stacks -> g_basic0 Instance > Properties > Settings > Property > Common > DMA Source Address| DMA Disabled | FS Address | USB basic driver is configured USB Speed as Full Speed. Accordingly, DMA Source Address is provided with FS Address.|
| configuration.xml -> HCDC ACM Thread Stacks -> g_basic0 Instance > Properties > Settings > Property > Common > DMA Destination Address| DMA Disabled | FS Address | USB basic driver is configured USB speed as Full Speed (Default). Accordingly, DMA Destination Address is provided with FS Address. |
| configuration.xml -> HCDC ACM Thread Stacks -> g_transfer0 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer0 Transfer driver > Activation Source| No ELC Trigger | USBFS FIFO 1 (DMA Transfer request 1)  |This is an event trigger for DMA transfer 0 instance for destination pointer address.|
| configuration.xml -> HCDC ACM Thread Stacks -> g_transfer1 Transfer Driver on r_dmac Instance > Properties > Settings > Property > Module g_transfer1 Transfer driver > Activation Source| No ELC Trigger | USBFS FIFO 0 (DMA Transfer request 0)  |This is an event trigger for DMA transfer 1 instance for source pointer address.|


The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|_ux_system_initialize|This API is used to initialize the USBx system. |
|ux_host_stack_initialize|This API is used to initialize the USBx device stack.  |
|ux_host_class_cdc_acm_write|This API is used to write to the CDC class.|
|ux_host_class_cdc_acm_read|This API is used to read from the CDC class.|
|R_USB_Open|This API opens the USB basic driver.|

**Callback:**  
ux_host_usr_event_notification callback function is called to identify the USB events and perform operation as per the received events, Also there are only two events available in the EP, they are used to signal the HCDC that USB communication can be started.
1. When event UX_FSP_DEVICE_INSERTION is received the USBX_HCDC checks for the interface cdc class and sets the event flag to perform USBX_HCDC operation(i.e write/read).
2. When event UX_FSP_DEVICE_REMOVAL is received the USBX_HCDC will clear the event flag and will wait for USB_INSERTION event.

## Verifying operation ##
1. Import, generate and build the USBX PCDC and USBX HCDC EP.
2. Now, flash USBX PCDC code on RA board(i.e board 2) which will acts as peripheral device.
3. Open RTT viewer for Board 2.
4. Next, flash USBX HCDC code on another RA Board (i.e board 1) which will acts as host device.
5. Open RTT viewer for Board 1 and verify host device operation.

**Note:** Both USBX HCDC and PCDC board needs to be running at High Speed or Full Speed in order for the correct EP functionality.


1. Below images showcases the output on **JLinkRTT_Viewer** for USBX HCDC:

 ![usb_hcdc_acm_rtt_log](images/USBX_HCDC_RTT_log.jpg "RTT output")

## Special Topics ##
* How to Switch between USB FS to HS for MCU supporting both USB speed:
1. Select the USB Speed and USB Module Number from configuration property as per supported USB speed.
2. Configure the DMA Source Address and DMA Destination Address, according to the USB speed selected.  
![usb_hcdc_acm_FS_HS](images/FS_HS_change.jpg "FS_HS_change")
3. When using DMAC, Configure the transfer size as per the selected USB Speed (i.e For FS: 2 bytes and HS: 4 bytes).  
![usb_hcdc_acm_transfer_size](images/dmac_transfer_size.jpg "dmac_transfer_size")
