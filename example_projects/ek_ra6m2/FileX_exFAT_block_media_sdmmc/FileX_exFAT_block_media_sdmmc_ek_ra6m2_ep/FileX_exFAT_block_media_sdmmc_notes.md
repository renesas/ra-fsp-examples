# Introduction #

The example project shows the operation of the FileX file system on block media via the SDHI driver on the RA MCU. The FileX file system with exFAT format enabled, the project will perform various file system operations based on the user's selection from the menu options, such as media management (open, get property, format, close), directory management (create, get property, delete), and file management (create, write, read, delete).

The J-Link RTT Viewer is used as the user interface. Menu options and system messages (errors and information messages) will be printed on J-link RTT Viewer during the execution of the project.    

In Main menu, based on RTT input, user selects sub menu such as media menu, directory menu, or file menu.
1. Media operation menu.
2. Directory operation menu.
3. File operation menu.

In Media menu, based on RTT input, user selects operations to perform.
1. Open Media.
2. Get Media Property.
3. Format Media.
4. Close Media.
5. Go back to main menu.

In Directory menu, based on RTT input, user selects operations to perform.
1. Create Directory.
2. Get Directory property.
3. Delete Directory.
4. Go back to main menu.

In file menu, based on RTT input, user selects operations to perform.
1. Create Empty File.
2. Write To File.
3. Read From File.
3. Delete File.
4. Go back to main menu.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ## 
To build and run the FileX_exFAT_block_media_sdmmc example project, the following resources are needed.

### Hardware ###
* 1 x Renesas RA boards
* 1 x PmodSD module
* 1 x Full-sized SD card or Micro SD card with full-sized adapter
* 1 x Micro USB cable
* 12 x Jumper cable (both ends female)

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.3.0
* e2 studio: Version 2025-12
* SEGGER J-Link RTT Viewer: Version 8.92
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
 High level block diagram of the system is as shown below:
 
