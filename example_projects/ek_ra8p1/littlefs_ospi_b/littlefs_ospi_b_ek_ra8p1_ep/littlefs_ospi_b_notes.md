# Introduction #
This example project demonstrates basic functionalities of LittleFS for external flash (e.g., SPI, OSPI, QSPI) on RA MCU based on FSP. The project will perform various file system operations based on the user's selection from menu options, such as erase entire flash, erase 32 KB flash, format file system, directory management (create, get properties, delete), and file management (create, write, read, delete). The terminal application is used as the user interface. The menu options and system messages (errors and information messages) will be printed on the terminal application during the execution of the project.

In the main menu, the user selects a sub menu such as flash operations menu, directory operations menu, file operations menu.
1. Flash operations.
2. Directory operations.
3. File operations.

In the flash operations menu, the user selects operation to perform.
1. Erase the entire flash.
2. Erase the first 32 KB of the flash.
3. Format the file system.
4. Go back to the main menu.

In the directory operations menu, the user selects operation to perform.
1. Create the directory.
2. Get the directory properties.
3. Delete the directory.
4. Go back to the main menu.

In the file operations menu, the user selects operation to perform.
1. Open the file.
2. Write to the file.
3. Read from the file.
4. Delete the file.
5. Go back to the main menu.

## Required Resources ##
The following resources are needed to build and run the littlefs_ospi_b example project.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.6.0
* e2 studio: Version 2026-07
* SEGGER J-Link RTT Viewer: Version 9.64
* LLVM Embedded Toolchain for ARM: Version 22.1.0
* Terminal Console Application: Tera Term or a similar application

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware ###
* Supported RA board: EK-RA8D1, EK-RA8D2, EK-RA8M2, EK-RA8P1, EK-RA8T2.
	* 1 x Renesas RA board.
	* 1 x USB cable for programming and debugging (USB cable type varies by board model).

### Hardware Connections ###
* Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and debugging.
* For EK-RA8P1: Set the configuration switches (SW4) as below to avoid potential failures.
	| SW4-1 PMOD1_SEL0 |SW4-2 PMOD1_SEL1 | SW4-3 OSPI | SW4-4 ARDUINO | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
	|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|
	| OFF | OFF | OFF | OFF | OFF | OFF | OFF | OFF |

* For EK-RA8D2: Set the configuration switches (SW4) as below to avoid potential failures.
	| SW4-1 PMOD1 | SW4-2 PMOD1 | SW4-3 Octo-SPI | SW4-4 Arduino/mikroBUS | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS | SW4-8 USBHS |
	|:-----------:|:-----------:|:--------------:|:----------------------:|:---------:|:----------:|:-----------:|:-----------:|
	|     OFF     |     OFF     |      OFF       |          OFF           |    OFF    |    OFF     |     OFF     |     OFF     |

* For EK-RA8M2:
	* The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.

