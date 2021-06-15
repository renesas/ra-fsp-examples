# Introduction #

The sample code accompanying this file shows the operation of a NetX Duo FTP Server on a RA MCU using Azure RTOS. 
In this sample code, a NetX Duo IP instance is created with the user-configured static IP address. The configuration of static IP address should be set
as per the user's network environment through updating RA configurator property "g_ftp_server0->g_ip0 NetX DUO IP instance->IPv4 address, Subnet mask".
The NetX stack is enabled for TCP, ICMP, ARP. FTP server utilizes the reliable Transmission Control Protocol (TCP) services
to perform its content transfer function. 
FTP Server creates its packet pool based on the settings minimum packet payload size and number of packets in the packet pool. 

When the FTP server is created and started, the NetX Duo FTP server creates a new TCP connection with the FTP client upon its connect requests and begins the FTP session.
The status messages, client's info(like IP address, port number), errors(if any) are displayed on the JlinkRTTViewer.

NetX Duo FTP Server is compliant with [RFC1579](https://datatracker.ietf.org/doc/html/rfc1579), [RFC959](https://datatracker.ietf.org/doc/html/rfc959), and related RFCs.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of the operation.

## Required Resources ##
To build and run the FTP server example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit with Ethernet Support
* Micro USB cable
* Host Machine
* Ethernet cable

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![ftp_server](images/ftp_server.jpg "FTP Server Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| NetX Duo FTP Server | FTP Server module is used to provide data transfer with a valid FTP client machine.| FTP |

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> BSP > Properties > Settings > Property > Heap Size (bytes)| 0 |0x400| Heap size is required for standard library functions to be used as per FSP requirements. |
|   configuration.xml -> RTT Thread > Settings > Property > Thread > priority  |   1   |   3   |   RTT thread priority is lowered to allow the FTP Server and IP threads to process incoming packets at the fastest rate possible.   |
|   configuration.xml -> ftp server thread > Settings > Property > Thread > priority  |   1   |   2   |   Priority of the Application threads generally given  lower priority compared to system services threads.   |
|   configuration.xml -> ftp server thread > Settings > Property > Thread > stack size  |   1024   |   2048   |   Updated to handle thread its worst-case function call nesting and local variable usage.   |
|   configuration.xml -> ftp server thread > g_fx_media0 FileX on Block Media > Settings > Property > Total Sectors  |   3751936   |   8192   |   number of total sectors is updated as per on board QSPI flash IC.   |
|   configuration.xml -> ftp server thread > g_fx_media0 FileX on Block Media > Settings > Property > Bytes per Sector  |   512   |   4096   |   number of total bytes in a sector is updated as per on board QSPI flash IC.   |
|   configuration.xml -> ftp server thread > g_fx_media0 FileX on Block Media > Settings > Property > Working meida memory size  |   512   |   4096   |  working media memory must be greater or equal to the size of one sector.   |

## API Usage ##

The table below lists the DHCPV4 Server API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|nx_ftp_server_start| This API is used to start FTP server |
|nxd_ftp_server_create| This API is used to create FTP server|

## Verifying operation ##
Import, Build and Debug the EP(see section Starting Development of **FSP User Manual**). After running the EP, open the RTT viewer to see the output.
Before running the example project, refer to the below steps for hardware connections :
* Connect RA MCU debug port to the host PC via a micro USB cable. 
* Connect an Ethernet cable to the RJ45 connector on RA MCU via ethernet switch or router which is connected to the Host machine where the FTP client is residing.

The below image showcases the hardware connection setup required for running the EP :

![ftp_server](images/ftp_server.jpg "FTP Server Hardware Setup")

Below images showcases the output on JLinkRTT_Viewer :

![ftp_server_rtt_log](images/ftp_server_rtt_log.jpg "FTP Server RTT Log")

## Special Topics ##
### Running tips ###
* Presuming all necessary hardware connections done, a user should connect their PC/Laptop on the same network environment similar to the RA board network environment.
* User needs to update the IP address of RA board as per their network environment through "g_ip0 NetX Duo IP Instance" stack as shown in the image below using RA configuration tool. This EP is built and tested with default properties as shown in the image below.

![update_ip](images/update_ip.jpg "update_ip")

* User needs to open command prompt(in Windows) or terminal(Linux_pc/raspberry_pi) and use FTP utility for FTP client operations. (FTP utility is platform independent)
* One sample usage is shown below  

![ftp_client](images/ftp_client.jpg "ftp_client")

* There is no authentication. Just press Enter for both Username and password required by FTP utility as shown in the above image.
* If EP is not working in terms of network connection/ip, please re-check [readme.txt](./readme.txt) and RA configuration property "g_ftp_server0->g_ip0 NetX DUO IP instance->IPv4 address, Subnet mask".
