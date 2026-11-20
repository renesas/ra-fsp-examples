# Introduction #
The example project demonstrates the basic functionalities of a USBX Device Firmware Update (DFU) driver on Renesas RA MCUs based on Renesas FSP using Azure RTOS. The RA board is connected to the host PC, where it acts as a peripheral device. Prepare a file (.txt, .srec, .xml, .hex, etc.) to download to the RA MCU which supports both CDC and DFU interfaces. In a real-world application, new device firmware can be downloaded. In this example project, a set of data is used to demonstrate the download functionality. The DFU example application works in four steps:
   * First, the system is initialized with support for the Interface Association Descriptor. The CDC and DFU classes are then initialized with separate interfaces.
   * Second, the device operates in Run-Time mode with the CDC and DFU interfaces. An application on the host (Windows PC) sends a DFU request. Upon reception of this command, the system un-initializes the Run-Time configuration and initializes the device in DFU mode.
   * Third, the device disappears from the bus for a short time (enough for the host and the device to detect a reset sequence). Upon restarting, the device is exclusively in DFU mode and downloads the data sent from the host.
   * Finally, when the new data has been downloaded, the host application resets the device, and upon re-enumeration, the device reverts to its normal operation with the new firmware. The RA MCU displays the USB enumeration status on the terminal.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of the operation.