* For EK-RA8T2: The user must set the configuration switches (SW6 and SW4) as below to use the on-board debug functionality.
	| SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
	|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:------:|
	|  OFF  |  OFF  |  OFF  |  OFF  |  ON   |  OFF  |  ON   |  ON   |  ON   |  ON    |

	| SW4-1 | SW4-2 | SW4-3 | SW4-4 | SW4-5 | SW4-6 | SW4-7 | SW4-8 |
	|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
	|  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |  OFF  |

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
* [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
* [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
 High level block diagram of the system is as shown below:
 
![littlefs_external_flash_hld](images/littlefs_external_flash_hld.png "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| LittleFS | LittleFS is used for file system operation. | LittleFS |
| LittleFS on SPI Flash | This module provides the hardware port layer for the LittleFS file system on SPI flash memory. | rm_littlefs_spi_flash |
| OSPI Flash | OSPI_B is used to configure flash device and perform write, read, or erase operations on flash device's memory array. | r_ospi_b |
| DMAC | DMAC is used to write data to OSPI_B flash and read back to verification without CPU. | r_dmac |

**Note:**
* Blocking Read/Write/Erase:  
  The LittleFS port blocks on Read/Write/Erase calls until the operation has completed.
* Memory Constraints:  
  The block size defined in the LittleFS configuration must be a multiple of the data flash erase size of the MCU. It must be greater than 104 bytes which is the minimum block size of a LittleFS block. 

## Module Configuration Notes ##
This section describes FSP configuration properties that are important or different from those selected by default.
**Note: The configuration shown in this document is based on the original target board (e.g., EK-RA8P1). Some settings may differ on other supported boards due to hardware differences.**

**Configuration Properties for using LittleFS**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > LittleFS > Properties > Settings > Property > Common > Use Malloc | Enabled | Enabled | Enable to use malloc with LittleFS. |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x1000 | A heap size is required to use malloc with LittleFS. |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x1200 | Set the size of the main program stack. |

**Configuration without using malloc**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > LittleFS > Properties > Settings > Property > Common > Use Malloc | Enabled | Disabled | Disable to not use malloc with LittleFS. |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0 | The main heap size is disabled by default. |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x1200 | Set the size of the main program stack. |

**Enable Data cache in BSP Configuration**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > BSP > Properties > Settings > Property > RA8P1 Family > Cache settings > Data cache | Disabled | Enabled | Enable Data cache to improve performance. |

**Configuration Properties for using LittleFS on SPI Flash**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > LittleFS on SPI Flash (rm_littlefs_spi_flash) > Properties > Settings > Property > Module LittleFS on SPI Flash (rm_littlefs_spi_flash) > Read Size | 1 | 1 | Minimum size of a block read. |
| configuration.xml > Stacks > LittleFS on SPI Flash (rm_littlefs_spi_flash) > Properties > Settings > Property > Module LittleFS on SPI Flash (rm_littlefs_spi_flash) > Program Size | 4 | 4 | Minimum size of a block program. |
| configuration.xml > Stacks > LittleFS on SPI Flash (rm_littlefs_spi_flash) > Properties > Settings > Property > Module LittleFS on SPI Flash (rm_littlefs_spi_flash) > Block Size (bytes) | 4096 | 4096 | Size of an erasable block. |
| configuration.xml > Stacks > LittleFS on SPI Flash (rm_littlefs_spi_flash) > Properties > Settings > Property > Module LittleFS on SPI Flash (rm_littlefs_spi_flash) > Block Cycles | 1024 | 1024 | Number of erase cycles before LittleFS evicts metadata logs and moves the metadata to another block. |
| configuration.xml > Stacks > LittleFS on SPI Flash (rm_littlefs_spi_flash) > Properties > Settings > Property > Module LittleFS on SPI Flash (rm_littlefs_spi_flash) > Cache Size | 64 | 64 | Size of block caches. |
| configuration.xml > Stacks > LittleFS on SPI Flash (rm_littlefs_spi_flash) > Properties > Settings > Property > Module LittleFS on SPI Flash (rm_littlefs_spi_flash) > Lookahead Size | 16 | 16 | Size of the lookahead buffer in bytes. |
| configuration.xml > Stacks > LittleFS on SPI Flash (rm_littlefs_spi_flash) > Properties > Settings > Property > Module LittleFS on SPI Flash (rm_littlefs_spi_flash) > Memory Size (bytes) | 33554432 | 0x20000 | Set the size that LittleFS Memory should be used. |

**Configuration Properties for using OSPI Flash (For LittleFS Instance)**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Common > DMAC Support | Disable | Enable | Enable DMAC support for the OSPI module. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Common > Autocalibration Support | Disable | Enable | Enable DS autocalibration for dual-data-rate modes. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > General > Name | g_ospi | g_ospi_b_littlefs | Module name. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > General > Unit | OSPI_B0 | OSPI_B0 | Specify the OSPI peripheral to use. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > General > Chip Select | CS1 | CS1 | Specify the OSPI chip select line to use. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > General > Write Status Bit | b0 | b0 | Position of the status bit in the flash device register. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > General > Write Enable Bit | b1 | b1 | Position of the write enable bit in the flash device register. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > General > DS Auto-calibration Pattern Address | 0 | 0x9001F000 | Address to the auto-calibration pattern in the target flash memory's address space. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Erase Sizes > Sector Erase | 4096 | 4096 | Size of memory region erased by Sector Erase. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Erase Sizes > Block Erase | 262144 | 65536 | Size of memory region erased by Block Erase. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Protocol Mode | SPI (1S-1S-1S) | Dual data rate OPI (8D-8D-8D) | Select Dual data rate OPI (8D-8D-8D) as the initial protocol mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Frame Format | Standard | xSPI Profile 1.0 | Select the frame format to use with this command set. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Address Length | 3 bytes | 4 bytes | Select number of bytes used to address data in a memory page or row. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Command Code Length | 1 byte | 2 bytes | Select number of bytes used for command codes. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Status Register | No address | 4 bytes | Select number of bytes used for addressing the status register. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Read > Command Code | 0x13 | 0xEE11 | Read command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Read > Dummy Cycles | 0 | 10 | Dummy cycles to use between the address and data phase for Read commands. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Program > Command Code | 0x12 | 0x12ED | Program command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Write Enable > Command Code | 0x06 | 0x06F9 | Write Enable command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Status Read > Command Code | 0x05 | 0x05FA | Status Read command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Status Read > Dummy Cycles | 0 | 4 | Dummy cycles to use between the address and data phase for Status Read commands. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Sector Erase > Command Code | 0x21 | 0x21DE | Sector Erase command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Block Erase > Command Code | 0xDC | 0xDC23 | Block Erase command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Chip Erase > Command Code | 0x60 | 0x609F | Chip Erase command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Protocol Mode | Dual data rate OPI (8D-8D-8D) | Dual data rate OPI (8D-8D-8D) | Select Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Frame Format | xSPI Profile 1.0 | xSPI Profile 1.0  | Select the frame format to use with this command set. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Address Length | 4 bytes | 4 bytes | Select number of bytes used to address data in a memory page or row. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Command Code Length | 2 bytes | 2 bytes | Select number of bytes used for command codes. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Read > Command Code | 0xEEEE | 0xEE11 | Read command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Read > Dummy Cycles | 20 | 10 | Dummy cycles to use between the address and data phase for Read commands. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Program > Command Code | 0x1212 | 0x12ED | Program command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Write Enable > Command Code | 0x0606 | 0x06F9 | Write Enable command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Status Read > Command Code | 0x0505 | 0x05FA | Status Read command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Status Read > Dummy Cycles | 3 | 4 | Dummy cycles to use between the address and data phase for Status Read commands. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Sector Erase > Command Code | 0x2121 | 0x21DE | Sector Erase command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Block Erase > Command Code | 0xDCDC | 0xDC23 | Block Erase command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > LittleFS > g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_littlefs OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Chip Erase > Command Code | 0x6060 | 0x609F | Chip Erase command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |

**Configuration Properties for using OSPI Flash (For Initialize Instance)**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Common > DMAC Support | Disable | Enable | Enable DMAC support for the OSPI module. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Common > Autocalibration Support | Disable | Enable | Enable DS autocalibration for dual-data-rate modes. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > General > Name | g_ospi | g_ospi_b_init | Module name. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > General > Unit | OSPI_B0 | OSPI_B0 | Specify the OSPI peripheral to use. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > General > Chip Select | CS1 | CS1 | Specify the OSPI chip select line to use. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > General > Write Status Bit | b0 | b0 | Position of the status bit in the flash device register. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > General > Write Enable Bit | b1 | b1 | Position of the write enable bit in the flash device register. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > General > DS Auto-calibration Pattern Address | 0 | 0x9001F000 | Address to the auto-calibration pattern in the target flash memory's address space. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Erase Sizes > Sector Erase | 4096 | 4096 | Size of memory region erased by Sector Erase. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Erase Sizes > Block Erase | 262144 | 65536 | Size of memory region erased by Block Erase. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Protocol Mode | SPI (1S-1S-1S) | SPI (1S-1S-1S) | Select SPI (1S-1S-1S) as the initial protocol mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Frame Format | Standard | Standard | Select the frame format to use with this command set. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Address Length | 3 bytes | 4 bytes | Select number of bytes used to address data in a memory page or row. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Command Code Length | 1 byte | 1 byte | Select number of bytes used for command codes. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Status Register | No address | No address | Select number of bytes used for addressing the status register. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Read > Command Code | 0x13 | 0x0C | Read command code of flash device in SPI (1S-1S-1S) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Read > Dummy Cycles | 0 | 8 | Dummy cycles to use between the address and data phase for Read commands. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Program > Command Code | 0x12 | 0x12 | Program command code of flash device in SPI (1S-1S-1S) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Write Enable > Command Code | 0x06 | 0x06 | Write Enable command code of flash device in SPI (1S-1S-1S) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Status Read > Command Code | 0x05 | 0x05 | Status Read command code of flash device in SPI (1S-1S-1S) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Status Read > Dummy Cycles | 0 | 0 | Dummy cycles to use between the address and data phase for Status Read commands. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Sector Erase > Command Code | 0x21 | 0x21 | Sector Erase command code of flash device in SPI (1S-1S-1S) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Block Erase > Command Code | 0xDC | 0xDC | Block Erase command code of flash device in SPI (1S-1S-1S) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > Initial Mode > Chip Erase > Command Code | 0x60 | 0x60 | Chip Erase command code of flash device in SPI (1S-1S-1S) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Protocol Mode | Dual data rate OPI (8D-8D-8D) | Dual data rate OPI (8D-8D-8D) | Select Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Frame Format | xSPI Profile 1.0 | xSPI Profile 1.0  | Select the frame format to use with this command set. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Address Length | 4 bytes | 4 bytes | Select number of bytes used to address data in a memory page or row. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Command Code Length | 2 bytes | 2 bytes | Select number of bytes used for command codes. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Read > Command Code | 0xEEEE | 0xEE11 | Read command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Read > Dummy Cycles | 20 | 10 | Dummy cycles to use between the address and data phase for Read commands. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Program > Command Code | 0x1212 | 0x12ED | Program command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Write Enable > Command Code | 0x0606 | 0x06F9 | Write Enable command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Status Read > Command Code | 0x0505 | 0x05FA | Status Read command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Status Read > Dummy Cycles | 3 | 4 | Dummy cycles to use between the address and data phase for Status Read commands. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Sector Erase > Command Code | 0x2121 | 0x21DE | Sector Erase command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Block Erase > Command Code | 0xDCDC | 0xDC23 | Block Erase command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |
| configuration.xml > Stacks > g_ospi_b_init OSPI Flash (r_ospi_b) > Properties > Settings > Property > Module g_ospi_b_init OSPI Flash (r_ospi_b) > Command Sets > High-speed Mode > Chip Erase > Command Code | 0x6060 | 0x609F | Chip Erase command code of flash device in Dual data rate OPI (8D-8D-8D) mode. |

**Clock Configuration**
|   Configure Clock path   |   Default Value   |   Used Value   |   Reason   |
|--------------------------|-------------------|----------------|------------|
| configuration.xml > Clocks > Clocks Configuration > PLL Src:XTAL > PLL Div /3 | PLL Mul x250.00 | PLL Mul x120.00 | Select multiple for operating clock PLL. |
| configuration.xml > Clocks > Clocks Configuration > PLL Src:XTAL > PLL Div /3 > PLL Mul x120.00 > PLL 960MHz | PLL1R Div /5 | PLL1R Div /8 | Select divisor for operating clock PLL1R. |
| configuration.xml > Clocks > Clocks Configuration > PLL2 Src:XTAL > PLL2 Div /3 | PLL2 Mul x300.00 | PLL2 Mul x133.00 | Select multiple for operating clock PLL2. |
| configuration.xml > Clocks > Clocks Configuration > PLL2 Src:XTAL > PLL2 Div /3 > PLL2 Mul x133.00 > PLL2 1.064GHz | PLL2P Div /4 | PLL2P Div /4 | Select divisor for operating clock PLL2P. |
| configuration.xml > Clocks > Clocks Configuration | OCTACLK Src: PLL1Q | OCTACLK Src: PLL1R | Enable operating clock for OCTA module by PLL1R clock source. |
| configuration.xml > Clocks > Clocks Configuration > OCTACLK Src: PLL1R | OCTACLK Div /1 | OCTACLK Div /1 | Select divisor for operating clock OCTACLK. |
| configuration.xml > Clocks > Clocks Configuration | SCICLK Src: PLL2R | SCICLK Src: PLL1R | Enable operating clock for SCI module by PLL1R clock source. |
| configuration.xml > Clocks > Clocks Configuration > SCICLK Src: PLL1R | SCICLK Div /4 | SCICLK Div /1 | Select divisor for operating clock SCICLK. |

## API Usage ##
The table below lists the FSP provided API used at the application layer in this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| RM_LITTLEFS_SPI_FLASH_Open | This API is used to open the driver and initialize the lower layer driver. |
| RM_LITTLEFS_SPI_FLASH_Close | This API is used to close the lower-level driver. |
| lfs_mount | This API is used to mount a LittleFS. |
| lfs_unmount | This API is used to unmount a LittleFS. |
| lfs_format | This API is used to format a block device with the LittleFS. |
| lfs_mkdir | This API is used to create a directory. |
| lfs_stat | This API is used to find info about a file or directory. |
| lfs_remove | This API is used to remove a file or directory. |
| lfs_file_open | This API is used to open a file (Used malloc). |
| lfs_file_opencfg | This API is used to open a file with extra configuration (Unused malloc). |
| lfs_file_close | This API is used to close a file. |
| lfs_file_write | This API is used to write data to the file. |
| lfs_file_seek | This API is used to change the position of the file. |
| lfs_file_read | This API is used to read data from the file. |
| R_OSPI_B_Open | This API is used to initialize OSPI_B module. |
| R_OSPI_B_SpiProtocolSet | This API is used to change OSPI_B's protocol mode. |
| R_OSPI_B_DirectTransfer | This API is used to write, or read flash device registers. |
| R_OSPI_B_Write | This API is used to write data to flash device memory array. |
| R_OSPI_B_Erase | This API is used to erase a Flash device's sector. |
| R_OSPI_B_StatusGet | This API is used to get the write or erase status of the flash. |
| R_OSPI_B_Close | This API is used to de-initialize OSPI_B module. |
| SCB_InvalidateDCache | This API is used to invalidate DCache. |

## Verifying Operation ##
1. Import the example project (EP).
2. Generate, build the EP.
3. Set up the hardware connection properly before running the EP.
4. Open a serial terminal application (e.g., Tera Term) to view output log, configure the settings as shown below:
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
5. Debug or flash the EP to the RA board.
6. After the main menu is displayed on the terminal application, the user selects options to perform file system management as desired.
	* Type '1' and enter to select flash operations menu.
		* Type '1' and enter to erase the entire flash.
		* Type '2' and enter to erase the first 32 KB of the flash.
		* Type '3' and enter to format the file system.
		* Type '4' and enter to go back to the main menu.
		* **Note: After erasing the entire flash or 32 KB flash, the user must format the file system.**

	* Type '2' and enter to select directory operations menu.
		* Type '1' and enter to create a new directory.
		* Type '2' and enter to get the root directory properties.
		* Type '3' and enter to delete a directory.
		* Type '4' and enter to go back to the main menu.

	* Type '3' and enter to select file operations menu.
		* Type '1' and enter to create an empty file or open an existed file.
		* Type '2' and enter to write a fixed content into a file.
		* Type '3' and enter to read the entire file and display the first 1 KB of its content.
		* Type '4' and enter to delete a file.
		* Type '5' and enter to go back to the main menu.

The images below showcase the output on the serial terminal application (e.g., Tera Term):

The EP information:

![ep_information](images/ep_information.png "EP information")

**The EP menu**

* The main menu:

![main_menu](images/main_menu.png "Main menu")

* The flash operations menu:

![flash_menu](images/flash_menu.png "The flash operations menu")

* The directory operations menu:

![dir_menu](images/dir_menu.png "The directory operations menu")

* The file operations menu:

![file_menu](images/file_menu.png "The file operations menu")

**Flash operations**

* Erase the entire flash:

![flash_erase_entire](images/flash_erase_entire.png "Erase the entire flash")

* Erase the first 32 KB of the flash:

![flash_erase_32kb](images/flash_erase_32kb.png "Erase the first 32 KB of the flash")

* Format the file system:

![flash_format_file_system](images/flash_format_file_system.png "Format the file system")

* Go back to the main menu:

![flash_back_to_main_menu](images/flash_back_to_main_menu.png "Go back to the main menu")

**Directory operations**

* Create directory:

![dir_create](images/dir_create.png "Create directory")

* Get directory properties:

![dir_get_properties](images/dir_get_properties.png "Get directory properties")

* Delete directory:

![dir_delete](images/dir_delete.png "Delete directory")

* Go back to the main menu:

![dir_back_to_main_menu](images/dir_back_to_main_menu.png "Go back to the main menu")

**File operations**

* Create an empty file or open an existed file:

![file_open](images/file_open.png "Create an empty file or open an existed file")

* Write to the file:

![file_write](images/file_write.png "Write to the file")

* Read from the file:

![file_read](images/file_read.png "Read from the file")

* Delete file:

![file_delete](images/file_delete.png "Delete file")

* Go back to the main menu:

![file_back_to_main_menu](images/file_back_to_main_menu.png "Go back to the main menu")
