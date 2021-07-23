# Introduction #

This project demonstrates the basic functionality of NetX Duo HTTPS client with ethernet driver on Renesas RA MCUs
based on Renesas FSP using AzureRTOS. HTTPS Client is used to connect to HTTPS Adafruit IO server which is a cloud platform.
On successful connection, menu is displayed enabling user to send GET, PUT, POST requests to Adafruit IO server. On POST/PUT
request, MCU Die temperature is read using ADC and uploaded to server. On GET request, the last MCU die temperature data
is read from the Adafruit IO server. JlinkRTTViewer is used to display the status and responses of the requests made to server.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the NetX_https_client example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit with Ethernet Support
* Micro USB cable
* Ethernet Switch
* Ethernet Cable CAT5/6 (LAN Cable)
* HOST Machine
* Router for Internet access or LAN connecting to the Internet

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
1. Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)
2. Refer to **[Special Topics](#special-topics)** for obtaining the certificates and key which is required to update in the code. 


 
## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)


# Project Notes #

## System Level Block Diagram ##
 High level block diagram
 
 
![NetX_https_client](images/NetX_https_client_high_level.jpg "NetX https client Block Diagram")




## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| NetX Duo Web HTTP/HTTPS Client| To get access of NetX HTTPS client library for client connection. | NetX Duo Web HTTP/HTTPS Client |
| NetX Duo IP Instance | For IP communication the NetX IP instance needs to be used. HTTPS Client uses the IPV4. | NetX Duo IP|
| NetX Duo Packet Pool | Packet Pool Module is used to send and receive the created data packets over TCP network.  | NetX Duo Packet Pool |
| NetX Duo Ethernet driver | Ethernet connection should require as physical connection to connect over network. | NetX Duo Ethernet Driver |
| NetX Duo DHCP IPv4 Client | DHCPv4 module is used to obtain IP address and to automate the process of configuring devices on IP networks, by using network services such as DNS, NTP, and any communication protocol based on UDP or TCP | NetX Duo DHCPv4 Client |
| NetX Duo DNS Client | DNS Module is used as network service to get the IP address of the Domain (Server) to which the HTTPS client is getting connected. | NetX Duo DNS Client |
| ADC |ADC module is used to demonstrate that to read internal temperature sensor value and send it to the server. | r_adc  |



## Module Configuration Notes ##
This section describes FSP configurator properties which are important or different than those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|configuration.xml -> BSP > Properties > Settings > Property > RA Common > Main Stack Size (bytes)| 0x400 |0x1000| Main Program thread stack is configured to store the local variables of different functions in the code. |
|configuration.xml -> BSP > Properties > Settings > Property > RA Common > Heap Size (bytes)| 0 |0x1000| Heap size is required for standard library functions to be used. |
|configuration.xml -> Https Client Thread > g_web_http_client0 NetX Duo Web HTTP/HTTPS Client > Properties > Settings > Module g_web_http_client0 NetX Duo Web HTTP/HTTPS Client > TCP socket window size (bytes) | 1024 | 8192 | Window size is increased to accommodate successful acknowledgements from host and server. |
|configuration.xml -> Https Client Thread > g_web_http_client0 NetX Duo Web HTTP/HTTPS Client > g_packet_pool0 NetX Duo Packet Pool Instance > Properties > Settings > Module g_packet_pool0 NetX Duo Packet Pool Instance > No of Packets in Pool | 16 | 50 | To accommodate all the data sent and received from the server. |
|configuration.xml -> Https Client Thread > Properties > Settings > Thread > Stack Size | 1024 | 4096 | The thread’s stack area must be large enough to handle its worst-case function call nesting and local variable usage. |
|configuration.xml -> Https Client Thread > Properties > Settings > Thread > Priority | 1 | 2 | HTTPS Client thread priority is lowered to allow the IP thread to process incoming packets at the fastest rate possible. |
|configuration.xml -> Https Client Thread > g_web_http_client0 NetX Duo Web HTTP/HTTPS Client > Properties > Settings > Common > HTTP > Common > Fragmentation Option | Don't Fragment | Fragment Okay | Provides fragmentation for HTTP in the thread. |
|configuration.xml -> Https Client Thread > g_web_http_client0 NetX Duo Web HTTP/HTTPS Client > Properties > Settings > Common > Web HTTP > Client > HTTPS Support | Disable | Enable | Enables HTTPS support for the Web HTTP Client. | 
|configuration.xml -> Https Client Thread > g_web_http_client0 NetX Duo Web HTTP/HTTPS Client > NetX Secure > Properties > Settings > Self Signed Certificates | Disable | Enable | This option allows TLS to accept self-signed certificates from a remote host. |
|configuration.xml -> Https Client Thread > g_web_http_client0 NetX Duo Web HTTP/HTTPS Client > NetX Secure > Properties > Settings > ECC Cipher Suite | Enable | Disable | When disabled, this option removes all TLS logic for Elliptic Curve Cryptography (ECC) ciphersuites. |
|configuration.xml -> Https Client Thread > g_web_http_client0 NetX Duo Web HTTP/HTTPS Client > NetX Secure > Properties > Settings > Disable Server | Enable | Disable | When disabled, this option removes all TLS stack code related to TLS Server mode, reducing code and data usage.|
|configuration.xml -> Https Client Thread > g_web_http_client0 NetX Duo Web HTTP/HTTPS Client > NetX Crypto Software Only > Properties > Settings > Standalone Usage | Use Standalone Crypto Only | Use with TLS | Use TLS for NetX HTTPS client connection. |
|configuration.xml -> Https Client Thread > g_adc ADC Driver on g_adc > Properties > Settings > Module g_adc ADC Driver on r_adc > General > Mode |Single Scan | Continuous Scan | Continuous mode to be selected for continuous reading of adc value (MCU Die Temperature)|
|configuration.xml -> Https Client Thread > g_adc ADC Driver on g_adc > Properties > Settings > Module g_adc ADC Driver on r_adc > input > Channel Scan Mask (channel availability varies by MCU) > Temperature Sensor | Uncheck | Check | Enable the internal temperature sensor to read the adc value |
|configuration.xml -> RTT Thread > Properties > Settings > Thread > Priority | 1 | 3 | RTT thread priority is lowered to allow the HTTPS Client and IP threads to process incoming packets at the fastest rate possible. |




## API Usage ##

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|nx_system_initialize|This function initializes the various components and system data structures.|
|nx_packet_pool_create|This API creates a packet pool of the specified packet size in the memory area supplied by the user.|
|nx_web_http_client_create|This API creates an HTTP Client instance on the specified IP instance.|
|nx_web_http_client_delete|This API deletes a previously created HTTP Client instance.|
|nx_web_http_client_request_initialize|This API creates a custom HTTP request and associates it with the HTTP Client instance.|
|nx_web_http_client_response_body_get|This API retrieves the next packet of content data.|
|nx_web_http_client_request_header_add|This API adds a custom header (in the form of a field name and value) to a custom HTTP request created with nx_web_http_client_request_initialize().
|nx_web_http_client_request_send|This API creates a custom HTTP request and associates it with the HTTP Client instance.|
|nx_web_http_client_request_packet_allocate|This API attempts to allocates a packet for Client HTTP(S).|
|nx_web_http_client_request_packet_send|This API sends a custom HTTP(S) request data packet created with nx_web_http_client_request_packet_allocate() to the server specified in the nx_web_http_client_secure_connect().|
|nx_web_http_client_secure_connect|This API opens a secured TLS session to an HTTPS server for custom requests.|
|nx_ip_address_get|This API retrieves IP address and its subnet mask of the primary network interface.|
|nx_packet_data_append|This API appends data to the end of the specified packet.|
|nx_packet_data_extract_offset|This API extracts data from packet via an offset.|
|nx_packet_release|This API releases a previously allocated packet.|
|nx_ip_create|This API creates an IP instance with the user supplied IP address and network driver.|
|nx_icmp_enable|This service enables the Internet Control Message Protocol (ICMP) component for the specified IP instance.|
|nx_ip_status_check|This service checks and optionally waits for the specified status of a previously created IP instance. |
|nx_dhcp_create|This service creates a DHCP instance for the previously created IP instance.|
|nx_dhcp_packet_pool_set|This service allows the application to create the DHCP Client packet pool|
|nx_dhcp_start|This service starts DHCP processing on all interfaces enabled for DHCP.|
|nx_dhcp_user_option_retrieve|This service retrieves the specified DHCP option from the DHCP options buffer on the first interface enabled for DHCP found on the DHCP Client record.|
|nx_dns_create|This service creates a DNS Client instance for the previously created IP instance.|
|nx_dns_packet_pool_set|This service sets a previously created packet pool as the DNS Client packet pool.|
|nx_dns_server_add|This service adds an IPv4 DNS Server to the server list.|
|nx_dns_host_by_name_get|This service requests IP address, from one or more DNS Servers previously specified by the application.|
|nx_secure_tls_session_create|This service initializes an NX_SECURE_TLS_SESSION structure instance for use in establishing secure TLS communications over a network connection.|
|nx_secure_tls_session_packet_buffer_set|This service associates a packet reassembly buffer to a TLS session.|
|nx_secure_x509_certificate_initialize|This service initializes an NX_SECURE_X509_CERT structure from a binary-encoded X.509 digital certificate for use in a TLS session.|
|nx_secure_tls_trusted_certificate_add|This service adds an initialized NX_SECURE_X509_CERT structure instance to a TLS session.|
|nx_secure_tls_remote_certificate_allocate|This service adds an uninitialized NX_SECURE_X509_CERT structure instance to a TLS session for the purpose of allocating space for certificates provided by a remote host during a TLS session.|
|R_ADC_ScanCfg| This API is used to configure the ADC scan parameters.|
|R_ADC_ScanStart|This API is used to start scanning of configured ADC channel.|
|R_ADC_Read|This API is used to read the ADC data from the configured channel.|


## Verifying operation ##


Import, Build and Debug the EP (*see section Starting Development* of **FSP User Manual**). After running the EP, open RTT viewer to see the output on the console.

**Note**: User must create an account on the Adafruit (https://io.adafruit.com), before running the Application. 
The details of creating the Account and getting the required credentials and IO Keys for successful running of the application are described in the **[Special Topics](#special-topics)** Section.

Below images showcase the output on J-Link RTT Viewer:

EP Banner and Info:

![NetX_https_client](images/rtt_viewer_banner.jpg "RTT viewer starting page")

Initialization and Menu Options:

![NetX_https_client](images/rtt_viewer_init.jpg "RTT viewer initialization page")

Post Request: 

![NetX_https_client](images/rtt_viewer_post_request.jpg "RTT viewer Post request page")

Put Request:

![NetX_https_client](images/rtt_viewer_put_request.jpg "RTT viewer Put request page")

Get Request:

![NetX_https_client](images/rtt_viewer_get_request.jpg "RTT viewer Get request page")

After executing the HTTP menu options, the updated data will be shown in the user feed on the Adafruit IO server. 
Below image shows the Adafruit server with updated data:

![NetX_https_client](images/adafruit_feed_data.jpg "adafruit server page")


## Special Topics ##

**1. Obtaining Adafruit server credentials**:

Following steps guide you on how to obtain the username and AIO key from Adafruit IO server.

1.Go to https://io.adafruit.com. Click on **Get Started for Free** option as shown in the  below image

![NetX_https_client](images/adafruit_getstart.jpg "adafruit getting started")

2. Create an account by providing the requested details to obtain user credintails viz., **username** and **password** 

3. After successful creation of account, user name will display on top of the page as shown in below image. 
   Click on **IO**, Dashboard will display with the following options **Feeds , Dashboards, My key** etc.

 ![NetX_https_client](images/adafruit_dashboard.jpg "adafruit dashboard")
 
4. Click on My Key option to see your **username** and **Active key**. These two details are important for communicating with Adafruit server. If the key is compromised, we can generate the new key by clicking on the Regenerate key option as shown in below image.

 ![NetX_https_client](images/adafruit_io_key.jpg "adafruit io key") 
 
5. For creating a new feed, go to Feeds -> view all, as shown in the image below:

 ![NetX_https_client](images/create_feed.jpg "creating a feed")
 
6. Then click on the "+New Feed" button, a dialog box will appear to Create a new Feed. Fill the name and description of feed and click on "Create" button, as shown in below image.
 
 ![NetX_https_client](images/create_new_feed.jpg "creating a new feed")

7. After obtaining the user name, io key, feed name from above mentioned steps(3-6). User has to update the following details at respective url macros in the **NetX_https_client_ep/src/user_app.h** file as shown in the below image.

![NetX_https_client](images/user_app_config.jpg "user configuration macros")


**2. Obtaining Server Certificate:**

1. Open browser and copy paste the following url http://cacerts.geotrust.com/GeoTrustRSACA2018.crt. Server certificate with .crt extension will be downloaded with the file name **GeoTrustRSACA2018.crt**

2. After downloaded the .crt file, need to be convert to .pem format using OpenSSL.

3. OpenSSL can be downloaded from  https://www.openssl.org/source/. depends on the Operating System (mainly for Linux), required installer can be downloaded and installed.

   For Windows users, OpenSSL can be downloaded from https://slproweb.com/products/Win32OpenSSL.html.

4. Copy the downloaded certificate to the bin folder of your installed openssl.

5. Open the `cmd prompt` in Administrator mode from the bin folder as shown in the below image.

 ![NetX_https_client](images/openssl_win.jpg "OpenSSL window")

6. Type the conversion command :**Openssl.exe x509 -inform DER -outform PEM -in GeoTrustRSACA2018.crt -out GeoTrustRSACA2018.crt.pem** as shown below image
 
 ![NetX_https_client](images/openssl_conversion_cmd.jpg "OpenSSL conversion command")
 
7. GeoTrustRSACA2018.crt.pem file will be generated in your bin folder as shown in below image.
 
 ![NetX_https_client](images/openssl_bin.jpg "OpenSSL bin folder")

8. Convert the Certificate into a C Hex array, with a tool available at https://github.com/downloads/tristan2468/Hexy/hexy.exe.

9. The tool requires no installation. So, run it and the following window will appear

 ![NetX_https_client](images/hex_to_c_array.jpg "Hex to C array converter")

10. Now, drop the PEM encoded certificate on the window. 

 ![NetX_https_client](images/hexy_conversion.jpg "Certificate conversion to hex format")

11. The file will generate a similarly named file with a “.h” extension, which contains the C array.

 ![NetX_https_client](images/converted_certificate.jpg "Converted certificate in .h file")

12. Update the contents from the .h file in the **NetX_https_client_ep\src\https_client_app.c** file at the **ca_cert_der** array as shown in below image.

 ![NetX_https_client](images/certificate_array.jpg "certificate array in https_client_app.h")




