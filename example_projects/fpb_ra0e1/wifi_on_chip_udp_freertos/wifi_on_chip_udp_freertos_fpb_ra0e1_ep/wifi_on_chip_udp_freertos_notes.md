# Introduction #

The example project demonstrates the functionalities of WiFi_OnChip_Silex running on Renesas RA MCUs using Renesas Silex UART Pmod hardware. The project gives user option to either scan WiFi networks or manually enter network credentials in J-Link RTT Viewer and connect. Then it performs a ping operation on user's entered URL or IP address. If successful, 'Ping Successful' message is printed on J-Link RTT Viewer. The project also demonstrates UDP socket operations in client mode. It tries connecting to the user entered UDP server IP address and port number. It is expected that UDP socket in server mode would be running on user PC and listening on a free port before the EP tries to connect. Once connected to the UDP server, the client sends a 'hello' to the server and then waits for the user to send the message from the server. Any incoming message from the server is displayed on J-Link RTT Viewer connected to the client. After receiving messages from the server, appropriate acknowledgment messages are sent to the server. 

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of the operation.

## Required Resources ##
To build and run the example project, the following resources are needed.

### Hardware ###
* 1 x RA board 
* 1 x Renesas Silex UART Pmod module
* 1 x Wi-Fi router Access Point (AP) with internet access. Note: WiFi router AP's credentials should be known.
* 1 x PC or laptop connected to the same Wi-Fi router AP. On PC/laptop, create UDP server and start listening at a port which is free. Port of value greater than 10000 is recommended.
* 1 x Micro USB cable (For MCK-RA8T1: Type C USB cable)

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.2.0
* e2 studio: Version 2025-10
* SEGGER J-Link RTT Viewer: Version 8.74
* GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
* Terminal Console Application: Tera Term or a similar application

1. Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
 High level block diagram
 
![wifi_freertos](images/WiFi_FreeRTOS_UDP_high_level_design.jpg "High Level Block Diagram")
 
## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| AWS Core HTTP | AWS Core HTTP is used to communicate with the Host machine over a Wi-Fi network using the underlying On-chip Silex module. | rm_wifi_onchip_silex|
| FreeRTOS Heap 5 | FreeRTOS Heap is required for FreeRTOS standard Library Functions. | FreeRTOS Heap 5 |

For EK-RA2A2
| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| TinyCrypt (S/W Only) | The TinyCrypt Library provides an implementation for targeting constrained devices with a minimal set of standard cryptography primitives. | TinyCrypt (S/W Only)|

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| --------------------------------------- | ----------------- | ---------------| ---------- |
|configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x1000 | Main Program thread stack is configured to store the local variables of different functions in the code. |
|configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x2500 |Heap size is required for standard library functions to be used.|
|configuration.xml > Stacks > WiFi Thread > g_uart0 UART (r_sci_uart) > Properties > Settings > Property > Module g_uart0 UART (r_sci_uart) > General > Channel | 0 | 9 | PMOD0 is available on UART channel 9. |
|configuration.xml > Stacks > WiFi Thread > g_uart0 UART (r_sci_uart) > Properties > Settings > Property > Module g_uart0 UART (r_sci_uart) > Flow Control > CTS/RTS Selection | - | Hardware RTS | To enable flow control. |
|configuration.xml > Stacks > WiFi thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 0x1000 | Configured to store the local variables of different functions in the code. |
|configuration.xml > Stacks > WiFi Thread > WiFi Onchip Silex Driver using UART (rm_wifi_onchip_silex) > Properties > Settings > Property > Common > Size of RX buffer CMD port | 3000 | 1500 | To match TX buffer size. |
|configuration.xml > Stacks > WiFi Thread > MbedTLS (Crypto Only) > Properties > Settings > Property > Common > Message Authentication Code (MAC) > MBEDTLS_CMAC_C | Undefine | Define | Enables MBed TLS CMAC-C. |
|configuration.xml > Stacks > WiFi Thread > MbedTLS (Crypto Only) > Properties > Settings > Property > Common > Public Key Cryptography (PKC) > ECC > MBEDTLS_ECDH_C | Undefine | Define | Enables MBed TLS ECDH-C. |
|configuration.xml > Stacks > WiFi thread > Properties > Settings > Property > Common > Memory Allocation > Total Heap Size | 0 | 0x20000 | RAM is used to obtain memory allocation for secure libraries. |
|configuration.xml > Stacks > WiFi thread > Properties > Settings > Property > Common > Memory Allocation > Support Dynamic Memory Allocation | Disabled | Enabled | RTOS objects can be created using RAM that is automatically allocated from the FreeRTOS heap. |
|configuration.xml > Stacks > WiFi thread > Properties > Settings > Property > Common > General > Use Mutexes | Disabled | Enabled | Enabled to include mutex functionality.|
|configuration.xml > Stacks > WiFi thread > Properties > Settings > Property > Common > General > Use Recursive Mutexes | Disabled | Enabled | Enabled to include recursive mutex functionality. |

For EK-RA2A2:
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| --------------------------------------- | ----------------- | ---------------| ---------- |
|configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x400 | Main Program thread stack is configured to store the local variables of different functions in the code. |
|configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x1000 |Heap size is required for standard library functions to be used.|
|configuration.xml > Stacks > WiFi Thread > g_uart0 UART (r_sci_uart) > Properties > Settings > Property > Module g_uart0 UART (r_sci_uart) > General > Channel | 0 | 3 | PMOD2 is available on UART channel 3. |
|configuration.xml > Stacks > WiFi thread > Properties > Settings > Property > Common > Memory Allocation > Total Heap Size | 0 | 0x4000 | RAM is used to obtain memory allocation for secure libraries. |

