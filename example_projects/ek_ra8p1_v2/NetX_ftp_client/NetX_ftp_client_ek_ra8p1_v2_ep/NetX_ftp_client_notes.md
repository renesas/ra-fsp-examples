# Introduction #
This project demonstrates the basic functionality of NetX Duo FTP Client with an ethernet driver on Renesas RA MCUs based on Renesas FSP using AzureRTOS. The example project configures the FTP Client to establish the connection with FTP Server. The FTP Client will perform read and write operations, on to the currently opened file on the FTP Server. The status message and other relevant data will be displayed on J-Link RTT Viewer.

NetX Duo FTP is compliant with [RFC 959](https://datatracker.ietf.org/doc/html/rfc959), [RFC 2428](https://datatracker.ietf.org/doc/html/rfc2428) and related RFCs.

***Note:** In the example project, NetX Duo IP instance is having a static IP address from our test setup environment.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the NetX_ftp_client example project, the following resources are needed.

### Hardware ###
* Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8P1, EK-RA8D2, EK-RA8M2, MCK-RA8T2, EK-RA8P1 V2.
  * 1 x Renesas RA board.
  * 1 x Host PC.
  * 1 x USB cable for programming and debugging (USB cable type varies by board model).
  * 2 x Ethernet/LAN cables (Ethernet cable CAT5/6).
  * 1 x Ethernet switch.

### Hardware Connections ###
* Connect the RA board USB debug port to the host PC using the appropriate USB cable for EP programming and debugging.
* Connect the Ethernet switch to the Ethernet port of the device running the FTP server using a LAN cable.
* Connect the RA board Ethernet port to the Ethernet switch using another LAN cable.
* Note: MCK-RA8T2 uses Ethernet port 0 (CN14) by default.

* For EK-RA8D1:
  * Set the configuration switches (SW1) as below.
      | SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
      |-------------|-------------|--------------|------------|------------|------------|-------------|-----------|
      | OFF | OFF | OFF | OFF | ON | OFF | OFF | OFF |

* For EK-RA8M1:
  * The user must remove jumper J61 to enable Ethernet B.

* For EK-RA8M2:
  * The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.

* For EK-RA8P1 V2:
  * The user must set the configuration switches (SW6) as below to use the on-board debug functionality.

    | SW6-1 | SW6-2 | SW6-3 | SW6-4 | SW6-5 | SW6-6 | SW6-7 | SW6-8 | SW6-9 | SW6-10 |
    |-------|-------|-------|-------|-------|-------|-------|-------|-------|--------|
    | OFF | OFF | OFF | OFF | ON | OFF | ON | ON | ON | ON |

* The image below showcases the hardware connection setup required for running the EP:

![ftp_client](images/ftp_client_high_level_block_connection_detail.jpg "Hardware connection")

### Software ###
1. Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf).
2. Refer to **Special Topics** for configuring the [Filezilla](https://filezilla-project.org/download.php?type=server) as FTP Server.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
High level block diagram:

![ftp_client](images/ftp_client_high_level_block_diagram.jpg "ftp_client Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| Netx Duo FTP Client | FTP Client module is used to connect with FTP Server to perform write/read operations in the created file on FTP Server. | NetX Duo FTP Client |
| Netx Duo IP Instance | Board IP address should be provided as FTP Client. | Netx Duo IP |
| Netx Duo Packet Pool Instance | Data packets to be created to send and receive over TCP network. | NetX Duo Packet Pool |
| Netx Duo Ethernet Driver | Ethernet connection should require as physical connection to connect over network. | rm_netxduo_ether |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different from those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x1000 | Main program thread stack is configured to store the local variables of different functions in the code. |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x1000 | Heap size is required for standard library functions to be used. |
| configuration.xml > Stacks > Threads > FTP Client Thread > Properties > Settings > Property > Thread > Priority | 1 | 3 | FTP Client thread priority is lowered to allow the IP thread to process incoming packets at the fastest rate possible. |
| configuration.xml > Stacks > Threads > FTP Client Thread > FTP Client Thread Stacks > g_ip0 Azure RTOS NetX Duo IP Instance > Properties > Settings > Property > Module g_ip0 Azure RTOS NetX Duo IP Instance > IPv4 Address | 192.168.0.2 | 132.158.124.122 | Current IP settings are provided as per our test environment. The user has to modify this IP address according to their network settings. |
| configuration.xml > Stacks > Threads > FTP Client Thread > FTP Client Thread Stacks > g_ip0 Azure RTOS NetX Duo IP Instance > Properties > Settings > Property > Module g_ip0 Azure RTOS NetX Duo IP Instance > Subnet Mask | 255.255.255.0 | 255.255.255.128 | Current subnet mask is provided as per our test environment. The user has to modify this subnet mask according to their network settings. |
| configuration.xml > Stacks > Threads > FTP Client Thread > FTP Client Thread Stacks > g_ip0 Azure RTOS NetX Duo IP Instance > Properties > Settings > Property > Module g_ip0 Azure RTOS NetX Duo IP Instance > Default Gateway Address | 0.0.0.0 | 132.158.124.1 | Current Default Gateway address is provided as per our test environment. The user has to modify this Gateway Address according to their network settings. |
| configuration.xml > Stacks > Threads > RTT Thread > Properties > Settings > Property > Thread > Priority | 1 | 5 | RTT thread priority is lowered to allow the FTP Client and IP threads to process incoming packets at the fastest rate possible. |

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| nx_ftp_client_create | This API is used to create an FTP instance. |
| nxd_ftp_client_connect | This API is used to connect the FTP Server at the supplied IP address. |
| nx_ftp_client_file_open | This API is used to open the specified file for writing/reading on FTP Server. |
| nx_packet_allocate | This API is used to allocate the FTP packet. |
| nx_packet_data_append | This API is used to append the packet data on the opened file. |
| nx_ftp_client_file_read | This API is used to read the packets from the opened file. |
| nx_ftp_client_file_write | This API is used to write a packet of data to the opened file on the FTP Server. |
| nx_ftp_client_file_close | This API is used to close the opened file on the FTP Server. |
| nx_ftp_client_disconnect | This API is used to disconnect from the FTP Server. |
| nx_ftp_client_delete | This API is used to delete the FTP Client instance. |

## Verifying Operation ##

Import, build and debug the EP (See Starting Development section of FSP User Manual). After running the EP, open RTT Viewer to see the output.

Before performing the test, confirm that the host PC has an inbound Firewall rule allowing connections on **TCP port 21**, and this port is not already in use by another application or service. This example project uses TCP port 21 for FTP communication. If no such rule exists, refer to [2. Configure Windows Defender Firewall to allow TCP port 21](#2-configure-windows-defender-firewall-to-allow-tcp-port-21).

The images below showcase the output on RTT Viewer:

![ftp_client](images/rtt_output_banner_page.jpg "RTT output")

![ftp_client](images/rtt_output_remaining_status.jpg "RTT output")

## Special Topics ##
### 1. Setting up the Filezilla Server: ###
[Filezilla](https://filezilla-project.org/download.php?type=server) can be use as FTP Server, and it should be running on the same local network.

Server should be configured with the username and password for authentication purpose of FTP Client. Please follow steps below for configuring the FTP Server.  
Following steps guide you on how to configure the Filezilla Server.

1. Download the [Filezilla](https://filezilla-project.org/download.php?type=server) Server and install in the host machine. By default, FileZilla Server runs using the SYSTEM Windows user account with full control permissions, or the user can provide account username and account password to run with different Windows user account. Click **Next**

![ftp_client](images/file_zilla_service_setting.jpg "filezilla service setting")

2. Then, the user needs to choose the listening port and administration password. By default, listening port is **14148**.

![ftp_client](images/file_zilla_admin_setting.jpg "filezilla administration setting")

3. Based on the installation settings, the user needs to fill in the information as shown in the image below. Then click on **OK** to connect.

![ftp_client](images/file_zilla_welcome_page.jpg "filezilla getting started")

4. Upon clicking on **Connect** button, Filezilla Server is connected, and it is waiting for authentication viz., **username** and **password**.

![ftp_client](images/file_zilla_waiting_for_authentication.jpg "filezilla for authentication")
 
5. Now click on **Server** > **Configure...** to configure the user account as shown in the image below. Click Users.

![ftp_client](images/file_zilla_user_creation.jpg "file zilla in user creation")

6. Now click on **Add** button as shown in the image below, please provide username into the field.

![ftp_client](images/file_zilla_add_user.jpg "Add user") 
 
7. Now provide password into the blank field as shown in the image below.

![ftp_client](images/file_zilla_add_user_account.jpg "Add user account")
 
8. Now **Users** has been registered with the registered **username** and **password**.
 
![ftp_client](images/file_zilla_add_user_account_credential_ok.jpg "Add password")
 
9. Next, choose the shared folder at **Mount points** option as shown in the image below.
 
![ftp_client](images/file_zilla_add_directory.jpg "Prompt add directory")
 
10. Now click on **Add** and provide the **Virtual path** and **Native path**. The native path is a local file path, the virtual path is the path that the FTP users will see, and it is mapped to the native path by FileZilla Server.
 
![ftp_client](images/file_zilla_shared_folders.jpg "Add shared folder")

11. After provide the folder path, adjust the **Mount options** as shown in the image below and click **OK**

![ftp_client](images/file_zilla_shared_folders_settings.jpg "Add shared folder")
 
12. Upon successfully created account settings, FileZilla Server displays the message as shown in the image below, and now it's ready to connect the FTP Client.
 
![ftp_client](images/file_zilla_account_settings.jpg "Account added")

13. Now the user configures credentials server IP address. Get credentials SERVER IP address by: open **command prompt**, type **ipconfig** (In case using window) and get IP address, Subnet Mask, Default Gateway.

![ftp_client](images/get_ip_information.jpg "Get IP information")

14. Now the user should provide credentials server IP address into FileZilla Server by click on **Server** > **Configures...** > **Server Listener** as shown in the image below.

![ftp_client](images/setting_server_listener.jpg "Setting server listener")
  
15. Now the user must update the **USERNAME**, **PASSWORD**, **SERVER_ADDRESS** macros in **user_app.h** file which match with their server setting values in **Step 6**, **Step 7**, and **Step 14** as shown in the image below.
 
![ftp_client](images/user_app.jpg "User app")

16. Next, update the **Subnet Mask**, **Default Gateway Address** as the information from step **12**. **IPv4 Address** is client IP address, it should same **Network ID** with server IP.

![ftp_client](images/ftp_client_configuration.jpg "Configuration")
  
17. After updating in the code, build and run the NetX_ftp_client EP. FileZilla Server status messages will display as shown in the image below upon successful communication.
 
![ftp_client](images/file_zilla_communication_result.jpg "Communication result")

### 2. Configure Windows Defender Firewall to allow TCP port 21: ###

This example project uses TCP port 21 for FTP communication. The host PC must allow incoming connections on this port to enable successful communication between the RA MCU and the FTP Server.

Follow these steps to configure Windows Defender Firewall:

1. Open **Windows Defender Firewall** and Click **Advanced settings**.

![Advanced settings in Windows Defender Firewall](images/advanced_setting_windows_firewall.jpg "Advanced Settings")

2. Select **Inbound Rules**.
3. Click **New Rule…**.
4. Choose **Port**, then click **Next**.

![New inbound rule - rule type selection](images/new_inbound_rule_1.jpg "New Inbound Rule - Rule Type")

5. Select **TCP** and enter **21** in **Specific local ports**, then click **Next**. 

![New inbound rule - protocol and port selection](images/new_inbound_rule_2.jpg "New Inbound Rule - Protocol and Port")

6. Select **Allow the connection**, then click **Next**.

![New inbound rule - allow the connection](images/new_inbound_rule_3.jpg "New Inbound Rule - Allow Connection")

7. Keep the default profiles (**Domain / Private / Public**) as required, then click **Next**.

![New inbound rule - profile selection](images/new_inbound_rule_4.jpg "New Inbound Rule - Profile Selection")

8. Enter a rule name (for example, **FTP Port 21**), then click **Finish**.

![New inbound rule - rule name and finish](images/new_inbound_rule_5.jpg "New Inbound Rule - Name and Finish")

After completing these steps, the host PC will accept incoming FTP connections on TCP port 21.
