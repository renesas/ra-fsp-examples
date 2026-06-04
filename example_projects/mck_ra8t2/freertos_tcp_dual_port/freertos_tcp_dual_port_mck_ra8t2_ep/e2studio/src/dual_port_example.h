/*
 * multiport_exaple.h
 *
 *  Created on: Jan 26, 2026
 *      Author: buiba
 */

#ifndef DUAL_PORT_EXAMPLE_H_
#define DUAL_PORT_EXAMPLE_H_

#define SERVER_PORT_ETH0 5000
#define SERVER_PORT_ETH1 5001

#define EP_INFO \
"This example demonstrates dual Ethernet interfaces on the RA MCU using the FreeRTOS+TCP stack.\r\n" \
"Each interface (ETH0 and ETH1) obtains its network configuration via DHCP.\r\n" \
"After the network is established, the system resolves google.com using DNS\r\n" \
"and sends a ping to verify connectivity.\r\n" \
"The application then runs two TCP echo servers on different ports.\r\n" \
"On the PC side, a Python script acts as a TCP client, allowing the user to select the IP\r\n" \
"address and port to send data and receive the echoed response.\r\n"

#define IP_BYTES(x)  (x)&0xFF, ((x)>>8)&0xFF, ((x)>>16)&0xFF, ((x)>>24)&0xFF

void vInitializeNetworkInterfaces(void);
void vInitializeNetworkInterfaces_1(void);
void vPrintAllNetworkInfo(void);

#endif /* DUAL_PORT_EXAMPLE_H_ */
