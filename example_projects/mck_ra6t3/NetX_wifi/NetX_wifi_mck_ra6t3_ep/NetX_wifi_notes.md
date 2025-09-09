# Introduction #

The example code accompanying this file shows the operation of a NetX Duo Wifi module using RA MCU Azure RTOS NetX Duo Network Drivers. 
In this example code, A NetX Duo IP instance is created upon call to NetX Duo Network Drivers APIs, NetX Duo Wifi utilizes the WiFi Onchip Silex module for Wifi Access point connectivity, IP address assignment and Ping to IP address entered by the user. 
Upon successful initialization of WiFi Onchip Silex module, user is asked to select the SSID to connect in auto scan mode or to Enter the desired SSID for wifi connection in Manual mode. Once Entered the SSID name the user is asked for security to be entered (Open WPA or WPA2) as menu option, after this the password (according to security) is to be entered by the user to connect to particular SSID.
The Azure NetX Duo initializes the packet pool creation with definite packet payload size, where the network packets are utilized in data transfer from client and server via TCP.
The connection to TCP server (Details of requirements and steps to connect are mentioned in below sections) is done in client mode using NetX APIs for creation of TCP socket, binding of the specified port and connecting to the entered address of running TCP server by the user. Upon successful connection with TCP server data packets are sent to the server and received from the server by the application, On board LED state is changed(on or off) as per the message received from the connected TCP server.

The status messages, like MCU IP address, Ping status, TCP connection status, message from the TCP server and errors(if any) are displayed on the JlinkRTTViewer.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of the operation.

## Required Resources ##
To build and run the NetX Wifi example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit with UART PMOD Support.
* Renesas Silex UART Pmod.
* WiFi router Access Point(AP) with internet access.
* PC or laptop connected to the same WiFi router AP.(For TCP server)

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware and start the TCP server.

### Software ###
* Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

* Sokit software is required for creation of TCP server over specified IP and port(detailed steps mentioned below).
Download Link : https://softfamous.com/sokit/ 


## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![wifi_block_digaram](images/wifi_block_digaram.jpg "NetX Wifi Block Diagram")


## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| Azure RTOS NetX Duo IP Instance | This module provides network drivers for Azure RTOS NetX Duo.| Azure RTOS NetX Duo Network Drivers |
| NetX Duo Wifi Driver | For IP communication the NetX IP instance needs to be used. | NetX Duo IP |
| Net Duo packet pool | Packet Pool Module is used to send and receive the created data packets over TCP network. | NetX Duo Packet Pool | 
| Wifi onchip Silex Driver | For wifi SSID connect, ping to IP/URL Wifi onchip Silex Driver is used | Wifi onchip Silex Driver |


## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> BSP > Properties > Settings > Property > Heap Size (bytes)| 0 |0x1000| Higher Heap size is required for standard library functions to be used as per FSP recommendation. |
|   configuration.xml -> BSP > Properties > Settings > Property > Main Stack Size (bytes)| 0 |0x400| Main Program thread stack is configured to store the local variables of different functions in the code. |
|   configuration.xml -> RTT Thread > Settings > Property > Thread > priority  |   1   |   5   |   RTT thread priority is lowered to allow the IP threads to process incoming packets at the fastest rate possible.   |
|   configuration.xml -> Azure Wifi thread > Settings > Property > Thread > priority  |   1   |   4   |   Priority of the Application threads generally given lower priority compared to system services threads.   |
|   configuration.xml -> g_ip0 Azure RTOS NetX Duo IP Instance > g_ip0 Azure RTOS NetX Duo IP Instance  > Default Gateway Address >| 0.0.0.0 | 192.168.0.1   |   Valid Gateway adress for IP routing.   |
|   configuration.xml -> g_ip0 Azure RTOS NetX Duo IP Instance > g_ip0 Azure RTOS NetX Duo IP Instance  > Subnet Mask >| 255.255.255.0 | 0.0.0.0   |   Subnet mask for IP routing.   |
|   configuration.xml -> Azure Wifi thread > Settings > Property > Thread > stack size  |   1024   |   4096   |   Updated to handle thread its worst-case function call nesting and local variable usage.   |

For EK-RA2L1/EK-RA4M1
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> Azure Wifi thread > Settings > Property > Thread > stack size  |   1024   |   2048   |   Updated to handle thread its worst-case function call nesting and local variable usage.   |
|   configuration.xml ->g_ip0 Azure RTOS NetX Duo IP Instance -> g_packet_pool0 Azure RTOS NetX Duo packet pool Instance > Settings > Property > Module g_packet_pool0 Azure RTOS NetX Duo packet pool > Packet Size  | 1568  | 256  | Packet size reduced to prevent memory overflow |
|   configuration.xml ->g_ip0 Azure RTOS NetX Duo IP Instance -> g_packet_pool0 Azure RTOS NetX Duo packet pool Instance > Settings > Property > Module g_packet_pool0 Azure RTOS NetX Duo packet pool > Number of Packets in Pool  | 16 | 8  | Number of Packets reduced to prevent memory overflow |

For EK-RA4E2/EK-RA6E2/MCK-RA4T1/MCK-RA6T3
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> BSP > Properties > Settings > Property > Heap Size (bytes)| 0 |0x400| Update to handle can not move location backwards. |
|   configuration.xml ->g_ip0 Azure RTOS NetX Duo IP Instance -> g_packet_pool0 Azure RTOS NetX Duo packet pool Instance > Settings > Property > Module g_packet_pool0 Azure RTOS NetX Duo packet pool > Number of Packets in Pool  | 16 | 4  | Number of Packets reduced to prevent memory overflow |


