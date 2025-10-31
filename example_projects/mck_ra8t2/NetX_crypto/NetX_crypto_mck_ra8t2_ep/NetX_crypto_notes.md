# Introduction #
This project demonstrates the basic functionality of NetX Secure Crypto on Renesas RA MCUs based on Renesas FSP supporting SCE7/SCE9/RSIP-E51A/RSIP-E11A/RSIP-E50D/RSIP-E31A hardware acceleration using AzureRTOS. The example project executes AES, ECC, RSA algorithms (including different chaining modes) in multi-threaded environment using mutex to enable access to SCE/RSIP. The user can enter any key to enable concurrent execution of the different algorithms and status of the execution will be displayed on J-Link RTT Viewer.

Please refer to the [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf) for general information on example projects and [readme.txt](./readme.txt) for specifics of operation.

## Required Resources ##
To build and run the NetX Crypto example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 6.2.0
* e2 studio: Version 2025-10
* SEGGER J-Link RTT Viewer: Version 8.74
* LLVM Embedded Toolchain for ARM: Version 18.1.3

* Refer to the software required section in [Example Project Usage Guide](https://github.com/renesas/ra-fsp-examples/blob/master/example_projects/Example%20Project%20Usage%20Guide.pdf)
* Refer to [Special Topics](#special-topics) for more information of NetX Crypto setup.

### Hardware ###
* Supported RA Boards:
  * SCE7      - EK-RA6M2
  * SCE9      - EK-RA4M2, EK-RA4M3, EK-RA6M4, EK-RA6M5
  * RSIP-E51A - EK-RA8M1, EK-RA8D1, MCK-RA8T1, FPB-RA8E1
  * RSIP-E11A - EK-RA4L1
  * RSIP-E50D - EK-RA8P1, MCK-RA8T2
  * RSIP-E31A - EK-RA4C1
* 1 x Renesas RA board.
* 1 x Type-C USB cable for programming and debugging.
* 1 x Host PC.

### Hardware Connections ###
* For EK-RA4C1:  
	* The user must place jumper J6 on pins 2-3, J8 on pins 1-2, J9 on pins 2-3 and turn OFF SW4-4 to use the on-board debug functionality.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub](https://renesas.github.io/fsp/)
- [FSP Known Issues](https://github.com/renesas/fsp/issues)

# Project Notes #
## System Level Block Diagram ##
 High level block diagram
 
![netx_crypto](images/high_level_block_diagram.jpg "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| NetX Crypto | NetX Crypto with hardware acceleration demonstrating the ECC, AES, RSA crypto algorithms on Renesas MCUs. | rm_netx_crypto |
| Azure RTOS | Azure RTOS timer is used for algorithm execution time tracking. ||
| [NIST CAVP test vectors](https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program) | NIST CAVP test vectors are used in the example project to demonstrate the cryptographic operation of the SCE7/SCE9/RSIP-E51A/RSIP-E11A/RSIP-E50D/RSIP-E31A. ||

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default.

**Common Configuration Properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
| configuration.xml > BSP > Properties > Settings > Property > RA Common > Main stack size (bytes) | 0x400 | 0x1000 | Main Program thread stack is configured to store the local variables of different functions in the code. |
| configuration.xml > Stacks > HAL/Common > Azure RTOS NetX Crypto HW Acceleration (rm_netx_secure_crypto) > Properties > Settings > Property > Common > Hardware Acceleration > TRNG | Enabled | Enabled | Setting for TRNG should be enable to test the random key generation for AES method. |
| configuration.xml > Stacks > AES Thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 0x2400 | Stack size of AES thread should require minimum of 9KB to execute the algorithms. |
| configuration.xml > Stacks > ECC Thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 0x2000 | Stack size of ECC thread should require minimum of 8KB to execute the algorithms. |
| configuration.xml > Stacks > RSA Thread > Properties > Settings > Property > Thread > Stack size (bytes) | 1024 | 0x2400 | Stack size of RSA thread should require minimum of 9KB to execute the algorithms. |
| configuration.xml > Stacks > RTT Thread > Properties > Settings > Property > Thread > Priority | 1 | 2 | RTT thread priority is lowered to ensure higher priority for critical crypto processing threads (ECC, AES, and RSA). |

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| HW_SCE_RNG_Read | This API is used to generate true random numbers from the hardware-based TRNG in the Secure Crypto Engine (SCE). |
| nx_crypto_initialize | This API is used to initialize the NetX Crypto. |
| _nx_crypto_method_aes_init | This API is used to initialize the NetX Crypto AES method. |
| _nx_crypto_method_aes_operation | This API is used to do the encrypt and decrypt operations in AES method. |
| _nx_crypto_method_aes_cleanup | This API is used to clean up the AES metadata. |
| _nx_crypto_method_ecdh_init | This API is used to initialize the NetX Crypto ECDH method. |
| _nx_crypto_method_ecdh_operation | This API is used to perform the ECDH operations. |
| _nx_crypto_method_ecdh_cleanup | This API is used to clean up the ECDH metadata. |
| _nx_crypto_method_ecdsa_init | This API is used to initialize the NetX Crypto ECDSA method. |
| _nx_crypto_method_ecdsa_operation | This API is used to perform the ECDSA operations. |
| _nx_crypto_method_ecdsa_cleanup | This API is used to clean up the ECDSA metadata. |
| _nx_crypto_method_hmac_sha256_init | This API is used to initialize the NetX Crypto HMAC-SHA256 method. |
| _nx_crypto_method_hmac_sha256_operation | This API is used to perform encryption and decryption operation in HMAC-SHA256 method. |
| _nx_crypto_method_hmac_sha256_cleanup | This API is used to clean up the HMAC-SHA256 metadata. |
| _nx_crypto_method_rsa_init | This API is used to initialize the NetX Crypto RSA method. |
| _nx_crypto_method_rsa_operation | This API is used to do the encrypt and decrypt operations in RSA method. |
| _nx_crypto_method_rsa_cleanup | This API is used to clean up the RSA metadata. |
| _nx_crypto_method_sha256_init | This API is used to initialize the NetX Crypto SHA-256 method. |
| _nx_crypto_method_sha256_operation | This API is used to encrypt and decrypt operations in SHA-256 method. |
| _nx_crypto_method_sha256_cleanup | This API is used to clean up the SHA-256 metadata. |

Note: The example project includes three threads exercising the above crypto APIs. AES, ECC, and RSA threads operate the corresponding cryptographic APIs. Note that the SHA APIs are exercised in the RSA thread.

Refer to [Azure RTOS NetX Secure Crypto Hardware Acceleration](https://docs.microsoft.com/en-us/azure/rtos/netx/netx-crypto/chapter1) for more details of API used.

## Verifying Operation ##
Import, build and debug the EP (Refer section Starting Development of FSP User Manual). After running the EP, open J-Link RTT Viewer to see the output status of the EP.  

The images below showcase the output for **concurrent execution of the algorithms** on **J-Link RTT Viewer** when the user input:

![netx_crypto](images/rtt_concurrent_1.jpg "RTT output")

![netx_crypto](images/rtt_concurrent_2.jpg "RTT output")

![netx_crypto](images/rtt_concurrent_3.jpg "RTT output")
