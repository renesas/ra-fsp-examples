# Introduction #
This example project demonstrates the functionalities of the HTTP Client within the DA16200 module, employing the FSP Integrated Onchip DA16XXX Middleware driver, and leveraging FreeRTOS on Renesas RA MCUs. EP uses the US159-DA16200EVZ Ultra Low Power Wi-Fi Pmod™ module to connect to an Access Point and communicate with a web server using HTTP methods (GET, PUT, POST). After sending each request, EP waits for the HTTP response and prints it to the terminal. Additionally, status and failure messages are displayed on the terminal.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of the operation.

## Required Resources ##
To build and run the example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.5.0
* e2 studio: Version 2026-04.2
* LLVM Embedded Toolchain for ARM: Version 21.1.1
* Terminal Console Application: Tera Term or a similar application

Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for basic information on setting up and using the above software.

### Hardware ###
* Supported RA boards: EK-RA6M5, EK-RA2A2, EK-RA2L1, EK-RA4E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA8D1, EK-RA8M1, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T2, MCK-RA6T3, MCK-RA8T1, EK-RA8P1, EK-RA4C1, MCK-RA8T2, EK-RA8M2, EK-RA8D2, FPB-RA0L1, FPB-RA6E2.
  * 1 x Renesas RA board.
  * 1 x USB cable for programming and debugging (USB cable type varies by board model).
  * 1 x Wi-Fi router Access Point (AP) with internet access. 
    * Note: Please ensure the Wi-Fi Access Point credentials are available before running the project.
  * 1 x US159-DA16200EVZ Ultra Low Power Wi-Fi Pmod™ (Part Number: US159-DA16200MEVZ) (SDK v3.2.9.2).
    * Note: Please ensure the Pmod™ SDK is up-to-date before executing the project. For SDK update instructions, please refer to Section 5, 6, and 7 in the provided link: https://www.renesas.com/en/document/apn/da16200da16600-sdk-update-guide

### Hardware Connections ###
* Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and debugging.

* For EK-RA4M1, EK-RA6M1, EK-RA6M2:
  * Plug in the US159-DA16200MEVZ to the PMODA connector on the MCU evaluation kit and be careful to align Pin 1 on the module to Pin 1 on the MCU kit.

* For EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, FPB-RA6E2:
  * For FPB-RA6E2: 
    * The user must close E6 and open E5 to use PMOD1 for UART.  
  * Plug in the US159-DA16200MEVZ to the PMOD1 connector on the MCU evaluation kit and be careful to align Pin 1 on the module to Pin 1 on the MCU kit.

* For EK-RA6M5, EK-RA2L1, EK-RA4E2, EK-RA6E2, EK-RA6M4, EK-RA8D1, EK-RA8M1, FPB-RA4E1, FPB-RA6E1, MCK-RA4T1, MCK-RA6T3, EK-RA8P1, EK-RA8M2, EK-RA8D2:
  * For EK-RA8M2: 
    * The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
  * Plug in the US159-DA16200MEVZ to the PMOD2 connector on the MCU evaluation kit and be careful to align Pin 1 on the module to Pin 1 on the MCU kit.

* For MCK-RA8T2:
  * Plug in the US159-DA16200MEVZ to the PMOD Type3A/6A (CN6) connector on the MCU evaluation kit and be careful to align Pin 1 on the module to Pin 1 on the MCU kit.

* For FPB-RA0L1:
	* The user must place jumper J9 on pins 1-2 to use the on-board debug functionality.
	* The user must close E34, E36, E35 and open E40 to use P101, P100, P013 on PMOD1.
	* Plug in the US159-DA16200MEVZ to the PMOD1 (J19) connector on the MCU evaluation kit and be careful to align Pin 1 on the module to Pin 1 on the MCU kit.

* For EK-RA2A2:  
  * Connect
    1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P501/TXD3 (J1:1)
    2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P500/RXD3 (J4:19)
    3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (J18:6)
    4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (J4:1)
    5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P203/RESET (J3:9)  
  * Note: If using Pmod2 on EK-RA2A2, it is necessary to solder jumper Ex3.

* For MCK-RA6T2:  
  * Connect
    1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> PD05/TXD9 (CN10:3)
    2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> PD06/RXD9 (CN10:2)
    3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (CN6:32)
    4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (CN6:34)
    5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> PA12/RESET (CN6:12)

* For MCK-RA8T1:  
  * Connect
    1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P112/TXD0 (CN2:1)
    2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P113/RXD0 (CN1:33)
    3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (CN1:30)
    4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (CN1:23)
    5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P114/RESET (CN1:31)