## API Usage ##

The table below lists the Azure RTOS NetX Duo Network Drivers API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|rm_wifi_onchip_silex_open| This API is used to initialize the WiFi module driver |
|rm_wifi_onchip_silex_scan| This API is used to scan all Wifi Access points |
|rm_wifi_onchip_silex_connect| This API is used to connect with specified Wifi Access point |
rm_wifi_onchip_silex_dns_query| This API is used to perform a DNS lookup |	
rm_wifi_onchip_silex_ip_addr_get|This API fetches the IP address assigned to MCU|	
rm_wifi_onchip_silex_ping|This API Performs the Ping operation |	
nx_system_initialize| This API Initializes NetX System|	
nx_packet_pool_create | This service creates a packet pool of the specified packet size in the memory area| 	
nx_ip_create|Creates an IP instance|
nx_tcp_enable|This API enables the Transmission Control Protocol (TCP) component of NetX.|	
nx_tcp_socket_create|This API creates a TCP client or server socket for the specified IP instance.|	
nx_tcp_client_socket_bind|This API binds the previously created TCP client socket to the specified TCP port|	
nx_tcp_client_socket_connect|This API connects the previously-created and bound TCP client socket to the specified server's port.|		
nx_packet_allocate|This API allocates a packet from the specified pool |
nx_tcp_socket_send |This service sends TCP data through a previously connected TCP socket.|	
nx_tcp_socket_receive|This service receives TCP data from the specified socket.|

## Verifying operation ##
Import, Build and Debug the EP(see section Starting Development of **FSP User Manual**). After running the EP, open the RTT viewer to see the output.
Before running the example project, refer to the below steps for hardware connections :
* Connect RA MCU debug port to the host PC via a micro USB cable. 
* Connect a Silex Module to the UART PMOD on RA board.
* TCP Server must be running at valid IP address on same network on which Application is running.
*Note*: If TCP server not started before MCU tries to connect, then application would fail.

## Below images showcases the output on JLinkRTT_Viewer ##
* NetX_wifi Banner Print and EP Information:    
![banner_print.png](images/banner_print.png "Banner Print")
* User prompt for Auto Scan or to Specify Wifi access point:                
![wifi_access.png](images/wifi_access.png "Wifi Access")
* Connecting to Wifi access point and IP assignment:              
![assign_IP.png](images/assign_IP.png "Wifi connection and IP assignment")
* Ping operation and Connection to TCP server running on same network:          
![URL_ping_tcp_conn.png](images/URL_ping_tcp_conn.png "Ping and TCP server connection")
* Validation of TCP server entered by the user and Prompt for user to enter valid Port Number:      
![validate_tcp.png](images/validate_tcp.png "validate TCP server address and TCP Port prompt")
* Send message to server on Connection and wait for server message:      
![server_status_message.png](images/server_status_message.png "Sending and Receive Server Message")  
The message from TCP Server as "on" or "off" is sent and is displayed in RTT viewer upon successful reception of valid message.  
(Here either "on" or "off" is considered as a valid message from TCP server as these perform same action on User LED. Refer below steps for using Sokit tool.)
## Steps for TCP server creation and connection ##

* Download and Run Sokit application.(Download link attached in **[Software](#Software)** Requirements section of this file.). The application interface looks like below image.
![sokit_interface.JPG](images/sokit_interface.JPG "Sokit Interface")

* The IP address of System on which sokit is running must be entered under **TCP Addr** section, so that it runs TCP Server over this entered IP of System on network. Suggestion is to
TCP server in PC should listen at a port number greater than 10000. Enter IP address under **TCP Addr** section and free port under **Port** of sokit application. After Entering the details click on **TCP Listen** button in application as shown in below image.    

![sokit_init.JPG](images/sokit_init.JPG "Sokit Initialization")

* After successful connection to the TCP server The sokit application shows IP address and port number of the client under **Connections** section in sokit application with "Hello" message from client, as shown in Image below.  
![sokit_client_conn.JPG](images/sokit_client_conn.JPG "Sokit Client Connection")

* After successful establishment of connection between TCP Client application(which is EP running in client mode) and TCP server, message transfer can be made from server to client Select the Client in Connections window, Enter 'on' or 'off' in Buffer section(shown as Buf1,Buf2,Buf3 in image) of Sokit, as shown in below image.
![sokit_on_off_msg.JPG](images/sokit_on_off_msg.JPG "Sokit Client Connection")

* For the Application Other message than "on" or "off" is considered Invalid(Reception of Message is shown in RTT viewer). Acknowledgement from connected client is sent to server on successful reception of "on" or "off" message.
On sending "on", user LED turns ON and on sending "off" User LED is turned OFF. No action is performed on reception of any other message string.

## Special Topics ##
### Running tips ###
* Presuming all necessary hardware connections done, a user should connect their PC or laptop connected to the same WiFi router AP. 
* On PC/laptop, user needs to create TCP server and start listening at port which is free.
* TCP server on PC can be created using freely available tool sokit and using **[ Steps for TCP server creation and connection](# Steps for TCP server creation and connection)**.
* TCP server must be running before the application tries to connect otherwise tcp connection functionality will fail on running application. 
* Values Sent from connected TCP server other than 'on' or 'off' will have no effect on LED.
* If EP is not working in terms of network connection/ip, please re-check RA configuration property for pins and Ensure that the TCP server is running before application starts.
