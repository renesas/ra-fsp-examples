# Introduction #

This project demonstrates the basic functionality of USBX Host HID module on Renesas RA MCUs based on Renesas FSP using AzureRTOS.    
USB Keyboard and USB Mouse will be enumerated by the RA MCU, which is configured as usbx_hhid host. On connecting      
Keyboard as HID, the RA boards read key pressed by the user on the keyboard connected and prints it on the RTTViewer.
On connecting a mouse as HID, the RA board reads the mouse X & Y coordinates, mouse button pressed and prints.
The updated value of x and y axis prints only when there is change in X and Y Position. 
The application status messages will be displayed on the Jlink RTT Viewer.


Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the USBX HHID example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit - 1x
* USB A to USB Micro B Cable - 1x
* OTG cable: Micro to USB Host (Female) - 1x
* USB Keyboard - 1x
* USB Mouse    - 1x

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
1. Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)


## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [Microsoft Azure USBX Host Class API ](https://docs.microsoft.com/en-us/azure/rtos/usbx/usbx-host-stack-5)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)


# Project Notes #

## System Level Block Diagram ##
 High level block diagram
 
 
![usbx_hhid](images/Block_diagram.jpg "High Level Block Diagram")


## FSP Modules Used ##
List of important modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
|USBX HHID | This usbx_hhid module combines with the r_usb_basic module to provide a USBX Host Human Interface Device Class (HHID) driver. | HHID|
|USB Porting layer | This USB driver works by combining USBX and r_usb_basic module |rm_usb_port|
|USB Basic Driver | USB driver operates in combination with the device class drivers provided by Renesas to form a complete USB stack|r_usb_basic|


## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

**Common Configuration properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml -> HHID Thread -> Properties > Settings > Property >Common > Timer> Timer Ticks Per Second |100 | 1000 |The default UX_PERIODIC_RATE ticks should be 1000 indicating 1 tick per millisecond|
| configuration.xml -> HHID Thread -> Properties > Settings > Property >Thread > Priority| 1 | 20 |HHID thread priority is lowered to allow the other usb operations at faster rate|
| configuration.xml -> RTT Thread -> Properties > Settings > Property >Thread > Priority| 1 | 25 | RTT thread priority is lowered to allow the USBX HHID data process at the fastest rate possible.|


The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|ux_system_initialize|This API is used to initialize the USBX system |
|ux_host_stack_initialize|This API is used to initialize the USBX device stack  |
|R_USB_Open|This API opens the USB basic driver |
|ux_host_class_hid_keyboard_key_get|This API is used to get the keyboard key and state  |
|ux_host_class_hid_mouse_buttons_get|This API is used to get the mouse buttons pressed  |
|ux_host_class_hid_mouse_position_get|This API is used to get the mouse position in x & y coordinates |
|R_USB_Close|This API is used to close the usb instance  |
|ux_host_stack_uninitialize|This API is used to uninitialize all the host code for USBX |


* Callback:  
apl_host_hid_change_function callback function is called to identify the USB events and respond accordingly. There are only two events available in the EP, which are used to signal to the HHID that USB communication can be started.
1. When event UX_DEVICE_INSERTION is received the USBX_HHID checks for the Host device connected and sets the event flag to perform USBX_HHID operation.
2. When event UX_DEVICE_REMOVAL is received the USBX_HHID will clear the event flag and will wait for USB_INSERTION event.

* Note:        
EP does not support high speed as USBX HHID does not support it.

## Verifying operation ##
1. Import, generate and build the project EP.
2. Flash USBX HHID code on RA board which will acts as host device.
3. Open RTT viewer and connect host device(i.e keyboard or mouse).
4. Once keyboard is connected as host device, press key and check pressed key output on RTT viewer.
5. Remove keyboard and attach mouse, check the x and y coordinates and button pressed of mouse on RTT Viewer.


1. Below images showcases the output on JLinkRTT_Viewer for USBX HHID:
 
 ![usbx_hhid_rtt_log](images/banner_info.jpg "RTT output")
 
 ![Keyboard output snapshot](images/keyboard_press_key.jpg "RTT output")

 ![Mouse x and y-axis snapshot](images/mouse_x,y_axis_data.jpg "RTT output")

 ![Mouse button press snapshot](images/mouse_button_rtt_log.jpg "RTT output")
