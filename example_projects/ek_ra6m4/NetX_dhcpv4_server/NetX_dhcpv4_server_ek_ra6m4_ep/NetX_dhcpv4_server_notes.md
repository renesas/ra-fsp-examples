# Introduction #

The sample code accompanying this file shows the operation of a NetX Duo DHCPv4 Server on a RA MCU using Azure RTOS. 
In this sample code, a NetX Duo IP instance is created with IP address as (default 0.0.0.0 - Not assigned).
The Server is always configured the IP address, In this EP 192.168.0.2 is the IP address of the Board (DHCP Server). 
The NetX stack is enabled for UDP, ICMP, ARP. DHCP Server uses the UDP protocol to receive DHCP Client requests and transmit responses. 
DHCP Server creates its own packet pool based on the settings minimum packet payload size and number of packets in the packet pool. 
DHCPV4 server creates a pool of IP addresses from the given range and set network parameters for specified interface.

Once the DHCPv4 Server is running and has created client records and assigned IP addresses, it
periodically checks the time remaining on each of the client IP leases. If a lease expires, the server simply
removes the client record from the client record table, and returns that IP address back to the pool of
assignable IP addresses. No message is sent to the client.

The status messages, clients info, and the code flow informational messages as part of the initialization are displayed on the JlinkRTTViewer.

**Note :** Range of IP address for clients are from 192.168.0.10 to 192.168.0.20.

NetX Duo DHCPV4 Server is compliant with [RFC2132](https://tools.ietf.org/html/rfc2132), [RFC2131](https://tools.ietf.org/html/rfc2131), and related RFCs.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the dhcpv4 server example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit with Ethernet Support
* Micro USB cable
* Host Machine
* Ethernet cable

Refer to [readme.txt](./readme.txt) on information on how to connect the hardware.

### Software ###
Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![dhcpv4_server](images/dhcpv4_server.jpg "DHCPV4 Server Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| NetX Duo DHCP IPV4 Server | DHCPV4 Server module is used to provide and assign IP addresses automatically, default gateways and other network parameters to dhcpv4 client devices.| dhcp |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> BSP > Properties > Settings > Property > Heap Size (bytes)| 0 |0x400| Heap size is required for standard library functions to be used as per FSP requirements. |
|   configuration.xml -> DHCPV4 Server Thread > Settings > Property > Thread > priority  |   1   |   2   |   DHCPV4 server thread priority is lowered to allow the IP thread to process incoming packets at the fastest rate possible.   |
|   configuration.xml -> RTT Thread > Settings > Property > Thread > priority  |   1   |   3   |   RTT thread priority is lowered to allow the DHCP server and IP threads to process incoming packets at the fastest rate possible.   |

## API Usage ##

The table below lists the DHCPV4 Server API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|nx_dhcp_create_server_ip_address_list| This API is used to create a pool of IP addresses from the given range. |
|nx_dhcp_set_interface_network_parameters| This API is used to set network parameters returned by the DHCP server.|
|nx_dhcp_server_start| This API is used to start DHCP Server processing.|
|nx_dhcp_server_stop| This API is used to stop DHCP Server processing.|

## Verifying operation ##
Import, Build and Debug the EP(see section Starting Development of **FSP User Manual**). After running the EP, open rtt viewer to see the output.
Before running the example project, refer the below steps for hardware connections :
* Connect RA MCU debug port to the host PC via a micro USB cable. 
* Connect an Ethernet cable to the RJ45 connector on RA MCU and other end connect to ethernet port of another RA board where dhcpv4 client is residing.

Below image showcases the hardware connection setup required for running the EP :

![dhcpv4_server_hardware_setup](images/dhcpv4_server.jpg "DHCPV4 Server Hardware Setup")

Below images showcases the output on JLinkRTT_Viewer :

![dhcpv4_server_rtt_output](images/dhcpv4_server_rtt_log.jpg "DHCPV4 Server RTT Log")

![dhcpv4_server_rtt_output](images/dhcpv4_server_rtt_log1.jpg "DHCPV4 Server RTT Log")

## Special Topics ##
