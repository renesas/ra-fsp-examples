# Introduction #

The sample code accompanying this file shows the operation of a NetX Duo SMTP Client on a RA MCU using Azure RTOS. 
In this sample code, a NetX Duo IP instance is created with IP address as (default 0.0.0.0 - Not assigned). 
The NetX stack is enabled for TCP, ICMP, ARP, IPV4. SMTP Client utilizes the reliable Transmission Control Protocol 
(TCP) services to perform its content transfer function.

SMTP Client creates its packet pool based on the settings minimum packet payload size and number of packets in the packet pool. 
When SMTP Client is created and started, NetX Duo SMTP Client creates a new TCP connection with the SMTP server and begins an SMTP session. In this session, 
the Client sends a series of commands to the SMTP Server as part of the SMTP protocol, culminating in sending out the actual mail message. 
The TCP connection is then terminated, regardless of the outcome of the SMTP session. 

After mail transmission, regardless of success or failure, the SMTP client is returned to the initial state and can be used for another mail transfer session.
The status messages, Client IP addresses, and the code flow informational messages as part of the initialization are displayed on the JlinkRTTViewer.

NetX Duo SMTP Client is compliant with [RFC2821](https://tools.ietf.org/html/rfc2821) , [RFC2554](https://tools.ietf.org/html/rfc2554) , and related RFCs.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) 
for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the SMTP client example project, the following resources are needed.

### Hardware ###
* Renesas RA™ MCU kit with Ethernet Support
* Micro USB cable
* Host Machine 
* Ethernet cable
* Ethernet switch

Refer to [readme.txt](./readme.txt) for information on how to connect the hardware.

### Software ###
* Local SMTP mail server(e.g Argo soft mail server)
      Note: One of the mail servers used here for testing is Argo soft mail server.
* Refer to software requirements mentioned in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of 
the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![smtp_client](images/smtp_client.jpg "SMTP Client Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| NetX Duo SMTP Client | SMTP is a protocol for transferring mail across networks and the Internet. It utilizes the reliable Transmission Control Protocol (TCP) services to perform its content transfer function.| smtp |

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml -> g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Settings > Property > Module g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Server IPv4 Address |  192.168.0.2   |  192.168.1.50  |  Set up the SMTP Server IP address. User can configure this as per their network environment |
|   configuration.xml -> g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Settings > Property > Module g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Client Name  |  username  |   epdesign   |  The host username during SMTP authentication with the Server  |
|   configuration.xml -> g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Settings > Property > Module g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Client Password  |  password    |   tel@1234   | The user password during SMTP authentication with the Server. |
|   configuration.xml -> g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Settings > Property > Module g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Client Address  |  username@domain.com    |   epdesign@test.local.com   |  Specify the sender address were message is to be delivered. |
|   configuration.xml -> g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Settings > Property > Module g_smtp_client0 Azure RTOS NetX Duo SMTP Client > Client Domain  |  domain.com   |   test.local.com    |  Selects domain name.  |
|   configuration.xml -> g_smtp_client0 Azure RTOS NetX Duo SMTP Client > g_ip0 Azure RTOS NetX Duo IP Instance > Property > Module g_ip0 Azure RTOS NetX Duo IP Instance > IPv4 Address |  192.168.0.2   |    192.168.1.122   |  Set up the SMTP Client IP address. User can configure this as per their network environment   |
|   configuration.xml -> g_smtp_client0 Azure RTOS NetX Duo SMTP Client > g_ip0 Azure RTOS NetX Duo IP Instance > Property > Module g_ip0 Azure RTOS NetX Duo IP Instance > Subnet Mask  |  255.255.255.0  |   255.255.255.0    |  Selects the subnet mask.  |
|   configuration.xml -> RTT Thread > Settings > Property > Thread > priority  |   1   |   2   |   RTT thread priority is lowered to allow the DHCP Client and IP threads to process incoming packets at the fastest rate possible.   |   

## API Usage ##

The table below lists the SMTP Client API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
|nxd_smtp_client_create| This API creates an SMTP Client instance on the specified IP instance. |
|nx_smtp_mail_send| This API creates and sends an SMTP mail item |
|nx_ip_fragment_enable| This API enables IPv4 and IPv6 packet fragmenting and reassembling functionality |
|nx_ip_status_check| This API checks and optionally waits for the specified status of the primary network interface of a previously created IP instance. |
|nx_ip_address_get| This API retrieves IPv4 address. |
|nx_icmp_ping| This API sends a ping request to the specified IP address and waits for the specified amount of time for a ping response message. |
|nx_packet_release| This API releases a packet, including any additional packets chained to the specified packet. |

## Verifying operation ##
Import, Build and Debug the EP(see section Starting Development of **FSP User Manual**). After running the EP, open the RTT viewer to see the output.
Before running the example project, refer to the below steps for hardware connections :
* Connect RA MCU debug port to the host PC via a micro USB cable. 
* Connect an Ethernet cable to the RJ45 connector on RA MCU via ethernet switch which is connected to the Host machine where the SMTP server is residing.
* A SMTP Server needs to exist on the network to run SMTP Client Example Project on the RA MCU board.  

**Note:**  
* To test this EP, it is highly recommended to connect the SMTP client (Running on RA Board) and  SMTP server (Running on test PC) on to the same LAN using Ethernet.  
* The EP will utilize the IPv4 address. You can find the SMTP server's IP address by executing "ipconfig /all" on the test PC via the Windows command prompt and locating it from the active Ethernet adapter being used for connecting to the LAN.  
* To configure an IP address for the Board (SMTP Client), users must assign an unused IP address from the LAN. This can be achieved by executing the command "for /L %i in (0,1,255) do ping -n 1 -w 250 10.0.0.%i>>ipaddress.txt" in the Windows command prompt. Subsequently, users can verify the allocated IP address by inspecting the ipaddress.txt file generated by the script. If a successful ping response is received for an IP address listed in ipaddress.txt, it indicates that the IP address is already in use by another device on the LAN. Select the  IP address which fails to the ping request.  Note: 10.0.0.%i --> corresponds to 10.0.0 network, modify this script according to your LAN  (eg 192.168.1.%i --> for 192.168.1.x network) 

Below image showcases the hardware connection setup required for running the EP :

![smtp_client_setup](images/smtp_connection_diagram.jpg "SMTP Client hardware setup")

Below images showcases the output on JLinkRTT_Viewer :

![smtp_client_rtt_output](images/smtp_rtt_log.jpg "SMTP Client RTT Log")

## Special Topics ##
Running the SMTP Server
 * Users can use any local SMTP mail server, but one used for testing is ArgoSoft Mail Server.
 * Following are the steps to run ArgoSoft Mail Server.
   1. Download and Install the server from link [Argosoft Mail Server](https://www.argosoft.com/rootpages/download)
   2. Run the MailServerUi file and configure the server 
   3. On running the server the first time it asks to accept the 30 days free trial version.
   4. On accepting, it asks to add a domain name, where we need to add a domain name as test.local.com

	  ![domain_option](images/add_domain.jpg "User domain name") 
   5. Once the domain name is added, add a New user by configuring the Username and Password and Confirm Password
      User can add Username: epdesign, Password: tel@1234, and Confirm Password: tel@1234.
	  
	  **Note**: The username, password, and confirm password should be entered same as configured in configuration property Client Name and Client Password under g_smtp_client0 Netx Duo SMTP Client. 

	  ![User_option](images/user_name_password_new.jpg "User option for selection")	
   6. To Confirm all the above have been configured correctly User can go to User and expand domain -> default, expand default, and check if a domain name is test.local.com added or not.
   7. Then Go to epdesign and click edit, and check Username it should appear as epdesign as configured previously.

	  ![Verification_option](images/verification_data_new.jpg "User option for selection")  
    **Note: Please ensure that "test.local.com" domain is created inside of _Default domain group.**
   8. Once all the above configurations are done and verified User can start the server.
   
Step to Validate the email from the Client
 * In Argo Softmail server go to Users -> Mailbox and click on it, to view the sent mail by the client.
 
   ![Mail_verification](images/mailbox_selection.jpg "User sent mail verification")
 * Also to check the detailed communication log between server and client user can check under logs ->  View logs. Image below is the successful log of sending mail in Argosoft Mail.  
 ![server_log.jpg](images/server_log.jpg "Server log send successfully")

## Known Issues ##

ArgoSoft SMTP server is a 30 day free trial version, if previous version is installed and trial period expires, it is required to get the license to in order to make it work. The server rejects the request and SMTP client may fail to send the message if the trial period is expired(This can be checked from the logs).  Uninstalling and reinstalling on the same PC sometimes may not work as it remembers the previous installation of the ArgoSoft tool.  It is recommended to install it on different PC and test the functionality.  
User can check detal of expired date at Help -> About. Detail show at picture below  
 ![Argosoft Mail date](images/argosoft_date.jpg "Argosoft Mail date")  
Note: It is advisable to stop and create a new user account and start the server. It is good idea to clean the previous user database if you are adding a new user for the server. Also to check the logs for the proper users are being used in the communication. If the sticky info about the previous user exists, the SMTP communication fails.

## Troubleshooting ##

As part of the EP testing, users may notice the ICMP ping failure when the EP pings to the server or SMTP connection failure as part of the SMTP connectivity phase. To resolve this issue, users are advised to verify the firewall settings and the port settings on your PC (Recommend to use the Lab PC/Test PC).   

**Issue 1:** ICMP ping fail.   
- The user had set up to run EP correctly as the Verifying operation section (Given SMTP Client and Server address successfully) but the EP pinged ICMP failed. -> The user needs to check the Solve section for this issue.  
![ping_fail.jpg](images/ping_fail.jpg "ICMP ping fail")  

- **Solve:** Please ensure ICMPv4 Echo Request is enabled. This can be achieved by going to Windows Defender Firewall -> Advanced settings -> Inbound Rules -> Enable File and Printer Sharing (Echo Request - ICMPv4-In).  
![ping_enable.jpg](images/ping_enable.jpg "Enable Ping action")    

**Issue 2:** Send mail fail. 
- The user had set up to run EP correctly as the Verifying operation section (Successfully pinged SMTP server) but the EP send mail failed (Option 1) -> The user needs to check the Solve section for this issue.     
![send_mail_fail.jpg](images/send_mail_fail.jpg "Send mail fail")  

- **Solve:**

  +  SMTP port 25 is used by this example project. This port is closed by default by Windows Firewall. Please ensure this port is open in the Windows Defender Inbound Rule and Outbound Rule prior to running this example project.
  + Please ensure the test.local.com domain needs to be under  “_Default” Domain   in the following hierarchy   (“Domain Group” --> “_Default” --> “test.local.com”) as shown in the image below.   
  ![domain_note.jpg](images/domain_note.jpg "Domain note")  
  
  + The user “epdesign” should belong to “test.local.com”.  When changing the user from “epdesign” to something different, it is required to change the user in the FSP configurator for the SMTP client as well. If there is mismatch between the username and password in the FSP configurator and the server side configuration, the failure can be expected. To overcome those error, please make sure you follow the instructions listed in the MD file.
  + The EP is designed to send to "epdesign@test.local.com", this user should be avaliable on “Domain Group” --> “_Default” --> “test.local.com” of Argosoft Mail. Incase user want to change destination please change in smtp_client_thread_entry.c --> line 154, detail at the image below.   
  ![change_user.jpg](images/change_user.jpg "Change user in code") 