/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
    This example project demonstrates basic functionalities of mbed crypto driver on Renesas RA MCUs based on Renesas FSP.
    The EP performs following mbed crypto operations :
    1. SHA256 - setup and calculate hash, compare calculated hash value and length with their expected values.
    2. AES256 Key generation(for GCM Mode), Encryption and Decryption.
    3. ECC -P256R1 Key pair generation, Signing the hashed message with private key & verify hashed message with public key.
    4. RSA 2048 Key pair generation, Signing the hashed message with private key & verify hashed message with public key.
    
    On successful completion of each operation, success message will be displayed on the Jlink RTTViewer.
    Error and info messages will be printed on JlinkRTTViewer.



Note : EK-RA2L1, EK_RA4M1 and EK_RA4W1 boards do not support SHA, ECC and RSA operations.
1) Segger RTT block address may needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200008bc
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
