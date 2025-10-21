# Introduction #
This project demonstrates the basic functionality of Netx Duo DHCPv6 client with an ethernet driver on Renesas RA MCUs based on Renesas FSP using AzureRTOS. DHCP client is created and started. On successful receiving of IP address from DHCPv6 server, It pings the server to verify the received IP address. On successful completion, the success status and IP address are displayed on the RTT Viewer. The error and info messages will be printed on RTT Viewer.

NetX Duo DHCPv6 Client is compliant with [RFC3315](https://tools.ietf.org/html/rfc3315), [RFC3646](https://tools.ietf.org/html/rfc3646), and related RFCs.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the NetX DHCPv6 client example project, the following resources are needed.

### Software ###
Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

### Hardware ###
Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1
* 1 x Renesas RA board.
* 1 x Type-C USB cable for programming and debugging.
* 1 x Ethernet/LAN cable (Ethernet cable CAT5/6).
* 1 x Ethernet switch.

### Hardware Connections ###
* For EK-RA6M3, EK-RA6M4, EK-RA6M5:
    * Connect the RA board USB device port to the host machine via a micro USB cable.
    * Connect LAN cable on RA board at ethernet port and other end connect to Ethernet switch/router.
* For EK-RA8M1:
    * Connect the RA board USB device port to the host machine via a micro USB cable.
    * Connect LAN cable on RA board at ethernet port and other end connect to Ethernet switch/router.
    * Remove jumper J61 to enable Ethernet B.
* For EK-RA8D1:
    * Connect the RA board USB device port to the host machine via a micro USB cable.
    * Connect LAN cable on RA board at ethernet port and other end connect to Ethernet switch/router.
    * Set the configuration switches (SW1) as below.

        | SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
        |-------------|-------------|--------------|------------|------------|------------|-------------|-----------|
        | OFF | OFF | OFF | OFF | OFF | OFF | ON | OFF |

* For MCK-RA8T1, EK-RA8P1:
    * Connect the RA board USB device port to the host machine via a Type-C USB cable.
    * Connect LAN cable on RA board at ethernet port and other end connect to Ethernet switch/router.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![dhcpv6_client](images/dhcp_client_ipv6.jpg "DHCPV6 Client Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| NetX Duo DHCP IPV6 Client | DHCPv6 client module is used for dynamic global IPv6 address assignment from a DHCPv6 server and to automate the process of configuring devices on IP networks, thus allowing them to use network services such as DNS, and any communication protocol based on UDP or TCP.| dhcp |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different from those selected by default.

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x1000 | Main Program thread stack is configured to store the local variables of different functions in the code. |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x2000 | Heap size is required for standard library functions to be used. |
| configuration.xml > Stacks > Threads > DHCP Client Thread > Properties > Settings > Property > Thread > Priority | 1 | 3 | DHCP Client thread priority is lowered to allow the IP thread to process incoming packets at the fastest rate possible. |
| configuration.xml > Stacks > Threads > DHCP Client Thread > DHCP Client Thread Stacks > g_dhcpv6_client0 Azure RTOS NetX Duo DHCP IPv6 Client > Properties > Settings > Property > Common > Duplicate Address Detection support | Enabled (default) | Disabled | Duplicate Address Detection property is disabled to avoid detecting any duplicate address. |
| configuration.xml > Stacks > Threads > RTT_Thread > Properties > Settings > Property > Thread > Priority | 1 | 4 | RTT thread priority is lowered to allow the DHCP Client and IP threads to process incoming packets at the fastest rate possible. |

## API Usage ##

The table below lists the DHCP Client API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| nx_ip_driver_direct_command | This API is used to issue command to network driver. |
| nx_dhcpv6_create_client_duid | This API is used to create a Link Layer Plus Time DUID for the DHCPv6 Client. |
| nx_dhcpv6_create_client_iana | This API is used to add an Identity Association to the Client. |
| nx_dhcpv6_request_option_timezone | This API is used to add the time zone option to the Client option request data in request messages to the Server. |
| nx_dhcpv6_request_option_DNS_server | This API is used to add the DNS server option to the Client option request data in request messages to the Server. |
| nx_dhcpv6_start | This API is used to start the DHCPv6 Client thread task. |
| nx_dhcpv6_request_solicit | This API is used to send a DHCPv6 SOLICIT request to any Server on the Client network (broadcast). |
| nx_dhcpv6_get_valid_ip_address_count | This API is used to get the number of IPv6 addresses assigned to the DHCPv6 Client. |
| nx_dhcpv6_get_IP_address | This API is used to get the global IPv6 address assigned to the DHCPv6 client. |
| nx_dhcpv6_get_DNS_server_address | This API is used to get DNS Server address at the specified index into the DHCPv6 Client DNS server list. |
| nxd_icmp_ping | This API is used to send out an ICMP Echo Request packet through an appropriate physical interface and waits for an Echo Reply from the destination host. |
| nx_dhcpv6_get_lease_time_data | This API is used to retrieve Client’s IA address lease time data. |
| nx_dhcpv6_request_release | This API is used to release the IP address back to the Server. |
| nx_dhcpv6_stop | This API is used to stop the DHCPv6 Client thread task. |
| nx_dhcpv6_client_delete | This API is used to delete a DHCPv6 client instance. |

## Verifying Operation ##
* Import, build and debug the EP (see section Starting Development of **FSP User Manual**). After running the EP, open J-Link RTT Viewer to see the output.
* Before running the example project, refer the steps below for hardware connections:
    * Connect RA MCU debug port to the host PC via a Type-C USB cable.
    * Connect an Ethernet cable to the J15 connector on RA board via ethernet switch/hub which is connected to Router or LAN where DHCP server is residing.
    * A DHCPv6 server needs to exist on the network to assign an IP address to the RA board running this DHCP client example project.

The image below showcases the hardware connection setup required for running the EP:

![dhcpv6_client_hardware_setup](images/dhcp_client_ipv6.jpg "DHCPV6 Client Hardware Setup")

The images below showcase the output on J-Link RTT Viewer:

![dhcpv6_client_ep_info](images/ep_info.jpg "EP Information")

![dhcpv6_client_rtt_output_1](images/dhcp_v6_client_rtt_log_1.jpg "DHCPv6 Client RTT Log")

![dhcpv6_client_rtt_output_2](images/dhcp_v6_client_rtt_log_2.jpg "DHCPv6 Client RTT Log")