For FPB-RA0E1:
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| --------------------------------------- | ----------------- | ---------------| ---------- |
|configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x400 | Main Program thread stack is configured to store the local variables of different functions in the code. |
|configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x400 |Heap size is required for standard library functions to be used. |
|configuration.xml > Stacks > WiFi Thread > g_uart0 UART (r_sau_uart) > Properties > Settings > Property > Module g_uart0 UART (r_sau_uart) > General > Channel | 0 | 0 | PMOD1 is available on UART channel 0. |
|configuration.xml > Stacks > WiFi thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 0x800 | Configured to store the local variables of different functions in the code. |
|configuration.xml > Stacks > WiFi thread > Properties > Settings > Property > Common > Memory Allocation > Total Heap Size | 0 | 0x4000 | RAM is used to obtain memory allocation for secure libraries. |
|configuration.xml > Stacks > WiFi Thread > WiFi Onchip Silex Driver using UART (rm_wifi_onchip_silex) > Properties > Settings > Property > Common > Size of RX buffer for socket | 4096 | 512 | Reduce size of RX buffer for socket to prevent memory overflow. |
|configuration.xml > Stacks > WiFi Thread > WiFi Onchip Silex Driver using UART (rm_wifi_onchip_silex) > Properties > Settings > Property > Common > Size of TX buffer for CMD port | 1500 | 512 | Reduce size of TX buffer for CMD port to prevent memory overflow. |
|configuration.xml > Stacks > WiFi Thread > WiFi Onchip Silex Driver using UART (rm_wifi_onchip_silex) > Properties > Settings > Property > Common > Size of RX buffer for CMD port | 3000 | 512 | Reduce size of RX buffer for CMD port to prevent memory overflow. |

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|[WIFI_On](https://renesas.github.io/fsp/group___w_i_f_i___a_p_i.html#ga2ee26776a00869c415da4a108031af1d) | This API is used to turn on Wi-Fi module and initializes the drivers. |
|[WIFI_Scan](https://renesas.github.io/fsp/group___w_i_f_i___a_p_i.html#ga4f1c2fb7ab3463f7d19afb506e945fd6) | This API is used to Perform a Wi-Fi network Scan. |
|[WIFI_ConnectAP](https://renesas.github.io/fsp/group___w_i_f_i___a_p_i.html#ga430d72715054198b6ce0885410b027a5) | This API is used to connect to the Wi-Fi Access Point (AP). |
|[WIFI_IsConnected ](https://renesas.github.io/fsp/group___w_i_f_i___a_p_i.html#gab293985fcadf84ff9a560e90271e0e0e) | This API is used to check if the Wi-Fi is connected.|
|[WIFI_GetIPInfo](https://renesas.github.io/fsp/group___w_i_f_i___a_p_i.html#ga5bcf899f31c02c57fd7e9c45f390b2c6) | This API is used to get IP configuration (IP address, NetworkMask, Gateway and DNS server addresses).|
|[WIFI_Ping](https://renesas.github.io/fsp/group___w_i_f_i___a_p_i.html#ga8680ac4cdb4455a2c03919c2f5417f72) |This API is used to ping an IP address in the network.|
|rm_wifi_onchip_silex_socket_create | This API is used to create a new socket instance.|
|rm_wifi_onchip_silex_udp_connect | This API is used to connect to a specific IP and Port using UDP socket.|
|rm_wifi_onchip_silex_send | This API is used to send data over UDP to a server.|
|rm_wifi_onchip_silex_recv | This API is used to receive data over TCP from a server.|
|rm_wifi_onchip_silex_socket_status_get | This API is used to get the socket status.|
|[WIFI_Off](https://renesas.github.io/fsp/group___w_i_f_i___a_p_i.html#ga93265c46f46cb899f08adc56608b35d5) |This API is used to turn off the Wi-Fi module.|


## Verifying operation ##
1. Download the PacketSender Software and install it on the Host PC. (https://packetsender.com/download#show)

2. Navigate to File > Settings on PacksetSender application and perform the following (described in the image).
	
	a) Enable UDP server
	
	b) Enter the desired server port number(preferably >10000)
	
	c) Click OK to complete the server setup
	
![wifi_freertos](images/PacketSender_settings.jpg "Packet Sender Settings")
	
3. The server is enabled which can be verified as below:

![wifi_freertos](images/PacketSender_setting_complete.jpg "Packet Sender Complete")
	
4. Connect the USB Debug port to view the status/error messages on J-Link RTT Viewer.

5. On the J-Link RTT Viewer, Either connect to the Wi-Fi Access point manually or connect via Wi-Fi scan.

![wifi_freertos](images/JLink_RTT_Viewer_1.jpg "Packet Sender Complete")

6. On successful connection, the EP displays the MCU IP address.

![wifi_freertos](images/JLink_RTT_Viewer_2.jpg "Packet Sender Complete")

7. Ping to any desired IP address to verifiy the network connectivity.

![wifi_freertos](images/JLink_RTT_Viewer_3.jpg "Packet Sender Complete")

8. On successful ping, Connect to the UDP server by providing IP address and Port number.

9. On successful connection, "UDP socket connected" is displayed on the J-Link RTT viewer.

![wifi_freertos](images/JLink_RTT_Viewer_4.jpg "Packet Sender Complete")
	
10. The EP then sends a "Hello" message to the server and continously listens for incoming messages.

11. Messages can be recieved from the client, with the IP address (mentioned in Step 6) and the port number(mentioned in step 10).

![wifi_freertos](images/JLink_RTT_Viewer_5.jpg "Packet Sender Complete")