# Introduction #

This example project demonstrates the USB OTG (On-The-Go) functionality of the RA MCU, showcasing its ability to dynamically switch between USB host and device roles. The project will perform various USB operations using the USB basic driver. The RA board switches roles depending on the type of USB cable connected (standard or OTG).

In USB Device Mode, the RA board acts as a USB Mass Storage Class (MSC) device using an SD card as block media. When connected to a PC via a USB micro cable, the RA board enumerates as a USB storage device. The PC recognizes the board as external storage, allowing file management operations such as reading and writing data directly on the PC.

In USB Host Mode, the RA board operates as a USB MSC host when a USB OTG cable is connected to its USB port along with USB flash drive. In this setup, the RA board manages connected USB storage devices (e.g., USB flash drive) as peripheral. In this mode, the SD card and USB storage device function as separate file systems. The application also supports file system operations such as listing directories, creating files, and copying files between devices.

The terminal displays real-time updates of USB operations and system status. The available operations include:
* Listing directories and files on USB or SD media.
* Formatting USB disk or SD media.
* Creating files on USB disk or SD media.
* Copying files between USB disk and SD media.

Note: 
* Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer. If supported, it defaults to the serial terminal (UART).
* To use the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please refer to the instructions provided in the Special Topic.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the USB OTG MSC example project, the following resources are needed.

### Hardware Requirements ###
* Supported RA boards: EK-RA8D1
* 1 x RA board
* 2 x Micro USB cables  
* 2 x USB OTG cables  
* 1 x PmodSD module  
* 1 x Full-sized SD card or microSD card with an adapter  
* 11 x Jumper cables (female to female)  
* 1 x Custom USB pinout board

**The schematic design for the custom USB pinout board is shown below:**

The USB ID Pin of the MCU must be connected to the USB ID Pin of the USB Ports to detect OTG cable attachment or detachment.
But in currently, on all RA boards, the USB ID Pin of the USB Port on the board is not connected.

To support the automatic USB mode switching feature when attaching or detaching the USB OTG cable, an external board is required to connect the USB ID Pin of the USB Port to the USB ID Pin of the MCU.

To create the custom USB pinout board, we need to prepare additional hardware:

