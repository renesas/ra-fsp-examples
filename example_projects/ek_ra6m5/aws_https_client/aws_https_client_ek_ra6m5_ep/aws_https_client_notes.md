# Introduction #

This project demonstrates the basic functionality of AWS https client library with ethernet driver on Renesas RA MCUs 
based on Renesas FSP using FreeRTOS. aws client is used to connect to https adafruit server which is cloud platform. 
On successful connection, menu is displayed enabling user to send GET,PUT,POST requests to adafruit server. On POST/PUT 
request, MCU Die temperature is read via(using) ADC and uploaded to server. On GET request, the last MCU die temperature 
data is read from the adafruit server. JlinkRTTViewer is used to display the status and responses of the requests made to server.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the aws_https_client example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit
* Micro USB cable
* Ethernet Switch
* LAN Cable
* HOST Machine

Refer to [readme.txt](./readme.txt) on information on how to connect the hardware.

### Software ###
1. Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)
2. Refer to **Special Topics** for obtaining the certificates and key which is required to update in the code. 


 
## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)


# Project Notes #

## System Level Block Diagram ##
 High level block diagram
 
 
![aws_https_client](images/aws_https_client_high_level.jpg "aws https client Block Diagram")




## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
|AWS HTTPS Client|To get access of aws client library for client connection |AWS Https Client|
|FreeRTOS + TCP|To get access of TCP/IP network library to form network layer|rm_freertos_plus_tcp|
|Ethernet Driver| This is dependent module of FreeRTOS+TCP to get access of physical layer of FSP board with the help of ethernet driver |r_ether & r_ether_phy|
|Secure Sockets | Secure sockets are used to form secure connection|Secure Sockets|
|FreeRTOS+Crypto|This is used for secure connection by accessing MbedTLS and MbedCrypto libraries|FreeRTOS+Crypto|
|MbedTLS|This is dependent module of FreeRTOS+Crypto which is used for server/client certificate parsing|mbedTLS|
| Mbed Crypto | This is dependent module of MbedTLS which is used for secure TLS connection | rm_psa_crypto |
| Littlefs | To store rsa keys in flash memory | rm_littlefs_flash |
| ADC |ADC module is used to demonstrate that to read internal temperature sensor value and send it to the server  | r_adc   |