* For EK-RA4C1:  
  * The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use the on-board debug functionality.  
  * The user must close E52 to use P305 for RST pin of PMOD2.
  * Connect
    1) Renesas US159-DA16200MEVZ UART Pmod RXD (J1:2) ----> P115/TXD1 (J2:35)
    2) Renesas US159-DA16200MEVZ UART Pmod TXD (J1:3) ----> P608/RXD1 (J2:37)
    3) Renesas US159-DA16200MEVZ UART Pmod GND (J1:5) ----> GND (J2:1)
    4) Renesas US159-DA16200MEVZ UART Pmod VCC (J1:6) ----> VCC (J1:1)
    5) Renesas US159-DA16200MEVZ UART Pmod RES (J1:8) ----> P305/RESET (J2:38)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
 High level block diagram
 
![wifi_freertos](images/http_client_onchip_DA16200_HLD.jpg "High Level Block Diagram")
 
## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| HTTP Client/Server on DA16XXX | The HTTP Onchip da16xxx Middleware is used to communicate with the HTTP server over a Wi-Fi network using the DA16XXX module. | rm_http_onchip_da16xxx |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different from those selected by default.

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| --------------------------------------- | ----------------- | ---------------| ---------- |
| configuration.xml > HTTP Client Thread > Properties > Settings > Property > Common > General > Use Mutexes | Disabled | Enabled | Enabled to include mutex functionality. |
| configuration.xml > HTTP Client Thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 2048 | RAM is used to obtain memory allocation. |
| configuration.xml > HTTP Client Thread > HTTP Client on DA16XXX (rm_http_onchip_da16xxx) > Wifi DA16XXX Framework Driver (rm_wifi_da16xxx) > DA16XXX transport on UART (rm_at_transport_da16xxx_uart) > Properties > Settings > Property > Module Reset Port | 06 | 00 | Setup reset port for the MCU. |
| configuration.xml > HTTP Client Thread > HTTP Client on DA16XXX (rm_http_onchip_da16xxx) > Wifi DA16XXX Framework Driver (rm_wifi_da16xxx) > DA16XXX transport on UART (rm_at_transport_da16xxx_uart) > Properties > Settings > Property > Module Reset Pin | 03 | 05 | Setup reset pin for the MCU. |
| configuration.xml > g_uart0 UART (r_sci_uart) > Module g_uart0 UART (r_sci_uart) > General > Channel | 0 | 9 | Select UART channel. |
| configuration.xml > g_uart0 UART (r_sci_uart) > Pins > TXD9 | P109 | P109 | Select TX pin. |
| configuration.xml > g_uart0 UART (r_sci_uart) > Pins > RXD9 | P110 | P110 | Select RX pin. |

The table below lists the FSP provided API used at the application layer in this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| WIFI_On | This API is used to turn on Wi-Fi module and initialize the drivers. |
| WIFI_Scan | This API is used to perform a Wi-Fi network scan. |
| WIFI_ConnectAP | This API is used to connect to the Wi-Fi Access Point (AP). |
| WIFI_IsConnected | This API is used to check if the Wi-Fi is connected. |
| WIFI_GetIPInfo | This API is used to get IP configuration (IP address, NetworkMask, Gateway and DNS server addresses). |
| WIFI_Disconnect | This API is used to disconnect from the currently connected Access Point. |
| WIFI_Off | This API is used to turn off the Wi-Fi module. |
| RM_HTTP_DA16XXX_Open | This API is used to initialize the DA16XXX on-chip HTTP Client service. |
| RM_HTTP_DA16XXX_Send | This API is used to send the HTTP request with the configured buffers. |
| RM_HTTP_DA16XXX_Close | This API is used to close the DA16XXX HTTP Client service. |

## Verifying Operation ##
1. Set up the hardware connection properly mentioned in [Hardware Connections](#hardware-connections).
2. Import, generate, build, and download project to the RA board.
3. Open a serial terminal application (e.g., Tera Term) to view output, configure the settings as follows:

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

### The images below showcase the output ###
**The EP information:** 

![ep_information.jpg](images/ep_information.jpg "The EP information")

**Scan for Wi-Fi Access Points:**
* The user presses '1' to scan for Wi-Fi Access Points.

![list_wifi_ap_scan.jpg](images/list_wifi_ap_scan.jpg "List of Wi-Fi Access Points")

* Enter the Wi-Fi index and password for Wi-Fi connection.

![wifi_scan_connection.jpg](images/wifi_scan_connection.jpg "Wi-Fi connection")

**Enter Wi-Fi AP credentials manually:**

* The user can press '2' to enter Wi-Fi AP credentials manually. Then the user enters SSID and password for Wi-Fi connection. 

![wifi_manual.jpg](images/wifi_manual.jpg "Connect to Wi-Fi manually")

**HTTP method menu:** 
* After successfully connecting to Wi-Fi, print the HTTP method menu for the user to choose.

![http_menu.jpg](images/http_menu.jpg "HTTP menu")

* The user presses '1' to make an HTTP GET request. 

![http_get_request.jpg](images/http_get_request.jpg "HTTP GET request")

* The user presses '2' to make an HTTP PUT request.

![http_put_request.jpg](images/http_put_request.jpg "HTTP PUT request")

* The user presses '3' to make an HTTP POST request.

![http_post_request.jpg](images/http_post_request.jpg "HTTP POST request")
