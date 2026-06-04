# Introduction #
This example project demonstrates Azure NetX Duo's networking capabilities using the Ethernet physical layer of the RA MCU. It leverages the NetX Duo network stack to execute various network operations, such as sending and receiving data over a network. The RA board will automatically obtain an IP address from the network using a DHCP (Dynamic Host Configuration Protocol) client service.

Once the RA board successfully acquires the IP address, the user can interact with the system by selecting options from a menu, which is accessible via the RTT Viewer or a terminal application running on the host PC. After the RA board retrieves its IP address from the router, it will display the network configuration details, along with the available menu options, in the terminal application. This enables the user to explore different features and operations supported by the network stack.

The available operations include:
* TCP Client Service: Connect to a server, send a request, wait for a response from the server, and disconnect from the server.
* UDP Client Service: Send a request and wait for a response from the server.
* DNS Client Service: Retrieves both the A record (forward lookup) and PTR record (reverse lookup), enabling the system to resolve domain names into IP addresses and vice versa.
* Web HTTP Server: Handle HTTP requests from the browser, including requesting the homepage, retrieving board network configuration, controlling onboard LEDs, and requesting the status of onboard LEDs.

The menu options:
* Type `1 <x.x.x.x>:<port>` to perform TCP client operation.
* Type `2 <x.x.x.x>:<port>` to perform UDP client operation.
* Type `3 <x.x.x.x>` to get the PTR record for the specified address. Type `3 <domain>` to get the A record for the specified domain.
* Type `4` to start the web HTTP server.

