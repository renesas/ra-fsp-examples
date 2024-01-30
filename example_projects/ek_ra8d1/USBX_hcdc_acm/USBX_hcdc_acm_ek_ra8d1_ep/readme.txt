/***********************************************************************************************************************
* Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
*
* This software is supplied by Renesas Electronics America Inc. and may only be used with products of Renesas Electronics Corp.
* and its affiliates (“Renesas”).  No other uses are authorized.  This software is protected under all applicable laws, 
* including copyright laws.
* Renesas reserves the right to change or discontinue this software.
* THE SOFTWARE IS DELIVERED TO YOU “AS IS,” AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT 
* PERMISSIBLE UNDER APPLICABLE LAW,DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE.  TO THE MAXIMUM 
* EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE (OR ANY PERSON 
* OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING, WITHOUT LIMITATION, 
* ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES;
* ANY LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF 
* THE POSSIBILITY OF SUCH LOSS,DAMAGES, CLAIMS OR COSTS.
* **********************************************************************************************************************/

1. Project Overview:
     This Example Project demonstrates the basic functionality of USBX Host CDC ACM module on Renesas RA MCUs
     using 2 RA boards. Board 1 acts as Host device running USBX HCDC-ACM example project and Board 2 acts as Peripheral device
     running USBX PCDC-ACM example project and connected with micro USB cable via OTG. The peripheral initiates the communication 
     and once the host reads the data initiated by the peripheral. The host will then send predefined data to the peripheral and the
     device will echo the data back to the host. The application status messages will be displayed on the Jlink RTT Viewer. 

2. Supported Boards:
    EK-RA6M3, EK-RA6M4, EK-RA4M2, EK-RA4M3, EK-RA8M1, EK-RA8D1

3. Hardware Requirements:
    External Hardware : OTG cable: Micro to USB Host (Female)   - 1 nos
                        USB A to USB Micro B Cable              - 3 nos

	EK-RA6M3 - Running USBX HCDC Example Project 
	Any RA board running USBX PCDC Example Project.

4. Configuration changes to switch Speed in configurator: 

     i. Full-Speed: (EK-RA6M3, EK-RA6M4, EK-RA4M2, EK-RA4M3, EK-RA8M1, EK-RA8D1)
          USB Speed                      :    Full Speed
          USB Module Number              :    USB_IP0 Port
          DMA Source Address             :    FS Address
          DMA Destination Address        :    FS Address
          DMAC Transfer Size             :    2 Bytes

    ii. High-Speed: (EK-RA6M3)
          USB Speed                      :    Hi Speed
          USB Module Number              :    USB_IP1 Port
          DMA Source Address             :    HS Address
          DMA Destination Address        :    HS Address
          DMAC Transfer Size             :    4 Bytes

5. Hardware Connections:
	Connect Board 1, running USBX HCDC ACM Example Project, through OTG cable to Board 2, running USBX PCDC ACM Example Project.

   Jumper Settings: 

     i. Full-Speed: (EK-RA6M3, EK-RA6M4, EK-RA4M2, EK-RA4M3, EK-RA8M1, EK-RA8D1)
          Jumper J12 placement is pins 1-2
          Remove Jumper J15 pins
          Connect USB device to J11 connector with help of micro USB Host cable.

    ii. High-Speed:(EK-RA6M3)
          Jumper J7: Connect pins 1-2
          Remove Jumper J17 pins
          Connect USB device to J6 connector with help of micro USB Host cable.

6. Steps for Running the EP
   1. Import, generate and build the USBX PCDC and USBX HCDC EP.
   2. Now, flash USBX PCDC code on RA board(i.e board 2) which will acts as peripheral device.
   3. Open RTT viewer for Board 2.
   4. Next, flash USBX HCDC code on another RA Board (i.e board 1) which will acts as host device.
   5. Open RTT viewer for Board 1 and verify host device operation.

   Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086

Note: 
1) Both USBX HCDC and PCDC board needs to be running at High Speed or Full Speed in order for the correct EP functionality.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000a20
   b. Keil: 	Not Available 
   c. IAR:  	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
