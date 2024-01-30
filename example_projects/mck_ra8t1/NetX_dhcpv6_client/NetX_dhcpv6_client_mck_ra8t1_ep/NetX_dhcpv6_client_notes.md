# Introduction #

The sample code accompanying this file shows the operation of a NetX Duo DHCPv6 Client on a RA MCU using Azure RTOS. 
In this sample code, a NetX Duo IP instance is created with IP address as (default 0.0.0.0 - Not assigned). 
The NetX stack is enabled for UDP, ICMP, ARP, IPV6, ICMPv6. DHCP Client uses the UDP for sending and receiving DHCP messages. 
DHCP Client creates its own packet pool based on the settings minimum packet payload size and number of packets in the packet pool. 
When DHCP Client is created and started, a Client first broadcasts a SOLICIT message to begin the process of requesting a global IPv6 address assignment,
and it waits for the valid IP address to be assigned from the DHCPv6 server.
After the successful DHCP messages are exchanged server leases and assigns the IP address to the client, now the Board can ping to the DHCP server. 
When the server assigns the client an IP address, it may also supply other network information, such as a DNS server, Gateway address etc. 
The status messages, Acquired IP addresses, and client's assigned IANA lease data, and the code flow informational messages as part of the 
initialization are displayed on the JlinkRTTViewer.

NetX Duo DHCPV6 Client is compliant with [RFC3315](https://tools.ietf.org/html/rfc3315) , [RFC3646](https://tools.ietf.org/html/rfc3646) , and related RFCs.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the dhcpv6 client example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit with Ethernet Support
* Micro USB cable (For MCK-RA8T1: Type C USB cable)
* Host Machine
* Ethernet cable
* Ethernet switch/hub

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
![dhcpv6_client](images/dhcp_client_ipv6.jpg "DHCPV6 Client Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| NetX Duo DHCP IPV6 Client | DHCPV6 Client module is used for dynamic global IPv6 address assignment from a DHCPv6 server and to automate the process of configuring devices on IP networks, thus allowing them to use network services such as DNS, and any communication protocol based on UDP or TCP.| dhcp |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> BSP > Properties > Settings > Property >Main Stack Size (bytes)| 0x400 |0x1000| Main Program thread stack is configured to store the local variables of different functions in the code. |
|   configuration.xml -> BSP > Properties > Settings > Property > Heap Size (bytes)| 0 |0x2000| Heap size is required for standard library functions to be used. |
|   configuration.xml -> g_dhcpv6_client0 NetX Duo DHCP IPv6 Client > Settings > Property > Common > Common > Duplicate Address Detection  |  Enabled    |   Disabled   |  Duplicate Address Detection property is disabled to avoid detecting any duplicate address.  |
|   configuration.xml -> DHCP Client Thread > Settings > Property > Thread > priority  |   1   |   3   |   DHCP Client thread priority is lowered to allow the IP thread to process incoming packets at the fastest rate possible.   |
|   configuration.xml -> RTT Thread > Settings > Property > Thread > priority  |   1   |   4   |   RTT thread priority is lowered to allow the DHCP Client and IP threads to process incoming packets at the fastest rate possible.   |

## API Usage ##

The table below lists the DHCP Client API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|nx_ip_driver_direct_command| This API is used to issue command to network driver. |
|nx_dhcpv6_create_client_duid| This API is used to create a Link Layer Plus Time DUID for the DHCPv6 Client.|
|nx_dhcpv6_create_client_iana| This API is used to add an Identity Association to the Client .|
|nx_dhcpv6_request_option_timezone| This API is used to add the time zone option to the Client option request data in request messages to the Server .|
|nx_dhcpv6_request_option_DNS_server| This API is used to add the DNS server option to the Client option request data in request messages to the Server .|
|nx_dhcpv6_start| This API is used to start the DHCPv6 Client thread task.|
|nx_dhcpv6_request_solicit| This API is used to send a DHCPv6 SOLICIT request to any Server on the Client network (broadcast) .| 
|nx_dhcpv6_get_valid_ip_address_count| This API is used to get the number of IPv6 addresses assigned to the DHCPv6 Client .| 
|nx_dhcpv6_get_IP_address| This API is used to get the global IPv6 address assigned to the DHCPv6 client .| 
|nx_dhcpv6_get_DNS_server_address| This API is used to get DNS Server address at the specified index into the DHCPv6 Client DNS server list .| 
|nxd_icmp_ping| This API sends out an ICMP Echo Request packet through an appropriate physical interface and waits for an Echo Reply from the destination host.| 
|nx_dhcpv6_get_lease_time_data| This API is used to retrieve Client’s IA address lease time data .| 
|nx_dhcpv6_request_release| This API is used to release the IP address back to the Server .| 
|nx_dhcpv6_stop| This API is used to stop the DHCPv6 Client thread task .|
|nx_dhcpv6_client_delete| This API is used to delete a DHCPV6 client instance .|

## Verifying operation ##
Import, Build and Debug the EP(see section Starting Development of **FSP User Manual**). After running the EP, open rtt viewer to see the output.
Before running the example project, refer the below steps for hardware connections :
* Connect RA MCU debug port to the host PC via a micro USB cable. 
* Connect an Ethernet cable to the RJ45 connector on RA MCU via ethernet switch/hub which is connected to Router or LAN where DHCP server is residing.
* A DHCPV6 Server needs to exist on the network to assign  an IP address to the RA MCU running this DHCP Client Example Project.

Below image showcases the hardware connection setup required for running the EP :

![dhcpv6_client_hardware_setup](images/dhcp_client_ipv6.jpg "DHCPV6 Client Hardware Setup")

Below images showcases the output on JLinkRTT_Viewer :

![dhcpv6_client_rtt_output](images/dhcp_v6_client_rtt_log.jpg "DHCPV6 Client RTT Log")

## Special Topics ##
