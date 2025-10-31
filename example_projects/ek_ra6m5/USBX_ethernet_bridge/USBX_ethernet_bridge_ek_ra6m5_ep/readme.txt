/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about the Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates the usage of the Azure NetX Duo network stack over the USBX RNDIS module on an RA MCU.
	It performs various network operations leveraging NetX Duo. The RA board creates a DHCP server and assigns the
	dynamic IP address for the host PC. The RA board and the host PC are directly linked via a point-to-point Ethernet
	connection over a USB cable. After obtaining the IP address, the project enables the user to interact with a menu
	through the J-Link RTT Viewer or a terminal application on the host PC.

	The available options include:
	- TCP Client Service: Connect to a server, send a request, wait for a response from the server, and disconnect 
	  from the server.
	- UDP Client Service: Send a request and wait for a response from the server.
	- Web HTTP Server: Handle HTTP requests from the browser, including requesting the homepage, retrieving board 
	  network configuration, controlling onboard LEDs, and requesting the status of onboard LEDs.
	  
	The menu options:
	- Type `1 <x.x.x.x>:<port>` to perform TCP client operation.
	- Type `2 <x.x.x.x>:<port>` to perform UDP client operation.
	- Type `3` to start the web HTTP server.

	Notes: 
	- For RA boards that do not support J-Link OB VCOM,  EP information is communicated to the user via host PC 
	  exclusively through the SEGGER J-Link RTT Viewer.
	    + The board does not support J-Link OB VCOM: EK-RA6M5.
	- For the RA boards support J-Link OB VCOM, the EP uses the Serial Terminal by default instead.

2. Software Requirements:
	- Renesas Flexible Software Package (FSP): Version 6.2.0
	- e2 studio: Version 2025-10
	- GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	- Terminal Console Application: Tera Term or a similar application
	- SEGGER J-Link RTT Viewer: Version 8.74
	- Socket Application: sokit version 1.3, etc
	- Browser Application: Microsoft Edge, Google Chrome, etc
	- RNDIS (Remote Network Driver Interface Specification) driver installed to recognize the MCU as a USB Ethernet interface
	Note: The file RNDIS_INF_NDIS_6_0_for_RA.inf is based on the windows RNDIS template. In this file, the VID and PID have been
	      modified to match the USB descriptor used in this project.
	      windows RNDIS template: https://learn.microsoft.com/en-us/windows-hardware/drivers/network/remote-ndis-inf-template
3. Hardware Requirements:
	- Supported RA boards: EK-RA6M5
	- 2 x Micro USB cables
	    + One cable is used for firmware programming and debugging via the onboard debugger.
	    + The other cable connects the RA board to a host PC and acts as a USB network interface, allowing network
	      communication over USB.

4. Hardware Connections:
	- General: Connect the RA Board to the Host PC: Using a micro-USB cable, connect the RA board's [DEBUG] port to the host PC.
	  This connection is necessary for programming the RA board, enabling debugging, and displaying runtime information in the
	  terminal or RTT Viewer on the host PC.
	- For the specific RA boards, to establish network connectivity, connect any 1 port (High speed or Full Speed) of the
	  RA board's USB Port to the Host PC following the below introductions:
	    + EK-RA6M5:
		* High Speed (Default):
		    Connect the RA board's USB HS port (J31 connector) to the Host PC using a micro USB cable.
		    Connect the jumper J17 pins.
		    Short Pin 2-3 of jumper J7.
		* Full Speed:
		    Connect the RA board's USB FS port (J11 connector) to the Host PC using a micro USB cable.
		    Connect the jumper J15 pins.
		    Short Pin 2-3 of jumper J12.

5. Verifying Operation:
	1. Make sure the RNDIS driver is installed successfully. If not, please go to RNDIS Driver installation section in USBX_ethernet_bridge_notes.md
	2. Import, generate, build, and flash the example project to the RA board using e2 studio on the host PC.
	3. Open the terminal application on the host PC and connect to the RA board.
	4. Wait for the HostPC to obtain the IP address from the RA Board.
	5. The RA board's network configuration details and menu options will be displayed in the terminal application.
	6. Verify TCP Client Operation:
		1. Launch a socket application on the host PC.
		2. Set up a TCP server by specifying the server's IP address and port number.
		3. Ensure that the server is configured to listen for incoming connections.
		4. In the terminal, type the command: '1 <x.x.x.x>:<port>'.  
		   For example, if the server's IP is '192.168.2.10' and the port is '12345', type: '1 192.168.2.10:12345'.
		5. The RA board will send a TCP request to the server.
		6. The socket application will display the TCP request.
		7. In the socket application, type any data and click 'Send' to respond to the RA board.
		8. The RA board will display the received data in the terminal application.
	7. Verify UDP Client Operation:
		1. Launch a socket application on the host PC.
		2. Set up a UDP server by specifying the server's IP address and port number.
		3. Ensure that the server is configured to listen for incoming connections.
		4. In the terminal, type the command: '2 <x.x.x.x>:<port>'.  
		   For example, if the server's IP is '192.168.2.10' and the port is '12345', type: '2 192.168.2.10:12345'.
		5. The RA board will send a UDP request to the server.
		6. The socket application will display the UDP request.
		7. In the socket application, type any data and click 'Send' to respond to the RA board.
		8. The RA board will display the received data in the terminal application.
	8. Verify Web Server Operation:
		1. In the terminal, type '3' to start the web server.
		2. Open a web browser on the host PC.
		3. In the URL bar, type: '<x.x.x.x>/index.html' and press 'Enter', where '<x.x.x.x>' is the IP address of the RA board.
		4. The browser will display the network configuration and LED control panel.
		5. In the LED control panel on the webpage, click on the SW1 or SW2 buttons to toggle LED1 or LED2 on the RA board.
		6. Click on SW1 or SW2 on the RA board to toggle LED1 or LED2.
		7. The RA board will update the LED states dynamically on the webpage.
		8. In the terminal, type any character to stop the web server.

Note:
	For the SEGGER RTT Viewer application: The macro USE_VIRTUAL_COM is set to 0.
		1. The Segger RTT block address may need to be updated to observe the EP operation using a hex file with SEGGER RTT Viewer.
			RTT Block addresses for hex file committed in the repository are as follows:
   a. e2studio: 0x20004fe0
			b. Keil: Not Available
			c. IAR: Not Available
		2. If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
		   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).