## Module Configuration Notes ##
This section describes FSP configurator properties which are important or different than those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> BSP > Properties > Settings > Property >MAin Stack Size (bytes)| 0x400 |0x10000| Main Program thread stack is configured to store the local variables of different functions in the code |
| configuration.xml -> BSP > Properties > Settings > Property > Heap Size (bytes)| 0 |0x2000| Heap size is required for standard library functions to be used |
| configuration.xml -> Ethernet Thread -> Properties > Settings > Property >Memory Allocation > Support Dynamic Mermory Allocation| Disabled | Enabled | RTOS objects can be created using RAM that is automatically allocated from the FreeRTOS heap |
| configuration.xml -> Ethernet Thread -> Properties > Settings > Property >Memory Allocation > Total Heap Size | 0 | 0x40000 | RAM is used to obtain memory allocation for secure libraries |
| configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > AWS IOT Common > Properties > Settings > Property > IOT Thread Default Stack Size | 512 | 4096 | Stack size for new thread to store network information| 
| configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > AWS IOT Common > Properties > Settings > Property > IOT Network Receive task Stack Size | 512 | 4096 | Stack size for storing data from receive task|
| configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > FreeRTOS+TCP  > Properties > Settings > Property > DHCP callback function | Disable | Enable |DHCP callback function is required to obtain dynamic ip address |
|  configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > FreeRTOS+TCP  > Properties >  Settings > Property > Let TCP use windowing Mechanism | Disable | Enable | For Flow control use the TCP windowing mechanism|
|  configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > FreeRTOS+TCP  > Properties >  Settings > Property > FreeRTOS_SendPingRequest() is available |Disable|Enable|To Support the sending of Ping request this needs to chosen as enabled.|
|configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > FreeRTOS+TCP  > Properties >  Settings > Property > The socket semaphore to unblock the MQTT task | Enable | Disable |   MQTT task is not support in https cleint application this needs to chosen as disabled |
|configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > FreeRTOS+TCP > g_ether0 Ethernet Driver on r_ether  > Properties >  Settings > Property > Number of TX buffer | 1 | 8 |Buffer size increased for faster processing|
|configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > FreeRTOS+TCP > g_ether0 Ethernet Driver on r_ether  > Properties >  Settings > Property > Number of RX buffer | 1 | 8 |Buffer size increased for faster processing|
|configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > FreeRTOS+TCP > g_ether_phy0 Ethernet Driver on r_ether_phy  > Properties >  Settings > Property > Reference Clock |Disable |Enable|Selected to use the RMII reference clock|
|configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > FreeRTOS+TCP > g_ether_phy0 Ethernet Driver on r_ether_phy  > Properties >  Settings > Property > PHI-LSI Address | 0 | 1 | which specifies the address of the PHI-LSI address|
|configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > Secure Sockets TLS Support [Optional for Wifi] > AWS PKCS11 to MbedTLS3 > AWS PKCS11 PAL on LittleFS > LittleFS > Properties >  Settings > Property > Warning Messages | Disabled | Enabled | To print the warning messages of LittleFS this needs to chosen as enabled|
|configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > Secure Sockets TLS Support [Optional for Wifi] > AWS PKCS11 to MbedTLS3 > AWS PKCS11 PAL on LittleFS > LittleFS > Properties >  Settings > Property > Error Messages | Disabled | Enabled | To print the error messages of LittleFs this needs to chosen as enabled|
|configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > Secure Sockets TLS Support [Optional for Wifi] > AWS PKCS11 to MbedTLS3 > FreeRTOS+Crypto > MbedTLS > Properties > Settings > Property > Common > General > mbedtls_tls_default_allow_sha1_in_certificates | Undefine | Define | Allow SHA1 in Certificates to update client and server certificate for secure connection |
| configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > Secure Sockets TLS Support [Optional for Wifi] > AWS PKCS11 to MbedTLS3 > FreeRTOS+Crypto > MbedTLS > Properties > Settings > Property > Common >  Key Exchange > MBEDTLS_KEY_EXCHANGE_PSK_ENABLED | Undefine | Define |Enable the PSK based ciphersuite modes in SSL / TLS. |
| configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > Secure Sockets TLS Support [Optional for Wifi] > AWS PKCS11 to MbedTLS3 > FreeRTOS+Crypto > Mbed Crytpo > Properties > Settings > Property > Common > Cipher > Alternate > MBEDTLS_GCM_ALT | Define | Undefine | MBEDTLS_GCM_ALT does not bind to the cipher keys |
| configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > Secure Sockets TLS Support [Optional for Wifi] > AWS PKCS11 to MbedTLS3 > FreeRTOS+Crypto > Mbed Crytpo > Properties > Settings > Property > Common > Public Key Cryptography (PKC) > ECC > MBEDTLS_ECDH_C | Undefine | Define | Mbed TLS implements ECDH algorithm |
| configuration.xml -> Ethernet Thread > AWS HTTPS Client > AWS Platform Network Abstraction > Secure Sockets on FreeRTOS Plus TCP > Secure Sockets TLS Support [Optional for Wifi] > AWS PKCS11 to MbedTLS3 > FreeRTOS+Crypto > Mbed Crytpo > Properties > Settings > Property > Common > Public Key Cryptography (PKC) > RSA > MBEDTLS_PK_RSA_ALT_SUPPORT | Undefine | Define |To Support external private RSA keys |
|configuration.xml -> Ethernet Thread > g_adc ADC Driver on g_adc > Properties > Settings > Module g_adc ADC Driver on r_adc > General > Mode |Single Scan | Continuous Scan | Continuous mode to be selected for continuous reading of adc value (MCU Die Temperature)|
|configuration.xml -> Ethernet Thread > g_adc ADC Driver on g_adc > Properties > Settings > Module g_adc ADC Driver on r_adc > input > Channel Scan Mask (channel availability varies by MCU) > Temperature Sensor | Uncheck | Check | Enable the internal temperature sensor to read the adc value |



## API Usage ##

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|R_ADC_ScanCfg| This API is used to configure the ADC scan parameters|
|R_ADC_ScanStart|This API is used to start scanning of configured adc channel|
|R_ADC_Read|This API is used to read the adc data from the configured channel|
|FreeRTOS_IsNetworkUp|This API checks the network status|
|FreeRTOS_gethostbyname| This API is used to resolve the host name to ip address|
|FreeRTOS_inet_ntoa|This API is used to convert an ip address expressed in decimal dot notation|
|FreeRTOS_SendPingRequest|This API is used to send a ping request to remote pc|
|R_ETHER_LinkProcess|This API is used to process the TCP/IP link|
|IotHttpsClient_GetUrlPath|This API is used to perform the https request to the server|
|IotHttpsClient_GetUrlAddress|This API is used to retrieve the url address|
|IotHttpsClient_Init|This API is used to initialize the https library|
|IotHttpsClient_Connect|This API is used to connect request to the server|
|IotHttpsClient_Disconnect|This API is used to disconnect from the connected server|
|IotHttpsClient_Cleanup|This API is used to celanup the https library|
|IotHttpsClient_InitializeRequest|This API is used to initialize the https client request to the server with the user credentials|
|IotHttpsClient_AddHeader|This API is used to add the header in JSON format for https client request to the server|
|IotHttpsClient_SendSync|This API is used to send in synchronous connection|
|IotHttpsClient_ReadResponseStatus|This API is used to read the response status from the server|
|IotClock_SleepMs | This API is used to delay for few seconds to retry the server connection|



## Verifying operation ##

Import, Build and Debug the EP(*see section Starting Development* of **FSP User Manual**). After running the EP, open rtt viewer to see the output.

Below images showcases the output on JLinkRTT_Viewer :


![aws_https_client](images/rtt_viewer_banner.jpg "RTT viewer starting page")

![aws_https_client](images/rtt_viewer_post_ip_init.jpg "RTT viewer ping page")

![aws_https_client](images/rtt_viewer_put_request.jpg "RTT viewer put request page")