Notes: 
* To display information, the user can select between the SEGGER J-Link RTT Viewer and the serial terminal (UART) with J-Link OB VCOM. 
* By default, the EP information is printed to the host PC via the serial terminal.
* RA boards supported for J-Link OB VCOM: EK-RA8D1, EK-RA8M1, EK-RA8P1.
* To use the SEGGER J-Link RTT Viewer instead of the serial terminal, please follow the instructions in the [Verifying Operation](#verifying-operation) section.

## Required Resources ## 
To build and run the Getting_started_with_Azure_NetX example project, the following resources are needed.

### Software Requirements ###
* Renesas Flexible Software Package (FSP): Version 6.5.0
* e2 studio: Version 2026-04.2
* LLVM Embedded Toolchain for ARM: Version 21.1.1
* Terminal Console Application: Tera Term or a similar application
* SEGGER J-Link RTT Viewer: Version 9.42
* Socket Application (e.g., sokit version 1.3)
* Browser Application (e.g., Microsoft Edge, Google Chrome)

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware Requirements ###
* Supported RA boards: EK-RA8D1, EK-RA8M1, EK-RA8P1.
    * 1 x Renesas RA board.
    * 1 x Ethernet router with an internet connection.
    * 2 x Ethernet cables to connect the RA board and the host PC to the router.
    * 1 x Type-C USB cable for programming and debugging.

### Hardware Connections ###
* Power on the Router: Ensure the router is powered on and properly connected to the internet. Verify that the router’s LAN ports are active and available for connections.
* Connect the RA board to the Router: Take an Ethernet cable and connect one end to the Ethernet port on the RA board, and the other end to one of the router's available LAN ports. This connection allows the RA board to communicate with the network and obtain an IP address via DHCP.
* Connect the host PC to the Router: Using a second Ethernet cable, connect the host PC to another LAN port on the same router. This step ensures that both the RA board and the host PC are on the same network, enabling communication between them for debugging and menu operations.
* Connect the RA board to the host PC: Using a USB cable, connect the RA board to the host PC. This connection is necessary for programming the RA board, enabling debugging, and displaying runtime information in the terminal or RTT Viewer on the host PC.
* By following these steps, you'll establish proper hardware connections between the RA board, the router, and the host PC, ensuring smooth network communication and debugging capabilities.
* For EK-RA8D1: Set the configuration switches (SW1) as below.
    | SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
    |-------------|-------------|--------------|------------|------------|------------|-------------|-----------|
    |     OFF     |     OFF     |      OFF     |     OFF    |     ON     |     OFF    |     OFF     |    OFF    |  
* For EK-RA8M1: Remove jumper J61 to enable Ethernet B.
* For EK-RA8P1: Set the configuration switches (SW4) as below to avoid potential failures.

    | SW4-1 PMOD1_SEL0 |SW4-2 PMOD1_SEL1 | SW4-3 OSPI | SW4-4 ARDUINO | SW4-5 I3C | SW4-6 MIPI | SW4-7 USBFS_ROLE | SW4-8 USBHS_ROLE |
    |:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|
    | OFF | OFF | OFF | OFF | OFF | OFF | OFF | OFF |

**Note: Sokit (Socket Application) setup on PC as TCP, UDP server**
1. Download sokit tool form https://www.softpedia.com/get/Network-Tools/Misc-Networking-Tools/sokit.shtml
2. Once downloaded, extract the contents and start sokit.exe.
3. Once started, in the 'Server' tab configure the 'Network Setup'.
4. Input the server's IP address and the port number
    * For TCP: In the 'TCP Addr' field, enter the PC's IPv4 address. And in 'Port' field, enter the port number.
    * For UDP: In the 'UDP Addr' field, enter the PC's IPv4 address. And in 'Port' field, enter the port number.
    
Note: Please provide an unused port number. It is recommended to give port number greater than 10,000.

5. Start the TCP, UDP server on PC.
    * For TCP: Click the 'TCP Listen' button. This would start the TCP server on PC.
    * For UDP: Click the 'UDP Listen' button. This would start the UDP server on PC.
6. The connection log will display on the 'Output window' on the PC.
    * For TCP: The 'Output' window would display the message 'hh:mm:ss MSG start TCP server successfully!'.
    * For UDP: The 'Output' window would display the message 'hh:mm:ss MSG start UDP server successfully!'.
7. Now wait for the EP to connect to server.
8. Once the EP connects to the server, the 'Connections' window would display the IP address and port number of the client.
9. There are three user input buffers provided for sending messages i.e. 'Buf 1', 'Buf 2' and 'Buf 3'.
10. In one buffer, enter 'This is a response sent from the TCP server'.
11. While sending, select the IP address of the client i.e. the MCU from the 'Connections' window. 
12. And then click 'Send' button of the appropriate buffer to send message.
13. Messages received from the client will be displayed on the 'Output' window.
14. The 'Output' window and console display messages upon each transaction.
15. To stop the server
    * For TCP: Click the 'TCP Listen' button again. This would stop the server.
    * For UDP: Click the 'UDP Listen' button again. This would stop the server.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
**The high-level block diagram of the system is shown below:**
![High Level Block Diagram](images/Getting_started_with_Azure_NetX_hld.png "The High Level Block Diagram")

**The Thread interaction flow diagram of the system is shown below:**
![Thread interaction flow diagram](images/Getting_started_with_Azure_NetX_thread_interaction_flow.png "The Thread interaction flow diagram")

**The web server homepage of the system is shown below:**
![Web server homepage](images/index_page.png "The Web server homepage")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| NetX Duo Web HTTP/HTTPS Server | The NetX Duo Web HTTP/HTTPS Server is used to access the NetX Duo Web HTTP/HTTPS Server library. | NetX Duo Web HTTP/HTTPS Server |
| NetX Duo DHCP IPv4 Client | The NetX Duo DHCP IPv4 Client is used to obtain IP addresses and network parameters, automating the process of configuring devices on IP networks. | NetX Duo DHCP |
| NetX Duo IP Instance | The NetX Duo IP instance is used for IP communication. The HTTPS Client uses IPv4. | NetX Duo IP |
| NetX Duo Packet Pool | The NetX Duo Packet Pool is used to send and receive data packets over a TCP network. | NetX Duo Packet Pool |
| NetX Duo Ethernet Driver | An Ethernet connection is required as a physical connection to the network. | NetX Duo Ethernet Driver |
| FileX on Block Media | The FileX on Block Media is used to configure the FileX system and media properties. | FileX on Block Media |
| Block Media SPI Flash | The Block Media SPI Flash is middleware that implements a block media interface using external SPI flash as a media area. | rm_block_media_spi |
| OSPI Flash | The OSPI Flash is used to communicate directly with the external flash chip. | r_ospi_b |

## Module Configuration Notes ##
This section describes FSP configuration properties that are important or different from those selected by default.

|   Module Property Path and Identifier   |   Default Value  |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x2000 | The stack size is increased to handle larger local variables required for the main thread. |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x2000 | Heap size is allocated to enable dynamic memory allocation for standard library functions. |
| configuration.xml > Stacks > Threads > Net Link Thread > Properties > Settings > Property > Thread > Auto start | Enabled | Disabled | Manual control of when the Net Link thread starts allows better flow management. |
| configuration.xml > Stacks > Threads > Net App Thread > Properties > Settings > Property > Thread > Auto start | Enabled | Disabled | Manual control of when the Net App Thread starts to ensure readiness of dependencies. |
| configuration.xml > Stacks > Threads > Net App Thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 4096 | Increased stack size to handle more complex operations or a larger number of local variables. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_fx_media0 Azure RTOS FileX on Block Media > Properties > Settings > Property > Module g_fx_media0 Azure RTOS FileX on Block Media > Total Sectors | 65536 | 32 | Set for the specific total number of sectors of the block media. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_fx_media0 Azure RTOS FileX on Block Media > Properties > Settings > Property > Module g_fx_media0 Azure RTOS FileX on Block Media > Bytes per Sector | 512 | 4096 | Set for the specific bytes per sector of the block media. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_rm_filex_block_media_0 FileX I/O (rm_filex_block_media) > Properties > Settings > Property > Module g_rm_filex_block_media_0 FileX I/O (rm_filex_block_media) > Partition Number | 0 | 1 | Set for the specific partition number of the block media. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_rm_block_media0 Block Media SPI Flash (rm_block_media_spi) > Properties > Settings > Property > Module g_rm_block_media0 Block Media SPI Flash (rm_block_media_spi) > Block count | 8192 | 32 | Set for the specific number of sectors of the external flash. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_ospi0 OSPI (r_ospi_b) > Properties > Settings > Property > Common > DMAC Support | Disable | Enabled | DMAC support enabled to offload memory transfers and improve performance. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_ospi0 OSPI (r_ospi_b) > Properties > Settings > Property > Module g_ospi0 OSPI (r_ospi_b) > General > Chip Select | CS1 | CS1 | Use OSPI_B Channel 1 to communicate with flash device. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_ospi0 OSPI (r_ospi_b) > Properties > Settings > Property > Module g_ospi0 OSPI (r_ospi_b) > Command Sets > Erase Sizes > Sector Erase | 4096 | 4096 | Sector size of the flash device. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_ospi0 OSPI (r_ospi_b) > Properties > Settings > Property > Module g_ospi0 OSPI (r_ospi_b) > Command Sets > Erase Sizes > Block Erase | 262144 | 65536 | Block size of the flash device. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_ospi0 OSPI (r_ospi_b) > Properties > Settings > Property > Module g_ospi0 OSPI (r_ospi_b) > Command Sets > Initial Mode > Address Length | 1 byte | 4 bytes | Set for the specific number of address length by default for the external flash. |
| configuration.xml > Stacks > Threads > Net App Thread > Net App Thread Stacks > g_rmac_phy0 Ethernet (r_rmac_phy) > Properties > Settings > Property > Module g_rmac_phy0 Ethernet (r_rmac_phy) > Select MII type | RMII | RGMII | Specify whether to use MII or GMII. |
| configuration.xml > Stacks > Threads > Led Control Thread > Led Control Thread Stacks > g_external_irq_sw1 External IRQ (r_icu) > Properties > Settings > Property > Module g_external_irq_sw1 > Channel | 0 | 13 | Channel set to 13 to match the external interrupt pin. |
| configuration.xml > Stacks > Threads > Led Control Thread > Led Control Thread Stacks > g_external_irq_sw1 External IRQ (r_icu) > Properties > Settings > Property > Module g_external_irq_sw1 > Callback | NULL | external_irq_sw1_callback | Callback function for handling external interrupts on SW1. |
| configuration.xml > Stacks > Threads > Led Control Thread > Led Control Thread Stacks > g_external_irq_sw2 External IRQ (r_icu) > Properties > Settings > Property > Module g_external_irq_sw2 > Channel | 0 | 12 | Channel set to 12 to match the external interrupt pin. |
| configuration.xml > Stacks > Threads > Led Control Thread > Led Control Thread Stacks > g_external_irq_sw2 External IRQ (r_icu) > Properties > Settings > Property > Module g_external_irq_sw2 > Callback | NULL | external_irq_sw2_callback | Callback function for handling external interrupts on SW2. |

## API Usage ##
The table below lists the FSP provided API used at the application layer in this example project.

**NetX Duo APIs**
| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| nx_system_initialize | This API is used to initialize the basic NetX Duo system resources in preparation for use. |
| nx_packet_pool_create | This API is used to create a packet pool of the specified packet size in the memory area supplied by the user. |
| nx_packet_allocate | This API is used to allocate a packet from the specified pool and adjust the prepend pointer in the packet according to the type of packet specified. |
| nx_packet_data_append | This API is used to append data to the end of the specified packet. |
| nx_packet_data_retrieve | This API is used to copy data from the supplied packet into the supplied buffer. |
| nx_packet_release | This API is used to release a packet, including any additional packets chained to the specified packet. |
| nx_ip_create | This API is used to create an IP instance with the user supplied IP address and network driver. |
| nx_ip_link_status_change_notify_set | This API is used to configure the link status change notify callback function. |
| nx_ip_status_check | This API is used to check and optionally waits for the specified status of the primary network interface of a previously created IP instance. |
| nx_ip_driver_direct_command | This API is used to provide a direct interface to the application's primary network interface driver specified during the nx_ip_create call. |
| nx_ip_interface_physical_address_get | This API is used to retrieve the physical address of a network interface from the IP instance. |
| nx_ip_address_get | This API is used to retrieve the IPv4 address and its subnet mask of the primary network interface. |
| nx_arp_enable | This API is used to initialize the ARP component of NetX Duo for the specific IP instance. |
| nx_icmp_enable | This API is used to enable the ICMP component for the specified IP instance. |
| nx_icmp_ping | This API is used to send a ping request to the specified IP address and waits for the specified amount of time for a ping response message. |
| nx_udp_enable | This API is used to enable the User Datagram Protocol (UDP) component of NetX Duo. |
| nx_tcp_enable | This API is used to enable the Transmission Control Protocol (TCP) component of NetX Duo. |
| nx_dhcp_create | This API is used to create a DHCP instance for the previously created IP instance. |
| nx_dhcp_packet_pool_set | This API is used to allow the application to create the DHCP Client packet pool by passing in a pointer to a previously created packet pool in this service call. |
| nx_dhcp_start | This API is used to start DHCP processing on all interfaces enabled for DHCP. |
| nx_dhcp_stop | This API is used to stop DHCP processing on all interfaces that have started DHCP processing. |
| nx_dhcp_reinitialize | This API is used to clear the host application network parameters, and clears the DHCP Client state on all interfaces enabled for DHCP. |
| nx_dhcp_user_option_retrieve | This API is used to retrieve the specified DHCP option from the DHCP options buffer on the first interface enabled for DHCP found on the DHCP Client record. |
| nx_tcp_socket_create | This API is used to create a TCP client or server socket for the specified IP instance. |
| nx_tcp_client_socket_bind | This API is used to bind the previously created TCP client socket to the specified TCP port. |
| nx_tcp_client_socket_unbind | This API is used to release the binding between the TCP client socket and a TCP port. |
| nx_tcp_client_socket_connect | This API is used to connect the previously created and bound TCP client socket to the specified server's port. |
| nx_tcp_socket_disconnect | This API is used to disconnect an established client or server socket connection. |
| nx_tcp_socket_receive | This API is used to receive TCP data from the specified socket. |
| nx_tcp_socket_send | This API is used to send TCP data through a previously connected TCP socket. |
| nx_udp_socket_create | This API is used to create a UDP socket for the specified IP instance. |
| nx_udp_socket_bind | This API is used to bind the previously created UDP socket to the specified UDP port. |
| nx_udp_socket_unbind | This API is used to release the binding between the UDP socket and a UDP port. |
| nx_udp_socket_receive | This API is used to receive a UDP data from the specified socket. |
| nx_udp_socket_send | This API is used to send a UDP data through a previously created and bound UDP socket for IPv4 networks. |
| nx_web_http_server_create | This API is used to create an HTTP Server instance, which runs in the context of its own ThreadX thread. |
| nx_web_http_server_start | This API is used to start a previously created HTTP or HTTPS Server instance. |
| nx_web_http_server_stop | This API is used to stop a previously created HTTP or HTTPS Server instance. |
| nx_web_http_server_callback_generate_response_header | This API is used to generate an HTTP response header in the HTTP(S) server callback routine, as defined by the application. |
| nx_web_http_server_callback_packet_send | This API is used to send a complete HTTP server response from an HTTP callback. |
| nx_dns_create | This API is used to create a DNS Client instance for the previously created IP instance. |
| nx_dns_packet_pool_set | This API is used to set a previously created packet pool as the DNS Client packet pool. |
| nx_dns_server_add | This API is used to add an IPv4 DNS Server to the server list. |
| nx_dns_host_by_name_get | This API is used to request name resolution of the supplied name from one or more DNS Servers previously specified by the application. |
| nx_dns_host_by_address_get | This API is used to request name resolution of the supplied IP address from one or more DNS Servers previously specified by the application. |

**ThreadX APIs**
| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| tx_thread_resume | This API is used to resume a suspended thread. |
| tx_thread_suspend | This API is used to suspend a currently running thread. |
| tx_thread_sleep | This API is used to put the current thread to sleep for a specified number of ticks. |
| tx_thread_info_get | This API is used to retrieve information about a specific thread, such as its state, run count, etc. |
| tx_byte_pool_create | This API is used to create a byte memory pool that can be used for dynamic memory allocation. |
| tx_byte_allocate | This API is used to allocate a block of memory from a byte pool. |
| tx_byte_release | This API is used to release a previously allocated block of memory back to the byte pool. |
| tx_queue_create | This API is used to create a queue that can store and manage messages passed between threads. |
| tx_queue_send | This API is used to send a message to a previously created queue. |
| tx_queue_receive | This API is used to receive a message from a previously created queue. |
| tx_event_flags_create | This API is used to create an event flag group that can be used for thread synchronization. |
| tx_event_flags_set | This API is used to set one or more event flags in a specified event flag group. |
| tx_event_flags_get | This API is used to retrieve the event flags that have been set in a specified event flag group. |

**FileX APIs**
| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| fx_system_initialize | This API is used to initialize the FileX file system, preparing it for use. |
| fx_media_open | This API is used to open and initialize a media instance for FileX file operations. |
| RM_FILEX_BLOCK_MEDIA_Open | This API is used to initialize the FileX Block Media interface for the block media driver. |
| RM_FILEX_BLOCK_MEDIA_BlockDriver | This API is used to access block media device functions such as open, close, read, write, and control operations on a block media device. |

## Verifying Operation ##
1. Import, generate, build, and flash the example project to the RA board using e2 studio on the host PC.
    * By default, the EP supports serial terminal for RA boards that support J-Link OB VCOM.

            Define USE_VIRTUAL_COM=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes

    * To use SEGGER J-Link RTT Viewer, please follow the instructions as below:

            Define USE_VIRTUAL_COM=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes
2. Open the terminal application on the host PC and connect to the RA board.  
Note: Before performing test, please confirm that the host PC has enabled the inbound rule '**File and Printer Sharing (Echo Request - ICMPv4-In)**' for accept ping request from RA MCU to TCP server: **Windows Defender Firewall -> Advanced settings -> Inbound Rules -> Enable File and Printer Sharing (Echo Request - ICMPv4-In).**  
![ping_enable.png](images/ping_enable.png "Enable Ping action")
3. Wait for the RA board to obtain the IP address from the router.
4. The RA board's network configuration details and menu options will be displayed in the terminal application.
5. Verify TCP Client Operation:
    1. Launch a socket application on the host PC.
    2. Set up a TCP server by specifying the server's IP address and port number.
    3. Ensure that the server is configured to listen for incoming connections.
    4. In the terminal, type the command: `1 <x.x.x.x><port>`. For example, if the server's IP is `192.168.1.12` and the port is `12345`, type: `1 192.168.1.12:12345`.
    5. The RA board will send a TCP request to the server.
    6. The socket application will display the TCP request.
    7. In the socket application, the user should type any data and click Send promptly to respond to the RA board. **Note:** If the user does not complete this step, a timeout error will occur (expected behavior). The operation will then end and return to the menu so the user can retry.
    8. The RA board will display the received data in the terminal application.
6. Verify UDP Client Operation:
    1. Launch a socket application on the host PC.
    2. Set up a UDP server by specifying the server's IP address and port number.
    3. Ensure that the server is configured to listen for incoming connections.
    4. In the terminal, type the command: `2 <x.x.x.x><port>`. For example, if the server's IP is `192.168.1.12` and the port is `12345`, type: `2 192.168.1.12:12345`.
    5. The RA board will send a UDP request to the server.
    6. The socket application will display the UDP request.
    7. In the socket application, the user should type any data and click Send promptly to respond to the RA board. **Note:** If the user does not complete this step, a timeout error will occur (expected behavior). The operation will then end and return to the menu so the user can retry.
    8. The RA board will display the received data in the terminal application.
7. Verify DNS Client Operation:
    * Request a PTR record for an IP address:
        1. In the terminal, type: `3 <x.x.x.x>`. For example, if the IP address is `192.168.1.12`, type: `3 192.168.1.12`.
        2. The RA board will receive and display the PTR record data.
    * Request an A record for a domain:
        1. In the terminal, type: `3 <domain>`. For example, if the domain name is `renesas.com`, type: `3 renesas.com`.
        2. The RA board will receive and display the A record data.
8. Verify Web Server Operation:
    1. In the terminal, type '4 to start the web server.
    2. Open a web browser on the host PC.
    3. In the URL bar, type: `<x.x.x.x>/index.html` and press Enter, where `<x.x.x.x>` is the IP address of the RA board.
    4. The browser will display the network configuration and LED control panel.
    5. In the LED control panel on the webpage, click on the SW1 or SW2 buttons to toggle LED1 or LED2 on the RA board.
    6. Click on SW1 or SW2 buttons on the RA board to toggle LED1 or LED2.
    7. The RA board will update the LED states dynamically on the webpage.
    8. In the terminal, type any character to stop the web server. 

### Note: ###
* **x.x.x.x** is a placeholder for an IP address, which may vary depending on the environment, and the use of the same notation does not imply that the values are identical across different steps.
* By default, the web.bin file containing the website content is flashed to OSPI during debugging. Ensure that it is also flashed when running the project using a hex file to avoid empty content in Option 4.
* For using the serial terminal application:
    1. To echo back characters typed in Tera Term, the user needs to enable it through: [Setup] -> [Terminal...] -> Check [Local echo].
    2. The configuration parameters of the serial port are:
		- Port: Provided by the J-Link OB VCOM.
		- Speed: 115200
		- Data: 8 bit
		- Parity: none
		- Stop bits: 1 bit
		- Flow control: none
* For the SEGGER RTT Viewer application:
    * If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) in .map file generated in the project folder (e2studio\Debug or e2studio\Release).