## Required Resources ##
To build and run the USBX Device Firmware Update (DFU) example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.6.0
* e2 studio: Version 2026-07
* LLVM Embedded Toolchain for ARM: Version 22.1.0
* Terminal Console Application: Tera Term or a similar application
* Zadig.exe - v2.0.1.160 (https://sourceforge.net/projects/libwdi/files/zadig/)
* dfu-util-0.11-binaries.tar.xz (https://sourceforge.net/projects/dfu-util/files/)

Note: Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware ###
* Supported RA boards: EK-RA4M2, EK-RA6M1, EK-RA6M3, EK-RA6M5, EK-RA8D1, EK-RA8M1, MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1, MCK-RA8T2, EK-RA8D2, EK-RA8M2.
   * 1 x Renesas RA board.
	* 1 x USB cable for programming and debugging (USB cable type varies by board model).
	* 1 x USB cable for USB communication between the RA board and the host PC (USB cable type varies by board model).
	* 1 x Host PC with at least 2 USB ports (1 for debug, 1 for COM port).

### Hardware Connections: ###
* Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and debugging.
* For EK-RA4M2, EK-RA6M3, EK-RA6M5, EK-RA8D1, EK-RA8M1 (Full-Speed):
   * Jumper J12: Connect pins 2-3.
   * Connect jumper J15 pins.
   * Connect USB Full-Speed port (J11) of the RA board to USB port of the host PC using the appropriate USB cable.

* For EK-RA6M1 (Full-Speed):
   * Connect USB Full-Speed port (J9) of the RA board to USB port of the host PC using the appropriate USB cable.

* For MCK-RA8T1 (Full-Speed):
   * Jumper JP9: Connect pins 2-3.
   * Connect jumper JP10 pins.
   * Connect USB Full-Speed port (CN14) of the RA board to USB port of the host PC using the appropriate USB cable.

* For EK-RA4L1 (Full-Speed):
   * The user must turn ON S4-4 to select USB device mode.
   * Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
   * Connect USB Full-Speed port (J11) of the RA board to USB port of the host PC using the appropriate USB cable.

* For EK-RA8E2, EK-RA8P1, EK-RA8D2, EK-RA8M2 (Full-Speed):
   * Note: For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
	* Connect USB Full-Speed port (J11) of the RA board to USB port of the host PC using the appropriate USB cable.

* For MCK-RA8T2 (Full-Speed):
   * Jumper JP6: Connect pins 2-3.
   * Connect Jumper JP7 pins.
   * Connect USB Full-Speed port (CN18) of the RA board to USB port of the host PC using the appropriate USB cable.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
* [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
* [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #
## System Level Block Diagram ##
![usbx_dfu_hld](images/usbx_dfu_hld.jpg "USBX DFU Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
| :---------: | :---------------: | :------------: |
| USBX DFU | This module provides USBX DFU class support on RA device. | dfu |
| USBX PCDC | USBX PCDC is used to communicate with Host machine to perform write/read operations. | rm_usbx_port |
| USB Basic Driver | USB driver is required for hardware configuration on RA MCU. | r_usb_basic |

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default.

**Note: The configuration shown in this document is based on the original target board (e.g., EK-RA8P1). Some settings may differ on other supported boards due to hardware differences.**

**Common Configuration Properties**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x400 | Heap size is required for standard library functions to be used. |
| configuration.xml > Stacks > Threads > USBX PERI DFU Thread > Properties > Settings > Property > Common > Timer > Timer Ticks Per Second | 100 | 1000 |The default ticks should be 1000 indicating 1 tick per millisecond. |
| configuration.xml > Stacks > Threads > USBX PERI DFU Thread > Properties > Settings > Property > Thread > Priority | 1 | 15 | USBX PERI DFU Thread priority is lowered to allow the other USB operations to be serviced at a faster rate. |
| configuration.xml > Stacks > Threads > USBX PERI CDC DFU Thread > Properties > Settings > Property > Thread > Priority | 1 | 15 | USBX PERI CDC DFU Thread priority is lowered to allow the other USB operations to be serviced at a faster rate. |
| configuration.xml > Stacks > Threads > Terminal Thread > Properties > Settings > Property > Thread > Priority | 1 | 16 | Terminal thread priority is lowered to allow the internal thread taking priority. |
| configuration.xml > Stacks > Threads > Terminal Thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 1024 | The default stack size is sufficient for the Terminal Thread operations. |

**Configuration Properties for USB Speed as Full-Speed**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > Stacks > Threads > USBX PERI DFU Thread > USBX PERI DFU Stacks > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Module g_basic0 USB (r_usb_basic) > USB Speed | Full Speed | Full Speed | USB Speed is configured as **Full-Speed**. |
| configuration.xml > Stacks > Threads > USBX PERI DFU Thread > USBX PERI DFU Stacks > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Module g_basic0 USB (r_usb_basic) > USB Module Number | USB_IP0 Port | USB_IP0 Port | This property is used to specify USB module number to be used as per configured USB speed. |
| configuration.xml > Stacks > Threads > USBX PERI CDC DFU Thread > USBX PERI CDC_DFU Stacks > g_basic1 USB (r_usb_basic) > Properties > Settings > Property > Module g_basic1 USB (r_usb_basic) > USB Speed | Full Speed | Full Speed | USB Speed is configured as **Full-Speed**. |
| configuration.xml > Stacks > Threads > USBX PERI CDC DFU Thread > USBX PERI CDC_DFU Stacks > g_basic1 USB (r_usb_basic) > Properties > Settings > Property > Module g_basic1 USB (r_usb_basic) > USB Module Number | USB_IP0 Port | USB_IP0 Port | This property is used to specify USB module number to be used as per configured USB speed. |

## API Usage ##
The table below lists the FSP provided APIs used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_USB_Open | This API is used to open the USB basic driver. |
| R_USB_Close | This API is used to close the USB basic driver. |
| ux_system_initialize | This API is used to initialize the USBX system. |
| ux_system_uninitialize | This API is used to uninitialize the various control data structures for the USBX system. |
| ux_device_stack_initialize | This API is used to initialize the USBX device stack. |
| ux_device_stack_uninitialize | This API is used to uninitialize the generic portion of the device side of USBX. |
| ux_device_stack_class_register | This API is used to register a device class with the device stack. |
| ux_device_stack_class_unregister | This API is used to unregister a device class from the device stack. |
| ux_device_class_dfu_entry | This API is the entry point of the DFU class. |
| ux_device_class_cdc_acm_read | This API is used to read from the CDC class. |
| ux_device_class_cdc_acm_write | This API is used to write to the CDC class. |
| ux_device_class_cdc_acm_entry | This API is the entry point of the CDC ACM class. |
| _ux_utility_memory_compare | This API is used to compare two memory blocks. |

## Verifying Operations ##
* This EP supports **only USB Full-Speed mode**.
* Two USB devices are enumerated at different stages of this application: **CDC USB Demonstration in Run-Time mode** and **DFU USB Demonstration in DFU mode**.
* Before proceeding, the user must **confirm whether the drivers for both devices are installed**:
   Open **Device Manager** → **View** → **Show hidden devices**, and check that the following entries appear under Universal Serial Bus devices:
   * **CDC USB Demonstration**
   * **DFU USB Demonstration**

   ![device_manager](images/device_manager.jpg "CDC USB Demonstration and DFU USB Demonstration driver installed")

* In case the host PC does not have drivers installed for any of the devices mentioned above, the user needs to perform the steps in the **Driver Installation Procedure** section to run this EP.

### The following procedure describes how to download new data and reset the MCU:
1. Import, generate, and build the USBX DFU project.

2. Set up hardware connection properly as mentioned in [Hardware Connections](#hardware-connections) section.

3. On the host PC, prepare a file in format of .txt, .srec, .xml, .hex, etc. to download to the MCU. File download to MCU: [data.txt](./data.txt).

**Note:** Prepare a file (e.g., .txt, .hex, .xml, .srec etc.) to download to the MCU. The file size must be smaller than 30 KB by default. The example test file data.txt (29 KB) is included in the EP folder. To download a larger file, update the **BUFFER_SIZE macro in usbx_dfu_ep.h** according to the available memory and the selected verification scheme. For EK-RA4L1, the file size must be smaller than 4 KB due to the limited RAM size of this board. The example test file data.txt (4 KB) is included in the EP folder.

4. Open a serial terminal application (e.g., Tera Term) to view output logs, configure the settings as shown below:
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

   ![ep_information](images/ep_information.jpg "EP Information")

5. Run the command **.\dfu-util -d 0000:0002 -D data.txt** to download data.txt file to the MCU.

   ![usbx_dfu_cmd_output](images/usbx_dfu_cmd_output.jpg "USBX DFU Command Prompt Output")

   **Note:** 
   - By default, the CDC USB Demonstration enumerates in Device Manager with Vendor ID: 0000 & Product ID: 0002, the **-d 0000:0002** argument must be used in the command above to ensure data.txt is downloaded to the MCU.

   ![usbx_cdc_hardware_ids](images/usbx_cdc_hardware_ids.jpg "USBX CDC Hardware Ids")

   - If you modify the Vendor ID or Product ID in the *rm_usbx_dfu_descriptor.c*, you must also update the command accordingly: **.\dfu-util -d `<VendorID>`:`<ProductID>` -D data.txt**
	
6. EP operation:

   ![ep_operation_1](images/ep_operation_1.jpg "EP Operation")
	
   Run until the maximum number of bytes has been written, as specified by the prepared file:
	
   ![ep_operation_2](images/ep_operation_2.jpg "EP Operation")

## Driver Installation Procedure ##
Before installing drivers, the user needs to perform the following steps:

1. Connect RA MCU board to the PC as mentioned in Hardware Connections section.

2. Generate Project Content, build and download the USBX DFU project to the RA board.

### A. CDC USB Demonstration Driver Installation ###

1. Launch the **Zadig** application on the host PC.

2. In Zadig, go to **Options → List All Devices**.

   ![Zadig Device List](images/zadig_1.jpg "Showing all devices in Zadig")
   
   **Note:** There are two interfaces of the same name in the Zadig application, i.e., **CDC USB Demonstration (Interface 0)** and **CDC USB Demonstration (Interface 2)**.  
   The USB device driver installed in interface 0 is a COM port. No changes should be made to this.

3. Select **CDC USB Demonstration (Interface 2)**.
   
   Install driver for CDC USB Demonstration (Interface 2):

   ![usbx_dfu_driver_interface_2_installation](images/zadig_2.jpg "USBX DFU Driver Interface 2 Installation")

   The CDC USB Demonstration (Interface 2) driver is already installed as shown below:

   ![usbx_dfu_driver_interface_2_installation](images/zadig_3.jpg "USBX DFU Driver Interface 2 Successful Installation")
		
   **Note:** The PC may need to be restarted to finish the driver installation.

### B. DFU USB Demonstration Driver Installation ###

1. Run the command **.\dfu-util -d 0000:0002 -D data.txt** to download data.txt file to the MCU. This also switches the device from CDC class to DFU class.  
   An error will occur on the Command Prompt — this is expected behavior because the driver is not yet installed.
   
   ![usbx_dfu_cmd_output](images/usbx_dfu_cmd_output_failed.jpg "USBX DFU command prompt output")

   **Note:** Do not reset, disconnect, or power off the board after this step.  
   The device is now operating under the DFU class, and any interruption may cause it to revert back to CDC class before the driver is installed.

2. Launch Zadig and Select **DFU USB Demonstration**
   
   Install the driver for DFU USB Demonstration:
   
   ![usbx_dfu_driver_installation](images/zadig_4.jpg "USBX DFU Driver Installation")

   The DFU USB Demonstration driver is already installed as shown below:

   ![usbx_dfu_driver_installation](images/zadig_5.jpg "USBX DFU Driver Successful Installation")

   **Note:** The user needs to reset the board after this step.

## Special Topics 
#### Download Links
1. Zadig Application download link: [Zadig.exe (v2.0.1.160)](https://sourceforge.net/projects/libwdi/files/zadig/)
2. dfu-util download link: [dfu-util-0.11-binaries.tar.xz](https://sourceforge.net/projects/dfu-util/files/)
