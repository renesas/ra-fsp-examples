# Introduction #
This project demonstrates the basic functionality of an SNMP agent running on Azure RTOS with Renesas RA MCUs using the Renesas Flexible Software Package (FSP). The SNMP agent is configured to respond to SNMP manager commands and to send event-driven traps. When any key is pressed in the J-Link RTT Viewer, the agent sends an SNMP trapv2 event to the manager. Application status is displayed in the J-Link RTT Viewer.

The NetX Duo SNMP agent is compliant with
[RFC1157](https://www.rfc-editor.org/rfc/rfc1157.html),
[RFC1155](https://www.rfc-editor.org/rfc/rfc1155.html),
[RFC1215](https://www.rfc-editor.org/rfc/rfc1215.html),
[RFC1901](https://www.rfc-editor.org/rfc/rfc1901.html),
[RFC1905](https://www.rfc-editor.org/rfc/rfc1905.html),
[RFC1906](https://www.rfc-editor.org/rfc/rfc1906.html),
[RFC1907](https://www.rfc-editor.org/rfc/rfc1907.html),
[RFC1908](https://www.rfc-editor.org/rfc/rfc1908.html),
[RFC2571](https://www.rfc-editor.org/rfc/rfc2571.html),
[RFC2572](https://www.rfc-editor.org/rfc/rfc2572.html),
[RFC2574](https://www.rfc-editor.org/rfc/rfc2574.html),
[RFC2575](https://www.rfc-editor.org/rfc/rfc2575.html),
[RFC3414](https://www.rfc-editor.org/rfc/rfc3414.html),
and related RFCs.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of the operation.

## Required Resources ##
To build and run the SNMP Agent example project, the following resources are needed.

### Hardware ###
* Supported RA boards: EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1, MCK-RA8T1, EK-RA8M2.
  * 1 x Renesas RA board.
  * 1 x Type-C USB cable for programming and debugging.
  * 2 x Ethernet/LAN cable (Ethernet cable CAT5/6).
  * 1 x Ethernet switch.
  * 1 x Linux machine, Raspberry Pi, or Windows PC running Ubuntu and WSL system to run the SNMP Manager.

### Hardware Connections ###
* For EK-RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
  * Connect USB debug port of the RA board to the host machine via a micro USB cable.
  * Connect ethernet port of the RA board to ethernet switch/router via a LAN cable.
  * Connect ethernet port of the Linux machine/Raspberry Pi/Windows PC to ethernet switch/router via a LAN cable.
  * For EK-RA8M1, the user must remove jumper J61 to enable ethernet B.

  * For EK-RA8D1, the user must set the configuration switches (SW1) as below to avoid potential failures:
    * CAUTION: Do not enable SW1-4 and SW1-5 together. 

      | SW1-1 PMOD1 | SW1-2 TRACE | SW1-3 CAMERA | SW1-4 ETHA | SW1-5 ETHB | SW1-6 GLCD | SW1-7 SDRAM | SW1-8 I3C |
      |-------------|-------------|--------------|------------|------------|------------|-------------|-----------|
      | OFF | OFF | OFF | OFF | ON | OFF | OFF | OFF |

* For MCK-RA8T1, EK-RA8M2:
  * For EK-RA8M2, the user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3, and J29 on pins 1-2, 3-4, 5-6, 7-8 to use the on-board debug functionality.
  * Connect USB debug port of the RA board to the host machine via a Type-C USB cable.
  * Connect ethernet port of the RA board to Ethernet switch/router via a LAN cable.
  * Connect ethernet port of the Linux machine/Raspberry Pi/Windows PC to Ethernet switch/router via a LAN cable.

### Software ###
Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)
- [Azure SNMP Documentation](https://docs.microsoft.com/en-us/azure/rtos/netx-duo/netx-duo-snmp/chapter1)

# Project Notes #
## System Level Block Diagram ##
![snmp_agent](images/snmp_agent_hardware_connection.jpg "SNMP Agent Block Diagram")

## FSP Modules Used ##
The following FSP modules are used in this example project. Refer to the FSP User Manual for detailed information about each module.

| Module Name | Usage | Searchable Keyword (using **New Stack > Search**) |
|--------------|--------|--------------------------------------------------|
| **NetX Duo SNMP Agent** | Provides access to the NetX SNMP agent library, which responds to SNMP Manager commands and sends event-driven traps. | snmp |
| **NetX Duo IP Instance** | Enables IP communication. The SNMP agent uses IPv4 for network management functions. | NetX Duo IP |
| **NetX Duo Packet Pool** | Manages packet memory used for sending and receiving data packets over the UDP network. | NetX Duo Packet Pool |
| **NetX Duo Ethernet Driver** | Provides the Ethernet interface required for physical network connectivity. | NetX Duo Ethernet Driver |
| **NetX Duo DHCPv4 Client** | Obtains the IP address dynamically and automates IP configuration for UDP/TCP-based communication. | dhcp |

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Heap size (bytes) | 0 | 0x400 | Heap size is required for using standard library functions. |
| configuration.xml > Stacks > Threads > SNMP Thread > Properties > Settings > Property > Thread > Priority | 1 | 14 | SNMP Thread priority is lowered to allow the IP thread to process incoming packets at the fastest rate possible. |
| configuration.xml > Stacks > Threads > RTT_Thread > Properties > Settings > Property > Thread > Priority | 1 | 15 | RTT thread priority is lowered to allow the SNTP Client and IP threads to process incoming packets at the fastest rate possible. |

## API Usage ##

The table below lists the **SNMP agent** and **DHCP client** APIs used at the application layer in this example project.

| API Name | Usage |
|-----------|--------|
| nx_snmp_agent_create | This API is used to create an SNMP agent instance. |
| nx_snmp_agent_private_string_set | This API is used to set the SNMP agent’s private community string. |
| nx_snmp_agent_public_string_set | This API is used to set the SNMP agent’s public community string. |
| nx_snmp_agent_context_engine_set | This API is used to set the context engine ID for SNMPv3 processing. |
| nx_snmp_agent_md5_key_create | This API is used to create an MD5 key for authentication and encryption. |
| nx_snmp_agent_authenticate_key_use | This API is used to specify the key for SNMPv3 authentication. |
| nx_snmp_agent_privacy_key_use | This API is used to specify the key for encrypting and decrypting SNMPv3 messages. |
| nx_snmp_agent_auth_trap_key_use | This API is used to specify the authentication key for trap messages. |
| nx_snmp_agent_priv_trap_key_use | This API is used to specify the encryption key for trap messages. |
| nx_snmp_agent_start | This API is used to bind the UDP socket to SNMP port 161 and start the SNMP agent. |
| nx_snmp_agent_trapv2_send | This API is used to send an SNMP trapv2 message to the SNMP Manager. |
| nx_snmp_object_compare | This API is used to compare a supplied object identifier (OID) with a reference OID. |
| nx_snmp_object_integer_get | This API is used to retrieve the integer value of the specified SNMP object. |
| nx_snmp_object_integer_set | This API is used to set the integer value for the specified SNMP object. |
| nx_system_initialize | This API is used to initialize the NetX Duo system components and data structures. |
| nx_packet_pool_create | This API is used to create a packet pool in the specified memory region. |
| nx_ip_create | This API is used to create an IP instance with the given IP address and network driver. |
| nx_ip_address_get | This API is used to retrieve the IP address and subnet mask of the primary interface. |
| nx_udp_enable | This API is used to enable UDP protocol support for the IP instance. |
| nx_icmp_enable | This API is used to enable ICMP protocol support for the IP instance. |
| nx_ip_status_check | This API is used to check (and optionally wait for) a specific status of the IP instance. |
| nx_dhcp_create | This API is used to create a DHCP client instance for the IP instance. |
| nx_dhcp_packet_pool_set | This API is used to assign a packet pool for DHCP client operations. |
| nx_dhcp_start | This API is used to start DHCP processing on all enabled network interfaces. |

## Verifying Operation ##
* Before running the example project, make sure to set up hardware connections as mentioned in the **[Hardware Connections](#hardware-connections)** section and the image below:

  ![snmp_agaent_hardware_setup](images/snmp_agent_hardware_connection.jpg "SNMP Agent Hardware Setup")

* Open a terminal on a Linux machine or a Raspberry Pi to check status of the SNMP Manager services. If the SNMP Manager services had not installed yet, please refer to the instructions in **[Special Topics](#special-topics)**.

  ![snmp_manager_services_status](images/snmp_manager_services_status.jpg "SNMP Manager Services Status")

* Import the example project into e2studio.

* The user needs to check and update the SNMP Manager's IP address in the **...\e2studio\src\snmp_agent_ep.h** file from the default value to the IPv4 address of the user's SNMP Manager on a Linux machine or a Raspberry Pi. **Note:** When using a Windows PC running Ubuntu with WSL, the IP address mentioned in this step should be the Windows PC’s IP address, because the SNMP Manager cannot connect directly to snmptrapd inside WSL due to its virtual network isolation.

  ![snmp_manager_ip_update](images/snmp_ip_change.jpg "IP change")

* Build and debug the example project (refer to the *Starting Development* section in the **FSP User Manual**). After running the project, open the **J-Link RTT Viewer** on the host PC to see the output.
 
* The user needs to check and update the IP address of the RA board in [snmp_manager_agent_test.sh](images/snmp_manager_agent_test.sh). The RA board IP address is displayed on the **J-Link RTT Viewer**. Open the [snmp_manager_agent_test.sh](images/snmp_manager_agent_test.sh) shell script and update the variable **AGENT_ADDR** with the IP address of the RA board:

  ![snmp_agent_IP_address_update](images/snmp_agent_IP_address_update.jpg "Command for updating Agent IP address")

* SNMP agent started successfully:

  ![snmp_agent_rtt_output](images/snmp_banner_rtt_log.jpg "SNMP Agent Banner")

* Verify SNMP trap reception:

  * Run the shell script [run_snmp_manager.sh](images/run_snmp_manager.sh) to verify the trap event sent by the SNMP agent. Enter any key in RTT Viewer to send trap to SNMP Manager. Refer to the image below. **Note:** Please ensure that port 162 is free and not in use by other services.

  ![snmp_manager_trap](images/snmp_manager_trap.jpg "SNMP manager trap")

* Note: Press Ctrl + C to terminate run_snmp_manager.sh (send SIGINT) before executing other commands.

* Verify **SET**, **GET**, and **GETNEXT** operations:

  * Run the shell script [snmp_manager_agent_test.sh](images/snmp_manager_agent_test.sh) to perform **SET**, **GET**, and **GETNEXT** operations with different versions of the SNMP agent and see the output on RTT Viewer:

  ![snmp_manager_script](images/snmp_manager_script.jpg "SNMP Manager services script")

  ![snmp_agent_rtt_output](images/snmp_when_agent_manager_1.jpg "SNMP Agent RTT Log")

  ![snmp_agent_rtt_output](images/snmp_when_agent_manager_2.jpg "SNMP Agent RTT Log")

## Special Topics ##
  * To install SNMP Manager services, the user should use **a Linux machine or Raspberry Pi with full access permissions**. Follow the steps below to run the necessary shell scripts in the terminal.

  1. **Install SNMP Manager services**  
    Open the Linux terminal and run the shell script [install_snmp_manager.sh](images/install_snmp_manager.sh), which installs the SNMP Manager services. Refer to the image below:
    ![snmp_install_services](images/snmp_install_services.jpg "Install SNMP Manager services")

      **Note:**  
      If the shell script [install_snmp_manager.sh](images/install_snmp_manager.sh) fails to execute, open the Linux or Raspberry Pi terminal and run the commands below to manually install the SNMP Manager packages:  
      ```
      sudo apt install snmptrapd
      sudo apt install snmpd
      sudo apt install snmp
      ```  

  2. **Edit the SNMP trap configuration file**  
  * Open the file using the command:  
      ```
      vim /etc/snmp/snmptrapd.conf
      ```

  * Add SNMP Manager configuration:  
      * Copy the commands below and paste them at the end of the `snmptrapd.conf` file, then save and close the file:  
      ```
      authCommunity log,execute,net public  
      createUser -e 0x8000c73803003055234455 public SHA "authpassword" DES "privpassword"  
      authUser log public
      ```  
  
      Refer to the image below:  
      ![snmp_manager_snmpconfig_command](images/snmp_manager_snmpconfig_command.jpg "Command for snmptrapd.conf file")
