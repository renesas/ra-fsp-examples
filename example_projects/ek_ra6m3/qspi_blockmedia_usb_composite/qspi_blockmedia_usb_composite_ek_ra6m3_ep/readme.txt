/***********************************************************************************************************************
* 
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
* 
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the QSPI as block media on Renesas RA MCU's using the USB composite Class driver(PMSC + PCDC).
	In this example, the application will configure the MCU as a CDC device as well as a Mass storage device on Block media on QSPI.
	Users can use a Serial terminal(like Tera-term) to communicate with the board. Data written by the tera-term will be echoed back
	on the terminal by the RA board. The user at the same time can read/write the data from/to the board using a host machine.

2. Hardware Requirement:

	micro USB Cable : 1no.
	
3. Hardware connection :
	
        EK-RA6M3,EK-RA6M4,EK-RA4M3,EK-RA4M2,EK-RA6M5:
        Jumper j12: Connect pins 2-3
        Connect Jumper j15 pins
        Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	port (J11) of the board. Connect the other end of this cable to a USB port of the 
	host PC. 
 

4. USB Disconnect Behaviour :
        The application also handles removing & connecting the Peripheral USB Cable use case.
        Ex: If the user removes the peripheral USB cable from the board, the Composite USB example project will be running 
        and it waits for the cable to be connected. Once the Cable is connected back, the example project works the same.
	
   Link to Errata:https://www.renesas.com/us/en/document/mat/ek-ra6m3-v1-errata?language=en&r=1168086


Note:
1. Build Download and Run firmware on board. Connect the cable to USB peripheral connector.        
2. Confirm that the PC detects an unformatted media using the "Disk Management" application on Windows.
3. Format the drive to use it as a flash drive.