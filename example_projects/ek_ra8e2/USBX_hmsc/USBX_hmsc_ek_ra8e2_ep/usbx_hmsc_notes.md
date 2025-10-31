# Introduction #
This project demonstrates the basic functionality of USBX Host MSC module using FileX on Renesas RA MCUs based on Renesas FSP using AzureRTOS. FileX interface on USBX uses the underlying
HMSC driver to perform various file operations such as File Create, Write, Read, Display current directory and Eject the USB Pendrive.
These operations are done via FileX interface, on underlying USBX_HMSC driver based on user input from the RTT Viewer.
The application status messages will be displayed on the J-Link RTT Viewer.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the USBX HMSC ACM example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.2.0
* e2 studio: Version 2025-10
* SEGGER J-Link RTT Viewer: Version 8.74
* LLVM Embedded Toolchain for ARM: Version 18.1.3
* Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware ###
Supported RA boards: EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA4L1, EK-RA8E2
* 1 x Renesas RA board.
* 1 x Type-C USB cable for programming and debugging.
* 1 x OTG cable.
* 1 x USB Pendrive.

### Hardware Connections ###
For EK-RA6M3 (High-Speed):
   * Jumper J7: Connect pins 1-2
   * Remove Jumper J17 pins
   * Connect USB device to J6 connector with help of micro USB Host cable.

For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
   * Jumper J7: Connect pins 1-2
   * Remove Jumper J17 pins
   * Connect USB device to J31 connector with help of micro USB Host cable.
 
For EK-RA6M4, EK-RA4M2, EK-RA4M3 (Full-Speed):
   * Jumper J12 placement is pins 1-2
   * Remove Jumper J15 pins
   * Connect USB device to J11 connector with help of micro USB Host cable.
   * Note: For EK-RA8D1, the user must turn OFF SW1-6 to use USBHS.

For MCK-RA8T1 (Full-Speed):
   * Jumper JP9 placement is pins 1-2
   * Remove Jumper JP10 pins
   * Connect USB device to CN14 connector with help of Type-C Host cable.
		
For EK-RA4L1 (Full-Speed):
   * Turn ON S4-4 to select USB device mode. 
   * Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
   * Connect USB device to J11 connector with help of Type-C USB Host cable.

