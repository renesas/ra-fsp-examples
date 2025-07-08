/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

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
	- Micro USB device cable - 1x

3. Hardware Connections:
	Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	port of the respective board. Connect the another end of this cable to USB port of the host PC. 

Note: EK-RA2L1, EK-RA4M1 and EK-RA4W1 boards do not support SHA, ECC and RSA operations.

1) Initialize device back to factory default by using Renesas Device Partition Manager before run the example project.
   User can refer Section 5.3 "Setting up Hardware" in the document via link:
   https://www.renesas.com/us/en/document/apn/renesas-ra-security-design-arm-trustzone-ip-protection
   For Renesas Device Partition Manager tool on each IDE:
   - Start E2Studio -> Run -> Renesas Debug Tools -> Renesas Device Partition Manager (Available on e2studio)

2) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22004de0
   b. Keil:  	Not Available 
   c. IAR: 	Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
