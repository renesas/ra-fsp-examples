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
    This example project demonstrates basic functionalities of mbed crypto driver on Renesas RA MCUs based on Renesas FSP.
    The EP performs following mbed crypto operations :
    1. SHA256 - setup and calculate hash, compare calculated hash value and length with their expected values.
    2. AES256 Key generation(for GCM Mode), Encryption and Decryption.
    3. ECC -P256R1 Key pair generation, Signing the hashed message with private key & verify hashed message with public key.
    4. RSA 2048 Key pair generation, Signing the hashed message with private key & verify hashed message with public key.
    
    On successful completion of each operation, success message will be displayed on the Jlink RTTViewer.
    Error and info messages will be printed on JlinkRTTViewer.

2. Hardware Requirements:
	- RA board	
	- Micro USB device cable - 1x or Type C USB device cable - 1x (For MCK-RA8T1)

3. Hardware Connections:
	Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	port of the respective board. Connect the another end of this cable to USB port of the host PC.

	For MCK-RA8T1:
	Connect the type C USB end of the type C USB device cable to DEBUG USB 
	port of the respective board. Connect the another end of this cable to USB port of the host PC.

Note : 
1) EK-RA2L1, EK-RA4M1 and EK-RA4W1 boards do not support SHA, ECC and RSA operations.

2) Initialize device back to factory default by using Renesas Device Partition Manager before run the example project.
   User can refer Section 5.3 "Setting up Hardware" in the document via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection
   For Renesas Device Partition Manager tool:
   a. e2studio: 0x220000ac

3) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220000ac
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
