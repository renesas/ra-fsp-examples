/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates basic functionalities of mbed crypto driver on Renesas RA MCUs based on Renesas FSP.
    	The EP performs following mbed crypto operations:
    	1. SHA256 - setup and calculate hash, compare calculated hash value and length with their expected values.
    	2. AES256 Key generation (for GCM Mode), Encryption and Decryption.
    	3. ECC - P256R1 Key pair generation, Signing the hashed message with private key & verify hashed message with public key.
    	4. RSA 2048 Key pair generation, Signing the hashed message with private key & verify hashed message with public key.
    
    	On successful completion of each operation, success message will be displayed on the J-Link RTT Viewer.
    	Error and info messages will be printed on J-Link RTT Viewer.

	Note:
	1) EK-RA2L1, EK-RA4M1 and EK-RA4W1 boards do not support SHA, ECC and RSA operations.
	2) Some MCUs support hardware crypto operations, some MCUs support software crypto operations. 
           Refer to Stacks -> MbedTLS (Crypto Only) -> Property > Common -> Hardware Acceleration configuration 
   	   for the specific configurations for each example project.

2. Hardware Requirements:
	Supported RA boards: EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G
			     EK-RA6M4, EK-RA6M5, RSSK-RA6T1, EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1
	1 x RA board	
	1 x Type C USB cable for programming and debugging

3. Hardware Connections:
	Connect the USB Debug port on the RA board to the host PC via a Type C USB cable.

Note:
1) Initialize device back to factory default by using Renesas Device Partition Manager before run the example project.
   User can refer Section 5.3 "Setting up Hardware" in the document via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection
   For Renesas Device Partition Manager tool on each IDE:
   - Start E2Studio -> Run -> Renesas Debug Tools -> Renesas Device Partition Manager (Available on e2studio)

2) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2206494c
   b. Keil:  	Not Available
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
