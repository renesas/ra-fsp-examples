/***********************************************************************************************************************
 * File Name    : pqc_example_app.h
 * Description  : Public interface for Post Quantum Cryptography example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef PQC_EXAMPLE_APP_H_
#define PQC_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "Post Quantum Cryptography"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module           *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to the post_quantum_cryptography_notes.md for the Example Project documentation"\
                        "\r\nand FSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe project demonstrates the basic functionality of Post Quantum Cryptography"\
                        "\r\n(PQC) on Renesas RA MCUs using the PSA Crypto API and Renesas FSP. All"\
                        "\r\ncryptographic operations are performed through the PSA Crypto API with"\
                        "\r\nunderlying Mbed TLS library support. The example showcases four distinct"\
                        "\r\nuse cases executed sequentially on a single MCU, simulating multi-party"\
                        "\r\ncryptographic operations, and a fifth use case that validates interoperability"\
                        "\r\nwith an external device in a real-world distributed environment.\r\n"\
                        "\r\nFor ML-KEM (Key Encapsulation Mechanism), the MCU first simulates a"\
                        "\r\ntwo-party key exchange where a decapsulator generates an ML-KEM keypair,"\
                        "\r\nexports the public key, and an encapsulator uses it to create a shared"\
                        "\r\nsecret through encapsulation. The decapsulator then decapsulates the"\
                        "\r\nreceived ciphertext to derive the identical shared secret, demonstrating"\
                        "\r\nquantum-resistant key establishment. The second ML-KEM use case"\
                        "\r\ndemonstrates key persistence: the decapsulator exports its keypair as a"\
                        "\r\nbackup, then restores the keypair from backup and successfully"\
                        "\r\ndecapsulates a previously stored ciphertext, verifying the restored key"\
                        "\r\nmaintains full cryptographic functionality.\r\n"\
                        "\r\nFor ML-DSA (Digital Signature Algorithm), the MCU simulates a two-party"\
                        "\r\nsigning scenario where a signer generates an ML-DSA signing keypair,"\
                        "\r\nexports the public key for distribution, and signs a message. The"\
                        "\r\nverifier imports the public key and verifies the signature to authenticate"\
                        "\r\nthe message source. The second ML-DSA use case demonstrates signing key"\
                        "\r\npersistence: the signer exports its keypair as a backup, then restores"\
                        "\r\nthe keypair from backup and signs a new message with the restored key."\
                        "\r\nSince the restored signing key corresponds to the same public key"\
                        "\r\nmaterial, signatures created before the backup restore remain verifiable"\
                        "\r\nby the verifier. Similarly, signatures created after the restore are also"\
                        "\r\nverifiable with the same verifier public key, as the restored keypair is"\
                        "\r\nidentical to the original.\r\n"\
                        "\r\nThe final use case combines both algorithms in a hybrid workflow,"\
                        "\r\nwhere the RA MCU performs ML-KEM decapsulation to establish a"\
                        "\r\nshared secret and then uses ML-DSA to sign it, with both operations"\
                        "\r\nvalidated against an external host PC rather than simulated within"\
                        "\r\nthe same device. This demonstrates that the RA MCU's PSA Crypto"\
                        "\r\nAPI implementation can correctly interoperate with an independent"\
                        "\r\nexternal party in a real-world distributed environment.\r\n"\
                        "\r\nThe EP information, menu options and error messages are displayed in the"\
                        "\r\nterminal application.\r\n"

#define EP_MENU         "\r\nMain menu:"\
                        "\r\n1. ML-KEM: Two-party key exchange"\
                        "\r\n2. ML-KEM: Export/import keypair (Backup & recovery)"\
                        "\r\n3. ML-DSA: Two-party signing & verification"\
                        "\r\n4. ML-DSA: Export/import keypair (Backup & recovery)"\
                        "\r\n5. ML-KEM decap & ML-DSA sign (Remote verify)"\
                        "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void pqc_example_app_run(void);

#endif /* PQC_EXAMPLE_APP_H_ */