### The images below showcase the output on the serial terminal application (Tera Term) ###
**The EP information**

![The EP Information](images/serial_ep_info.png "The EP Information")

**The TCP client operation**

![The TCP socket application](images/socket_app_tcp.png "The TCP socket application")

![The TCP client operation](images/serial_tcp_client.png "The TCP client operation")

**The UDP client operation**

![The UDP socket application](images/socket_app_udp.png "The UDP socket application")

![The UDP client operation](images/serial_udp_client.png "The UDP client operation")

**The DNS client operation**

![The DNS client operation](images/serial_dns_client.png "The DNS client operation")

**The Web http server operation**

![The Web http server operation](images/serial_web_http_server.png "The Web http server operation")

![The Web page](images/webpage.png "The Web page")

### The images below showcase the output on the SEGGER J-Link RTT Viewer ###
**The EP information**

![The EP Information](images/rtt_ep_info.png "The EP Information")

**The TCP client operation**

![The TCP client operation](images/rtt_tcp_client.png "The TCP client operation")

**The UDP client operation**

![The UDP client operation](images/rtt_udp_client.png "The UDP client operation")

**The DNS client operation**

![The DNS client operation](images/rtt_dns_client.png "The DNS client operation")

**The Web http server operation**

![The Web http server operation](images/rtt_web_http_server.png "The Web http server operation")