![aws_https_client](images/rtt_viewer_post_request.jpg "RTT viewer post request page")

![aws_https_client](images/rtt_viewer_get_request.jpg "RTT viewer get request page")

Below image show the adafruit server with feed data:

![aws_https_cleint](images/adafruit_feed_data.jpg "adafruit server page")




## Special Topics ##

**1. Obtaining Adafruit server credentials**:

Following steps guide you how to obtain the username and AIO key.

1.Go to https://io.adafruit.com. Click on **Get Started for Free** option as shown in the  below image

![aws_https_client](images/adafruit_getstart.jpg "adafruit getting started")

2. Create an account by providing the requested details to obtain user credintails viz., **username** and **password** 
3. After successful creation of account, user name will display on top of the page as shown in below image. Click on **IO**, dashboard will displays with the following options **Feeds , Dashboards, My key** etc.


 ![aws_https_client](images/adafruit_dashboard.jpg "adafruit dashboard")
 
 4.  Click on My Key option to see your username and Active key. These two details are important for communicating with adafruit server. If the key is compromised, we can generate the new key by clicking on the Regenerate key option as shown in below image.

 ![aws_https_client](images/adafruit_io_key.jpg "adafruit io key") 
 
 5. After obtaining the user name and io key. User has to update the following details at respective url macros in the aws_https_client_ep/src/user_app.h file as shown in the below image.

![aws_https_cleint](images/user_config_macro.jpg "user configuration macros")




**2. Obtaining Server Certificate:**

1. Open browser and copy paste the following url http://cacerts.geotrust.com/GeoTrustRSACA2018.crt. Server certificate with .crt extension will be downloaded with the file name **GeoTrustRSACA2018.crt**

2. After downloaded the .crt file, need to be convert to .pem format using OpenSSL.

3. OpenSSL can be downloaded from  https://www.openssl.org/source/. depends on the Operating System, required installer can be downloaded and installed.

4. Copy the downloaded certificate to the bin folder of your installed openssl.

5. Open the cmd prompt in Administrator mode from the bin folder as shown in the below image.

 ![aws_https_client](images/openssl_win.jpg "OpenSSL window")

 6. Type the conversion command :**Openssl.exe x509 -inform DER -outform PEM -in GeoTrustRSACA2018.crt -out GeoTrustRSACA2018.crt.pem** as shown below image
 
 ![aws_https_client](images/openssl_conversion_cmd.jpg "OpenSSL conversion command")
 
 7. GeoTrustRSACA2018.crt.pem file will be generated in your bin folder as shown in below image.
 
![aws_https_client](images/openssl_bin.jpg "openssl bin folder")

8. Open the converted certificate file with notepad and copy the content and update in the aws_https_client_ep\src\usr_app.h file at the **IOT_HTTPS_TRUSTED_ROOT_CA** macro as shown in below image.

![aws_https_client](images/server_certificate.jpg "server certificate macro updation")




**3. Obtaining Client Certificate and Private Key:**

1. Open the cmd prompt from the bin folder of installed Openssl. 
![aws_https_client](images/openssl_win.jpg "OpenSSL window")

2. To generate the private key and CSR certificate use the command **openssl req -newkey rsa:2048 -nodes -keyout clientside.key -x509 -days 365 -out clientside.crt**  as shown below.
**NOTE:** Output file can be any name.

![aws_https_client](images/open_ssl_client_key.jpg "OpenSSL Client Key")

3. clientside.key and clientside.crt files are generated in binary folder. Verify client certificate is properly generated by using command **openssl x509 -text -noout -in clientside.crt** as shown below.

![aws_https_client](images/openSSL_Certificate_validation.jpg "OpenSSL Client Key validation")

4. Generated client certificate is in .crt format. It has to be converted to .pem format. Conversion can be done from .crt to .der and then .der to .pem. 
To convert from .crt to .der, Use the command **openssl x509 -in clientside.crt -outform der -out clientside.der** as shown below.

![aws_https_client](images/openSSL_crt_der_conversion.jpg "OpenSSL format conversion")

5. To convert .der to .pem use the command **openssl x509 -in clientside.crt -outform der -out clientside.der** as shown below.

![aws_https_client](images/openSSL_der_pem_conversion.jpg "OpenSSL format conversion")

6. The generated files can be found in bin folder of OpenSSL installed software as shown in below image.

![aws_https_client](images/openSSL_Generated_files.jpg "OpenSSL Generated files")

7. Open the generated files in notepad and copy the content and paste in the aws_https_client_ep/src/usr_app.h 

 i . Copied client certificate to be update at the CLIENT_CERTIFICATE_PEM macro as shown in below image.
 
 ![aws_https_client](images/update_client_certificate.jpg "Update client certificate pem")
 
 ii. Copied client key to be update at the CLIENT_KEY_PEM macro as shown in below image.
 
  ![aws_https_client](images/update_client_key.jpg "Update client key pem")
  
  
**NOTE:** Client Certificate and client Key are required for AWS Client application to authenticate server in secure connection. If missing of both, then it cannot be connect to server instead return an error as no certificates were found.