* 1 x [Micro USB Female to DIP Adapter 2.54mm 5 Pin.](https://www.amazon.com/dp/B09WQTR1H3/ref=twister_B0BVB9YPK7?_encoding=UTF8&th=1)
* 1 x 5-pin header.
* 1 x [USB 2.0 Male to DIP Adapter 2.54mm 4 Pin.](https://www.amazon.com/dp/B09WQXWVFH/ref=twister_B0BVB9YPK7?_encoding=UTF8&th=1)
* 1 x 4-pin header (optional).
* 1 x 1K ohm resistor.

Note: The pinout header in the schematic is a 5-pin header, which is soldered to the Micro USB Female to DIP Adapter 2.54mm 5 Pin.

![The custom USB pinout board schematic](images/usb_otg_custom_board.png "The custom USB pinout board schematic")

### Hardware Connections  ###

#### General Setup ####
* EK-RA8D1: 
    * Connect the RA board's USB ID pin (Pin 411 - J52:P18) to the custom board's USB ID pin using a jumper cable.
    * High-Speed (Default):
        * Connect the RA board's USB HS port (J31 connector) to the custom board's USB Type-A female port using a USB OTG cable.
    * Full-Speed:
        * Connect the RA board's USB FS port (J11 connector) to the custom board's USB Type-A female port using a USB OTG cable.
#### Hardware configuration ####  
* EK-RA8D1: Set the configuration switches (SW1) as below.
    | SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
    |-------------|-------------|--------------|------------|------------|------------|-------------|-----------|
    |     OFF     |     OFF     |      OFF     |     OFF    |     OFF    |     OFF    |     OFF     |    OFF    |

#### PmodSD Setup ####  
Note: Connect PMOD: use short wiring connections (should be shorter than 10cm).
* EK-RA8D1: 
    * 1-Bit Width Connections:
        - P401 (CMD) (J51 Pin7) --> PMOD SD Pin2 (MOSI)
        - P402 (DAT0) (J52 Pin15) --> PMOD SD Pin3 (MISO)
        - P400 (CLK) (J51 Pin10) --> PMOD SD Pin4 (SCLK)
        - GND --> PMOD SD Pin5 (GND)
        - +3V3 (VCC) --> PMOD SD Pin6 (VCC)
        - P406 (CD) (J51 Pin5) --> PMOD SD Pin9 (CD)
        - P700 (WP) (J51 Pin6) --> PMOD SD Pin10 (WP)  

    * 4-Bit Width Additional Connections:
        - P403 (DAT1) (J51 Pin9) --> PMOD SD Pin7 (DAT1)
        - P404 (DAT2) (J57 Pin1) --> PMOD SD Pin8 (DAT2)
        - P405 (DAT3) (J51 Pin8) --> PMOD SD Pin1 (CS)

#### Switching USB Modes ####  
* EK-RA8D1:
    * High-Speed (Default):
        * USB Device Mode:
            - Connect the jumper J17 pins.  
            - Short Pin 2-3 of jumper J7.  
            - Connect the custom USB pinout board's micro USB port to a host PC using a micro USB cable.

        * USB Host Mode:
            - Remove the jumper from J17.  
            - Short Pin 1-2 of jumper J7. 
            - Connect the custom USB pinout board's micro USB port to a USB storage device via a USB OTG cable. 
    * Full-Speed:
        * USB Device Mode:
            - Connect the jumper J15 pins.  
            - Short Pin 2-3 of jumper J12.  
            - Connect the custom USB pinout board's micro USB port to a host PC using a micro USB cable.

        * USB Host Mode:
            - Remove the jumper from J15.  
            - Short Pin 1-2 of jumper J12. 
            - Connect the custom USB pinout board's micro USB port to a USB storage device via a USB OTG cable. 

#### Snapshots of hardware connections:  ####  
**USB Host mode connections:**

![host_mode_connection_snapshot](images/Host_mode_connection.png "Snapshots of host mode connections")

**USB Device mode connections**

![device_mode_connection_snapshot](images/Device_mode_connection.png "Snapshots of device mode connections")

### Software Requirements ###
* Renesas Flexible Software Package (FSP): Version 6.0.0
* e2 studio: Version 2025-04.1
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
* Serial Terminal Console Application: Tera Term or a similar application
* SEGGER J-Link RTT Viewer: Version 8.44a

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
**The high-level block diagram of the system is shown below:**

![High Level Block Diagram](images/usb_otg_msc_hld.png "The High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name        | Usage                                                                                      | Searchable Keyword      |
|--------------------|--------------------------------------------------------------------------------------------|-------------------------|
| FreeRTOS+FAT       | The FreeRTOS+FAT module is used for file system operations in FreeRTOS applications.       | FreeRTOS+FAT            |
| USB PMSC           | The USB PMSC is used to configure a USB Peripheral Mass Storage Class application.         | r_usb_pmsc              |
| USB Basic          | The USB Basic module provides basic USB functionality for communication with devices.      | r_usb_basic             |
| Block Media SD/MMC | The Block Media SD/MMC is used to implement a block media interface for SD/MMC cards.      | rm_block_media_sdmmc    |
| SD/MMC             | The SD/MMC module is used to interface with SD/MMC cards for data storage and retrieval.   | r_sdhi                  |
| External IRQ       | The External IRQ module is used to handle external interrupts from hardware components.    | r_icu                   |

## Module Configuration Notes ##
This section describes FSP configuration properties that are important or different from those selected by default.

|   Module Property Path and Identifier   |   Default Value  |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > BSP > Properties > Settings > Property > Main stack size (bytes) | 0x400 | 0x1000 | Increased stack size to accommodate larger local variables required by the main thread. |
| configuration.xml > BSP > Properties > Settings > Property > Heap size (bytes) | 0 | 0x1000 | Allocated heap size to enable dynamic memory allocation for standard library functions. |
| configuration.xml > Stacks > Threads > USB OTG MSC Thread > Properties > Settings > Property > Common > Memory Allocation > Support Dynamic Allocation | Disabled | Enabled | Enabled heap memory for FreeRTOS. |
| configuration.xml > Stacks > Threads > USB OTG MSC Thread > Properties > Settings > Property > Common > Memory Allocation > Total Heap Size | 1024 | 65536 | Allocated heap size for FreeRTOS. |
| configuration.xml > Stacks > Threads > USB OTG MSC Thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 10240 | Increased stack size for USB OTG MSC Thread. |
| configuration.xml > Stacks > Threads > FAT Thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 10240 | Increased stack size for FAT Thread. |
| configuration.xml > g_basic_pmsc USB (r_usb_basic) > Properties > Settings > Property > Module g_basic_pmsc USB (r_usb_basic) > USB Speed | Full Speed | Hi Speed | Selected USB high-speed mode. |
| configuration.xml > g_basic_pmsc USB (r_usb_basic) > Properties > Settings > Property > Module g_basic_pmsc USB (r_usb_basic) > USB Module Number | USB_IP0 Port | USB_IP1 Port | Configured to use USB module 1. |
| configuration.xml > g_basic_pmsc USB (r_usb_basic) > Properties > Settings > Property > Module g_basic_pmsc USB (r_usb_basic) > USB Callback | NULL | freertos_basic_usb_pmsc_callback | Configured callback function to handle USB PMSC events. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Properties > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Channel | 0 | 1 | Configured SD/MMC to use channel 1. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Properties > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Bus Width | 4 Bits | 4 Bits | Set 4-bit bus width for SD/MMC communication. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Properties > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Card Interrupt Priority | Disabled | Priority 12 | Set card interrupt priority. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Properties > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > DTC Interrupt Priority | Disabled | Priority 12 | Set DTC interrupt priority. |
| configuration.xml > FreeRTOS+FAT Port for RA (rm_freertos_plus_fat) > Properties > Settings > Property > Module FreeRTOS+FAT Port for RA (rm_freertos_plus_fat) > Sector Size (bytes)	| 512 | 512 | Configured sector size. |
| configuration.xml > FreeRTOS+FAT Port for RA (rm_freertos_plus_fat) > Properties > Settings > Property > Module FreeRTOS+FAT Port for RA (rm_freertos_plus_fat) > Cache Size (bytes) | 1024 | 1024 | Set cache size, ensuring it is a multiple of the sector size and at least twice the sector size. |
| configuration.xml > FreeRTOS+FAT Port for RA (rm_freertos_plus_fat) > Properties > Settings > Property > Module FreeRTOS+FAT Port for RA (rm_freertos_plus_fat) > Callback | NULL | freertos_plus_fat_usb_callback | Configured callback function to handle FAT file system events, it will be called when a media is inserted or removed. |
| configuration.xml > g_basic_hmsc USB (r_usb_basic) > Properties > Settings > Property > Module g_basic_hmsc USB (r_usb_basic) > USB Speed | Full Speed | Hi Speed | Selected USB high-speed mode. |
| configuration.xml > g_basic_hmsc USB (r_usb_basic) > Properties > Settings > Property > Module g_basic_hmsc USB (r_usb_basic) > USB Module Number | USB_IP0 Port | USB_IP1 Port | Configured to use USB module 1. |
| configuration.xml > g_external_irq_usb_otg_id External IRQ (r_icu) > Properties > Settings > Property > Module g_external_irq_usb_otg_id External IRQ (r_icu) > Channel | 0 | 4 | Configured external hardware interrupt channel. |
| configuration.xml > g_external_irq_usb_otg_id External IRQ (r_icu) > Properties > Settings > Property > Module g_external_irq_usb_otg_id External IRQ (r_icu) > Trigger | Rising | Both Edges | Configured interrupt trigger to both edges. |
| configuration.xml > g_external_irq_usb_otg_id External IRQ (r_icu) > Properties > Settings > Property > Module g_external_irq_usb_otg_id External IRQ (r_icu) > Digital Filtering	| Disabled | Enabled |  Enabled digital noise filter. |
| configuration.xml > g_external_irq_usb_otg_id External IRQ (r_icu) > Properties > Settings > Property > Module g_external_irq_usb_otg_id External IRQ (r_icu) > Callback	| NULL | external_irq_usb_otg_id_callback |  Configured callback function to handle external interrupts. |

## API Usage ##
The table below lists the FSP provided API used at the application layer in this example project.

| API Name                         | Usage                                                                                     |
|----------------------------------|-------------------------------------------------------------------------------------------|
| FF_Partition                     | This API is used to create partitions on a storage device in the FAT file system.         |
| FF_FormatDisk                    | This API is used to format a storage device with a FAT file system.                       |
| FF_Mount                         | This API is used to mount a FAT file system on a storage device.                          |
| FF_Unmount                       | This API is used to unmount a FAT file system from a storage device.                      |
| FF_FS_Add                        | This API is used to add a FAT file system to the FreeRTOS+FAT driver.                     |
| FF_FS_Remove                     | This API is used to remove a FAT file system from the FreeRTOS+FAT driver.                |
| ff_findfirst                     | This API is used to find the first file or directory in a specified path.                 |
| ff_findnext                      | This API is used to continue searching for files or directories in a path.                |
| ff_fopen                         | This API is used to open a file for reading or writing on a FAT file system.              |
| ff_fclose                        | This API is used to close an open file in the FAT file system.                            |
| ff_fread                         | This API is used to read data from a file in the FAT file system.                         |
| ff_fwrite                        | This API is used to write data to a file in the FAT file system.                          |
| ff_stat                          | This API is used to retrieve information about a file or directory in the FAT file system.|
| FreeRTOS_mktime                  | This API is used to convert time to time_t format for timestamp operations.               |
| xTimerStart                      | This API is used to start a FreeRTOS timer for task management.                           |
| xTimerStartFromISR               | This API is used to start a FreeRTOS timer from an ISR context.                           |
| xTaskNotify                      | This API is used to send notifications to a FreeRTOS task.                                |
| xTaskNotifyFromISR               | This API is used to send notifications to a FreeRTOS task from an ISR context.            |
| xTaskNotifyWait                  | This API is used to wait for task notifications in FreeRTOS.                              |
| RM_FREERTOS_PLUS_FAT_Open        | This API is used to initialize the FreeRTOS+FAT module.                                   |
| RM_FREERTOS_PLUS_FAT_Close       | This API is used to deinitialize the FreeRTOS+FAT module.                                 |
| RM_FREERTOS_PLUS_FAT_MediaInit   | This API is used to initialize storage media for FreeRTOS+FAT.                            |
| RM_FREERTOS_PLUS_FAT_DiskInit    | This API is used to initialize a disk for use with FreeRTOS+FAT.                          |
| RM_FREERTOS_PLUS_FAT_DiskDeinit  | This API is used to deinitialize a disk for use with FreeRTOS+FAT.                        |
| R_USB_Open                       | This API is used to initialize the USB driver.                                            |
| R_USB_Close                      | This API is used to deinitialize the USB driver.                                          |
| R_ICU_ExternalIrqOpen            | This API is used to configure external interrupts.                                        |
| R_ICU_ExternalIrqEnable          | This API is used to enable external interrupts.                                           |

## Verifying Operation ##
1. Import the project into e2 studio, generate project content, build, and flash the firmware onto the RA board using a micro USB cable.  
2. Open a terminal application (e.g., Tera Term or SEGGER RTT Viewer) on the host PC and connect to the RA board’s debug port.  
   * Note: For using the serial terminal:
        * Please ensure that the connection to the SEGGER J-Link RTT Viewer has been terminated.
        * The COM port is provided by the J-Link onboard, with a baud rate of 115200 bps, a data length of 8 bits, no parity check, one stop bit, and no flow control.
3. Switch USB Modes: Follow the "Switching USB Modes" section to configure the board in Device Mode or Host Mode.  
4. Verify USB Device Mode:
    - The RA board enumerates as a USB MSC device, and the PC recognizes it as external storage.
    - The file system of the SD card should be displayed, and users can perform read/write operations using the PC.  
5. Verify USB Host Mode:
    - The RA board manages the connected USB storage device and the SD card.
    - Use the terminal menu to:
        - List directories and files on USB and SD media.
        - Format USB and SD media.
        - Create and write files.
        - Copy files between the USB storage device and SD card. 

### Note: ###
1. To echo back typed content in Tera Term, go to [Setup] -> [Terminal...] and check [Local echo].
2. The configuration parameters of the serial port are:
    - COM port: Provided by the J-Link on-board.
    - Baud rate: 115200 bps
    - Data length: 8 bits
    - Parity: None
    - Stop bit: 1 bit
    - Flow control: None

### The below images showcase the output on the Serial terminal application (Tera Term) ###
**The EP information**

![The EP Information](images/usb_otg_msc_ep_info.png "The EP Information")

* For USB Host Mode:

Change to USB Host Mode:

![Change to USB Host Mode](images/change_to_usb_host_mode.png "Change to USB Host Mode")

USB Disk operation:
1. List USB Disk directory

![USB disk directory list](images/usb_disk_directory_list.png "USB disk directory list")

2. Format USB Disk

![USB disk format](images/usb_disk_format.png "USB disk format")

3. Create and write data to a file on USB Disk

![USB disk create and write data to a file](images/usb_disk_create_and_write_data_to_a_file.png "USB disk create and write data to a file")

4. Copy a file from USB Disk to SD card 

![USB disk copy to SD card](images/usb_disk_copy_to_sdcard.png "USB disk copy to SD card")

SD card operation:

5. List SD card directory list

![SD card directory list](images/sdcard_directory_list.png "SD card directory list")

6. Format SD card

![SD card format](images/sdcard_format.png "SD card format")

7. Create and write data to a file on SD card

![SD card create and write data to a file](images/sdcard_create_and_write_data_to_a_file.png "SD card create and write data to a file")

8. Copy a file from SD card to USB Disk 

![SD card copy to USB disk](images/sdcard_copy_to_usb_disk.png "SD card copy to USB disk")

* For USB Device Mode:

Change to USB Device Mode:

![Change to USB Device Mode](images/change_to_usb_device_mode.png "Change to USB Device Mode")

When the RA board is connected to a host PC via a USB port, it typically appears as a removable storage device in File Explorer on Windows. Here’s how it is displayed on a Windows PC.

![USB device disk](images/usb_device_disk.png "USB device disk")

## Special Topics ##
To change USB speed mode, users can refer to the following configuration:

Change for r_usb_basic stack of both usb hmsc module and usb pmsc module.

* USB High-speed configuration with DMAC support:

![USB_hs_config](images/usb_hs_configuration_1.png "r_usb_basic HS")

![USB_hs_config](images/usb_hs_configuration_2.png "r_dmac HS")

![USB_hs_config](images/usb_hs_configuration_3.png "r_dmac HS")

 * USB Full-speed configuration with DMAC support:

![USB_fs_config](images/usb_fs_configuration_1.png "r_usb_basic FS")

![USB_fs_config](images/usb_fs_configuration_2.png "r_dmac FS")

![USB_fs_config](images/usb_fs_configuration_3.png "r_dmac FS")

Special notes:
* Information can be displayed using either the SEGGER J-Link RTT Viewer or a serial terminal (UART) via J-Link OB VCOM, depending on availability. If J-Link OB VCOM is unsupported, the example project defaults to the SEGGER J-Link RTT Viewer; if supported, it defaults to the serial terminal (UART).
  * To configure display options:
    * **For UART via J-Link OB VCOM**: Define `USE_VIRTUAL_COM=1`
    * **For J-Link RTT Viewer**: Define `USE_VIRTUAL_COM=0`
  * The board supports J-Link OB VCOM: EK-RA8D1.

Set this in **Project Properties** -> **C/C++ Build** -> **Settings** -> **Tool Settings** -> **GNU ARM Cross C Compiler** -> **Preprocessor**.

![USE_VIRTUAL_COM settings](images/use_virtual_com_setting.png "USE_VIRTUAL_COM settings")