![The Web page](images/webpage.png "The Web page")

## Special Topics ##
* The `src/webserver` folder contains all the necessary files for the web HTTP server functionality.
* The `src/webserver/website` folder contains all the resources that will be served by the web server.
    * The `index.html` file is the main HTML file for the homepage of the website.
    * The `script.js` file includes all the JavaScript functions used to create dynamic content on the website.
    * The `logo.png` file is the logo displayed on the homepage.
    * The `favicon.ico` file is the small icon that appears in the browser tab for the website.
* The `src/webserver/website.bin` is a file system image containing all the files from the `website` folder in FAT12 format. This image is loaded into the external OSPI flash on the RA board to support FileX operations, which serve as a file system dependency for the web HTTP server.
* To create `website.bin` in a Linux environment, such as Windows Subsystem for Linux (WSL), follow these steps:
  1. **Create an empty binary file (`website.bin`)**  
     ```bash
     dd if=/dev/zero of=website.bin bs=4096 count=32
     ```

  2. **Format the binary file as a FAT12 filesystem**  
     ```bash
     mkdosfs -F 12 -S 4096 -s 1 website.bin
     ```

  3. **Mount the FAT12 filesystem**  
     ```bash
     mkdir -p /mnt/website_bin
     sudo mount -o loop website.bin /mnt/website_bin
     ```

  4. **Copy resources into the filesystem**  
     ```bash
     sudo cp -r /path/to/web/resources/* /mnt/website_bin/
     ```

  5. **Unmount the filesystem**  
     ```bash
     sudo umount /mnt/website_bin
     ```