![FileX_exFAT](images/FileX_exFAT_hld.png "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| Azure RTOS FileX on Block Media | Azure RTOS FileX on Block Media is used to configure the FileX system and media properties. | FileX |
| SD/MMC | SD/MMC is used to communicate directly with the SD card. | r_sdhi |

## Module Configuration Notes ##
This section describes FSP configuration properties that are important or different from those selected by default.

**Configuration Properties for using Azure RTOS FileX on Block Media**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Common >Extra Features > Enable exFAT | Disabled (default) | Enabled | Enables exFAT format support in FileX. |
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Module g_fx_media Azure RTOS FileX on Block Media > Volume Name | Volume 1 | RA SDMMC | Set the volume label for the media. |
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Module g_fx_media Azure RTOS FileX on Block Media > Number of FATs | 1 | 1 | Set the number of FATs in the media to 1. |
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Module g_fx_media Azure RTOS FileX on Block Media > Directory Entries | 256 | 256 | Set the number of directory entries in the root directory. |
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Module g_fx_media Azure RTOS FileX on Block Media > Hidden Sectors | 0 | 0 | set the number of sectors hidden. |
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Module g_fx_media Azure RTOS FileX on Block Media > Total Sectors | 65536 | 3751936 | Set the total number of sectors in the media. |
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Module g_fx_media Azure RTOS FileX on Block Media > Bytes per Sector | 512 | 512 | Set the number of bytes per sector. |
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Module g_fx_media Azure RTOS FileX on Block Media > Sectors per Cluster | 1 | 64 | Set the number of sectors in each cluster. |
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Module g_fx_media Azure RTOS FileX on Block Media > Boundary Unit (exFAT only) | 128 | 128 | Set the physical data area alignment size, in number of sectors. |
| configuration.xml > g_fx_media Azure RTOS FileX on Block Media > Settings > Property > Module g_fx_media Azure RTOS FileX on Block Media > Working media memory size | 512 | 512 | Set the memory allocated for the file system. |

**Configuration Properties for using SD/MMC**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Channel | 0 | 0 | Select SD/MMC channel 0. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Bus Width | 4 Bits | 4 Bits | Select 4 bits bus width. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Block Size | 512 | 512 | Must be 512 for SD cards. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Card Detection | CD Pin | CD Pin | Select card detection using the CD pin. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Write Protection | WP Pin | WP Pin | Select use the write protect pin. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Access Interrupt Priority | Priority 12 | Priority 12 | Select the access interrupt priority. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Card Interrupt Priority | Disabled | Priority 12 | Select the card interrupt priority. |

## API Usage ##
The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| tx_event_flags_set | This API is used to set or clear event flags in an event flags group, depending upon the specified set option. |
| tx_event_flags_get | This API is used to retrieve event flags from the specified event flags group. |
| tx_thread_sleep | This API is used to cause the calling thread to suspend for the specified number of timer ticks. |
| fx_system_initialize | This API is used to initialize all the major FileX data structures. |
| fx_system_date_set | This API is used to set the system date as specified. |
| fx_system_time_set | This API is used to set the system time as specified. |
| fx_system_date_get | This API is used to retrieve the current system date. |
| fx_system_time_get | This API is used to retrieve the current system time. |
| fx_media_open | This API is used to open media for file access using the supplied I/O driver. |
| fx_media_extended_space_available | This API is used to retrieve the number of bytes available in the media. It is designed for exFAT. |
| fx_media_exFAT_format | This API is used to format the supplied media in an exFAT-compatible manner based on the supplied parameters. |
| fx_media_volume_get | This API is used to retrieve the volume name of the previously opened media. |
| fx_media_flush | This API is used to flush all cached sectors and directory entries of any modified files to the physical media. |
| fx_media_close | This API is used to close the specified media. |
| fx_directory_create | This API is used to create a subdirectory in the current default directory or in the path supplied in the directory name. |
| fx_directory_first_full_entry_find | This API is used to retrieve the first entry name in the default directory with full information. |
| fx_directory_next_full_entry_find | This API is used to retrieve the next entry name in the default directory with full information. |
| fx_directory_delete | This API is used to delete the specified directory. |
| fx_directory_information_get | This API is used to get a directory's entry information. |
| fx_file_create | This API is used to create the specified file in the default directory or in the directory path supplied with the file name. |
| fx_file_open | This API is used to open the specified file for either reading or writing. |
| fx_file_extended_truncate | This API is used to truncate the size of the file to the specified size. It is designed for exFAT. |
| fx_file_write | This API is used to write bytes from the specified buffer, starting at the file's current position. |
| fx_file_date_time_set | This API is used to set the date and time of the specified file. |
| fx_file_extended_seek | This API is used to position the internal file read/write pointer at the specified byte offset. It is designed for exFAT. |
| fx_file_read | This API is used to read bytes from the file and store them in the supplied buffer. |
| fx_file_delete | This API is used to delete the specified file. |
| fx_file_close | This API is used to close the specified file. |
| RM_FILEX_BLOCK_MEDIA_Open | This API is used to initialize callbacks and configure the FileX Block Media interface. |
| RM_FILEX_BLOCK_MEDIA_BlockDriver | This API is used to access block media device functions such as open, close, read, write, and control. |
| RM_BLOCK_MEDIA_SDMMC_InfoGet | This API is used to retrieve SD/MMC module information. |

## Verifying Operation ##
1. Import, generate, build the EP project.
2. Connect the RA MCU debug port to the host PC via a micro USB cable. 
3. Connect the PmodSD module to the RA board.
4. Insert the SD card into the PmodSD module.
5. Debug or flash the EP project to the RA board.
6. Open the J-Link RTT Viewer.
7. After the main menu is displayed on the RTT Viewer, the user selects options to perform file system management as desired. Note that the media must be opened before performing other operations.

### Important ###

1. SD card must be in exFAT format before verify the EP.
2. In case the SD is not in exFAT format, users can format the SD card in exFAT format using this EP using the following procedure: Open Media -> Format Media -> Close Media -> Open Media.
3. The Media must be opened before performing other operations.
	
### Menu details ###
1. In MAIN MENU: The user input option from the RTT Viewer will go to the next menu as follows:
    * Type '1' and enter to go to MEDIA MENU.
    * Type '2' and enter to go to DIRECTORY MENU.
    * Type '3' and enter to go to FILE MENU.
2. In MEDIA MENU: The user input options will perform media operations as follows:
    * Type '1' and enter to open the media.
    * Type '2' and enter to get media properties.
    * Type '3' and enter to format the media in exFAT format.
    * Type '4' and enter to close the media.
    * Type '5' and enter to go back MAIN MENU.
3. In DIRECTORY MENU: The user input options will perform directory operations as follows:
    * Type '1' and enter to create a new directory.
    * Type '2' and enter to get root directory properties.
    * Type '3' and enter to delete a directory.
    * Type '4' and enter to go back MAIN MENU.
4. In FILE MENU: The user input options will perform file operations as follows:
    * Type '1' and enter to create an empty file.
    * Type '2' and enter to write more than 4 GB of fixed content into a file.
    * Type '3' and enter to read the first 1 KB of content in a file.
    * Type '4' and enter to delete a file.
    * Type '5' and enter to go back MAIN MENU.

### The below images showcase the output on J Link RTT Viewer ###
The EP information :

![Filex_exFAT_ep_info](images/ep_info.png "EP Information")

Media Operation:

![Filex_exFAT_media_open](images/media_open.png "Media Open")
![Filex_exFAT_media_get_property](images/media_get_property.png "Media Get Property")
![Filex_exFAT_media_format](images/media_format.png "Media Format")
![Filex_exFAT_media_close](images/media_close.png "Media Close")

Directory Operation:

![Filex_exFAT_directory_create](images/directory_create.png "Directory Create")
![Filex_exFAT_directory_get_property](images/directory_get_property.png "Directory Get Property")
![Filex_exFAT_directory_delete](images/directory_delete.png "Directory Delete")

File Operation:

![Filex_exFAT_file_create](images/file_create.png "File Create")
![Filex_exFAT_file_write](images/file_write.png "File Write")
![Filex_exFAT_file_read](images/file_read.png "File Read")
![Filex_exFAT_file_delete](images/file_delete.png "File Delete")