For EK-RA8E2 (Full-Speed):
   * Connect USB device to J11 connector with help of Type-C USB Host cable.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [Microsoft Azure USBX Host Class API ](https://docs.microsoft.com/en-us/azure/rtos/usbx/usbx-host-stack-5)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #
## System Level Block Diagram ##
 High level block diagram
 
![usbx_hmsc](images/Block_diagram.jpg "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
|FileX on USBX|  With FileX on USBX HMSC driver, the user can perform different file operation or store data on the USB Pendrive. | USBX |
|USBX HMSC | This module provide a USBX Host Mass storage class support on RA device. Using USBX HMSC in combination with a file system it is possible to communicate with USB storage devices.| HMSC|
|USB Basic Driver | USB driver operates in combination with the device class drivers provided by Renesas to form a complete USB stack.|r_usb_basic|
|DMAC Transfer | DMAC is used to transfer the USB data for faster rate without using the CPU.  | r_dmac  |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

**Common Configuration properties**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > HMSC Thread > Properties > Settings > Property > Common > Timer> Timer Ticks Per Second |100 | 1000 |The default UX_PERIODIC_RATE ticks should be 1000 indicating 1 tick per millisecond.|
| configuration.xml > Stacks > HMSC Thread > Properties > Settings > Property > Thread > Priority| 1 | 14 |HMSC thread priority is lowered to allow the other USB operations at faster rate.|
| configuration.xml > Stacks > RTT Thread > Properties > Settings > Property > Thread > Priority| 1 | 15 | RTT thread priority is lowered to allow the USBX HMSC data process at the fastest rate possible.|
| configuration.xml > Stacks > HMSC Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Common > DMA/DTC Support | DMA Disabled | DMA Enabled | DMA is enabled to off load the MCU processing and provide priorities for data transfer. |

**Configuration Properties for using USBHS**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > HMSC Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > g_basic0 > USB Speed| Full-Speed | Hi-Speed | This property is used to configure USB speed. |
| configuration.xml > Stacks > HMSC Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Common > DMA/DTC Source Address| DMA Disabled | HS Address | USB basic driver is configured USB Speed as High Speed. Accordingly, DMA Source Address is provided with HS Address.|
| configuration.xml > Stacks > HMSC Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Common > DMA/DTC Destination Address| DMA Disabled | HS Address | USB basic driver is configured USB speed as High Speed (Default). Accordingly, DMA Destination Address is provided with HS Address. |
| configuration.xml > Stacks > HMSC Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > g_basic0 > USB Module Number | USB_IP0_Port | USB_IP1_Port | This property is used to specify USB module number to be used as per configured USB speed. |
| configuration.xml > Stacks > HMSC Thread > g_transfer0 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer0 Transfer (r_dmac) > Activation Source| No ELC Trigger | USBHS FIFO 1 (DMA/DTC Transfer request 1)  |This is an event trigger for DMA transfer 0 instance for destination pointer address.|
| configuration.xml > Stacks > HMSC Thread > g_transfer1 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer1 Transfer (r_dmac) > Activation Source| No ELC Trigger | USBHS FIFO 0 (DMA/DTC Transfer request 0)  |This is an event trigger for DMA transfer 1 instance for source pointer address.|
| configuration.xml > Stacks > HMSC Thread > g_transfer1 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer1 Transfer (r_dmac) > Transfer Size| 2 Bytes | 4 Bytes  | This is used to select DMAC transfer size. |
| configuration.xml > Stacks > HMSC Thread > g_transfer0 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer0 Transfer (r_dmac) > Transfer Size| 2 Bytes | 4 Bytes  |This is used to select DMAC transfer size. |

**Configuration Properties for using USBFS**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > HMSC Thread > g_basic0 Instance > Properties > Settings > Property > Common > DMA/DTC Source Address| DMA Disabled | FS Address | USB basic driver is configured USB Speed as Full Speed. Accordingly, DMA Source Address is provided with FS Address.|
| configuration.xml > Stacks > HMSC Thread > g_basic0 Instance > Properties > Settings > Property > Common > DMA/DTC Destination Address| DMA Disabled | FS Address | USB basic driver is configured USB speed as Full Speed. Accordingly, DMA Destination Address is provided with FS Address. |
| configuration.xml > Stacks > HMSC Thread > g_transfer0 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer0 Transfer (r_dmac) > Activation Source| No ELC Trigger | USBFS FIFO 1 (DMA/DTC transfer request 1)  |This is an event trigger for DMA transfer 0 instance for destination pointer address.|
| configuration.xml > Stacks > HMSC Thread > g_transfer1 Transfer (r_dmac) > Properties > Settings > Property > Module g_transfer1 Transfer (r_dmac) > Activation Source| No ELC Trigger | USBFS FIFO 0 (DMA/DTC transfer request 0)  |This is an event trigger for DMA transfer 1 instance for source pointer address.|

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|_ux_system_initialize|This API is used to initialize the USBx system. |
|fx_system_initialize|This API is used to initialize the FileX System component. |
|ux_host_stack_initialize|This API is used to initialize the USBx device stack.  |
|fx_system_date_set|This API is used to set the system date.  |
|fx_system_time_set|This API is used to set the system time.  |
|R_USB_Open|This API opens the USB basic driver. |
|fx_media_extended_space_available|This API is used to check media space available.  |
|fx_media_volume_get|This API is used to retrieves media volume name.  |
|fx_directory_create|This API is used to creates a subdirectory in the current directory or in the path provided in directory name.  |
|fx_directory_default_set|This API is used to set the default directory of the media.  |
|fx_file_create|This API is used to creates the specified file in the default directory with specified name.  |
|fx_file_open|This API is used to open the specified file for either reading or writing. |
|fx_file_truncate|This API is used to truncates the size of the file to the specified size.  |
|fx_file_write|This API is used to write bytes from the specified buffer starting at the file's current position.  |
|fx_file_close|This API is used to close the specified file.  |
|fx_media_flush|This API is used to flush all cached sectors and directory entries of any modified files to the physical media.  |
|fx_file_read|This API is used to read bytes from the file and stores them in the supplied buffer.  |
|fx_directory_first_full_entry_find|This API is used to get first directory entry with full information.  |
|fx_directory_next_full_entry_find|This API is used to get next directory entry with full information.  |
|fx_media_close|This API is used to close the specified media.  |
|ux_host_stack_uninitialize|This API is used to uninitialize all the host code for USBX.|

* Callback:  
ux_host_usr_event_notification callback function is called to identify the USB events and perform operation as per the received events, also there are only two events available in the EP, they are used to signal the HMSC that USB communication can be started.
1. When the event UX_FSP_DEVICE_INSERTION is received the USBX_HMSC checks for the MSC class interface and sets the event flag to perform the USBX_HMSC operations (i.e create directory/write/read/eject/display).
2. When the event UX_FSP_DEVICE_REMOVAL is received the USBX_HMSC will clear the event flag and wait for the USB_INSERTION event.

## Verifying operation ##
1. Import, generate and build the EP.
2. Now, flash USBX HMSC code on RA board which will act as host device.
3. Open RTT Viewer and perform host device operation as per MENU Option.
4. The images below showcases the output on J-Link RTT Viewer for USBX HMSC:

 ![usbx_hmsc_rtt_log](images/USBX_HMSC_RTT_log1.jpg "RTT output")
 
 ![usbx_hmsc_rtt_log](images/USBX_HMSC_RTT_log2.jpg "RTT output")
 
 ![usbx_hmsc_rtt_log](images/USBX_HMSC_RTT_log3.jpg "RTT output")
 
 ![usbx_hmsc_rtt_log](images/USBX_HMSC_RTT_log4.jpg "RTT output")

## Special Topics ##
* The user is expected to execute the "Eject" option before removing USB else further file operations may fail and USB data may get corrupted.
* Operation is not guaranteed for any user input value other than int or char types (e.g., float, special characters) through J-Link RTT Viewer input.
