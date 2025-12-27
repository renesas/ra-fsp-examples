#/**********************************************************************************************************************
#* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
#*
#* SPDX-License-Identifier: BSD-3-Clause
#*********************************************************************************************************************/

#/**********************************************************************************************************************
# * File: bootmode_demonstration_code_RA8.py
# * This file demonstrates below functionalities
# *     Read current DLM, Protection and Authentication level states
# *     Set up TrustZone boundary
# *     Read TrustZone boundary
# *     Inject DLM AL2 and AL1 keys
# *     Transition the Protection Level state from PL2 to PL1
# *     Transition the Protection Level state from PL1 to PL0
# ***********************************************************************************************************************/

# Both serial and USB COM port can be used to communicate with the Boot firmware
import serial
import time
import serial.tools.list_ports
# DLM .rkey file is base64 encoded
import base64
# Response Value Calculation
from Crypto.Hash import CMAC
from Crypto.Cipher import AES

# Set DEBUG_OUTPUT_ENABLE to 1 to enable more debug output
DEBUG_OUTPUT_ENABLE = 0

# Set INVOKE_DISABLE_INITIALIZE_COMMAND to 1 to disable initialize command
# !!!!! WARNING !!!!!
# This is non-reversible, so invoke with caution !!!
INVOKE_DISABLE_INITIALIZE_COMMAND = 0

# Adjust these delays for your hardware system
SHORT_DELAY = 0.15
LONG_DELAY = 3
AL_DATA_RESPONSE_DELAY = 5

# Unknown DLM Key Type
UNKNOWN_KEY = 0xFF

# The key type number for DLM keys is 0x00.
# The other user key type number is defined in Boot Firmware app note, section "User Key List". 
# The meaning of each entry is defined in key_type_string.
key_type_num = [0x00,
                0x05,
                0x06,
                0x07,
                0x08,
                0x09,
                0x0A,
                0x0B,
                0x0C,
                0x0D,
                0x0E,
                0x0F,
                0x10,
                0x11,
                0x12,
                0x13,
                0x14,
                0x15,
                0x16,
                0x17,
                0x18,
                0x19,
                0x1A,
                0x1B,
                0x1C,
                0x1D,
                0x1E,
                0x1F,
                0x20,
                0x21,
                0x22,
                0x23,
                0x24,
                0x25,
                0x26,
                0x27,
                0x28,
                0x29,
                0x2A,
                0x2B,
                0x2C,
                0x2D,
                0x2E,
                0x2F,
                0x30,
                0xFE,
                0xFF ]

key_type_string = ["DLM Authentication Key",
                "AES-128",
                "AES-192",
                "AES-256",
                "AES-128 XTS",
                "AES-256 XTS",
                "RSA-1024 Public Key",
                "RSA-1024 Private Key",
                "RSA-2048 Public Key",
                "RSA-2048 Private Key",
                "RSA-3072 Public Key",
                "RSA-3072 Private Key",
                "RSA-4096 Public Key",
                "RSA-4096 Private Key",
                "ECC P192 Public Key",
                "ECC P192 Private Key",
                "ECC P224 Public Key",
                "ECC P224 Private Key",
                "ECC P256 Public Key",
                "ECC P256 Private Key",
                "ECC P384 Public Key",
                "ECC P384 Private Key",
                "HMAC-SHA224",
                "HMAC-SHA256",
                "ECC P256r1 Public Key 80",
                "ECC P256r1 Private Key 48",
                "ECC P384r1 Public Key 112",
                "ECC P384r1 Private Key 64",
                "ECC P512r1 Public Key 144",
                "ECC P512r1 Private Key 80",
                "ECC secp256k1 Public Key",
                "ECC secp256k1 Private Key",
                "ECC P521 Public Key",
                "ECC P521 Private Key",
                "Ed25519 Public Key",
                "Ed25519 Private Key",
                "HMAC-SHA384",
                "HMAC-SHA512",
                "HMAC-SHA512-224",
                "HMAC-SHA512-256",
                "HMAC-SHA3-224",
                "HMAC-SHA3-256",
                "HMAC-SHA3-384",
                "HMAC-SHA3-512",
                "ChaCha20-Poly-1305",
                "RSA-2048 Public Key for TLS",
                "Key update key" ]

#########################################################
# Establish communication with the boot firmware
#########################################################

# ==================================
# This routine demonstrates the communication setting phase handling
def communication_setting():
    loopcount = 20

    print("Sending three 0x00 to target to start Communication Setting Phase")
    while loopcount != 0:
        # Try this a few times
        ser.write(b'\x00')
        # This sleeping time can change based on what source clock is used for 
        # the MCU. Reference the Section "Communication Setting Phase" in 
        # Renesas Boot Firmware app note for details".
        time.sleep(SHORT_DELAY)
        ser.write(b'\x00')
        time.sleep(SHORT_DELAY)
        ser.write(b'\x00')
        time.sleep(SHORT_DELAY)
        h = ser.read()
        if h == b'\x00':
            print("Success: ACK received")
            break
        loopcount -= 1
        time.sleep(SHORT_DELAY)

    return loopcount

#########################################################
# Boot firmware command supporting subroutines
#########################################################

# ==================================
# This routine prints a message wrapped by lines above & below, to
# indicate a section in output being displayed.
def print_header_message(message):
    mlen = len(message)
    wrap_string = "=" * mlen
    print("\n" + wrap_string + "\n" + message + "\n" + wrap_string + "\n")

#===================================
# This routine cleans up the system on exit with error.
def terminate_execution():
    print_header_message("Demonstration finished.")
    ser.close()
    quit()

# ==================================
# This routine print the type of key based on the key index. 
def print_key_type_string(number, sig_type):
    if sig_type == 0x03:
        # For signature type 0x03 (RA8x1 MCU Group), mark certain key types as unsupported
        if number in [0x2C, 0x2D, 0x2E, 0x2F, 0x30]:
            print("!*** ERROR : MCU does not support this key type: ", number)
            terminate_execution()

    # Handle all other cases
    if number in key_type_num:
        elem_num = key_type_num.index(number)
        print("Key Type = '",key_type_string[elem_num],"'")
    else:
        print("!*** ERROR : Key type unknown : ", number)
        terminate_execution()

# ==================================
# This routine calculates the check sum. The low byte is the check sum.
def calc_sum(lotsofbytes):
    count = 0

    for index, value in enumerate(lotsofbytes):
        count = count + value

    count = count % 256
    summation = (256-count) % 256
    h = summation.to_bytes(1, "big")

    return h

# ==================================
# This routine prints the array in hex format.
def print_bytes_hex(lotsofbytes):
    bstring = r"b'"

    for index, value in enumerate(lotsofbytes):
        bstring = bstring + r"\x" + format(value,'02x')

    bstring = bstring + r"'"
    print(bstring)

# ==================================
# This routine reads out the current TrustZone boundary setup.
def print_boundary_region(return_packet, sig_type):
    # For signature type 0x03 (RA8x1 MCU Group)
    if sig_type == 0x03:
        CFS_1 = return_packet[6]
        CFS_2 = return_packet[7]
        h = CFS_1*256 + CFS_2
        secure_code_flash = f' - The secure code flash region size is {h} KB'
        print(secure_code_flash)

        DFS_1 = return_packet[8]
        DFS_2 = return_packet[9]
        h = DFS_1*256 + DFS_2
        secure_data_flash = f' - The secure data flash region size is {h} KB'
        print(secure_data_flash)
    else:
        # RA8P1 MCU
        CMS_1 = return_packet[6]
        CMS_2 = return_packet[7]
        h = CMS_1*256 + CMS_2
        secure_code_mram = f' - The secure code MRAM region size is {h} KB'
        print(secure_code_mram)

# ==================================
# This routine receives the entire response data packet.    
def receive_data_packet():
    packet = bytearray()
    SOD = ser.read()

    if SOD != b'\x81':
        print("\n!!! ERROR : Unexpected SOD byte value received in data packet : ", SOD, " (", int.from_bytes(SOD, "big"),")")
        print("!!!         Try resetting board with MD pulled low, then running again.") 
        terminate_execution()

    packet = packet + SOD
    LNH = ser.read()
    packet = packet + LNH
    LNL = ser.read()
    packet = packet + LNL

    # +2 is so that get SUM and ETX bytes too
    for loop in range(0, ((int.from_bytes(LNH, "big") * 256) + int.from_bytes(LNL, "big") + 2)):
        x = ser.read()
        packet = packet + x

    return packet

# ==================================
# This routine calculates response value or authentication code. Refer to "Response Value Calculation"
def calc_response_value(data, key):
    # Check data is 16 bytes (128-bit) or not
    if len(data) != 16:
        print("Data length is not 128-bit (16 bytes).")
        return 0

    # Create a CMAC object
    cipher = CMAC.new(key, ciphermod=AES)
    # Generate the CMAC
    cipher.update(data)
    mac = cipher.digest()
    # Fill “1” to lower 16 bytes of MAC on the Data Packet because the calculated Response is 16 bytes.
    response = mac + (b'\x01' * 16)

    return response

# ==================================
# This routine parses the DLM key file
# The decode DLM key data buffer and the size of the encrypted key are returned.
def parse_the_dlm_key_field(text_file, dlm_key_type, sig_type):
    # Read entire file to a string
    data = text_file.read()

    # Close file
    text_file.close()

    if (dlm_key_type == b'\x01'):
        print("Start Processing AL2 rkey file") 
    elif (dlm_key_type == b'\x02'):
        print("Start Processing AL1 rkey file") 

    if (DEBUG_OUTPUT_ENABLE == 1):
        print(data)

    if not (data.startswith("-----BEGIN RENESAS KEY-----\n")) or not (data.endswith("-----END RENESAS KEY-----\n")):
        print("!*** ERROR : Incorrect rkey file header and/or footer")
        return 0, 0
    else:
        if (DEBUG_OUTPUT_ENABLE == 1):
            print("\nCorrect rkey file header and footer found")

        data = data.removeprefix("-----BEGIN RENESAS KEY-----\n")
        data = data.removesuffix("-----END RENESAS KEY-----\n")

        if (DEBUG_OUTPUT_ENABLE == 1):
            print("Header & Footer removed \n") 
            print(data)

        print("Base64 Decoding key") 
        message_bytes = base64.b64decode(data)

        if (DEBUG_OUTPUT_ENABLE == 1):
            print("\nBase64 decoded. Here is the decoded data: \n") 
            print_bytes_hex(message_bytes)

        if (DEBUG_OUTPUT_ENABLE == 1):
            print("\nrkey file structure \n") 

        magic_code = message_bytes [0:4]
        if magic_code == b'REK1' :
            if (DEBUG_OUTPUT_ENABLE == 1):
                print("Correct Magic Code found : ", magic_code)
        else:
            print("!!! ERROR : Incorrect Magic Code found : ", magic_code)

        suite_version = int.from_bytes(message_bytes [4:8], byteorder='big', signed = False)
        if suite_version == 1 :
            if (DEBUG_OUTPUT_ENABLE == 1):
                print("Correct Suite Version found : ", suite_version)
        else:
            print("!!! ERROR : Incorrect Suite Version : ", suite_version)

        reserved_field7 = int.from_bytes(message_bytes [8:15], byteorder='big', signed = False)
        if reserved_field7 == 0 :
            if (DEBUG_OUTPUT_ENABLE == 1):
                print("Correct Reserved field[7] found : ", reserved_field7)
        else:
            print("!!! ERROR : Incorrect Reserved field[7] : ", reserved_field7)

        key_type = int.from_bytes(message_bytes [15:16], byteorder='big', signed = False)
        if (DEBUG_OUTPUT_ENABLE == 1):
            print("Key Type = ", key_type)
        print_key_type_string(key_type, sig_type)

        encrypted_key_size = int.from_bytes(message_bytes [16:20], byteorder='big', signed = False)
        if (DEBUG_OUTPUT_ENABLE == 1):
            print("Encrypted Key Size  = ", encrypted_key_size)
            print("\nW_UFPK \n") 

        w_ufpk = message_bytes [20:56]
        if (DEBUG_OUTPUT_ENABLE == 1):
            print_bytes_hex(w_ufpk)
            print('Note that when injecting the key, user needs to remove the first 4 bytes')
            print("\nInitialize Vector \n") 

        initial_vector = message_bytes [56:72]
        if (DEBUG_OUTPUT_ENABLE == 1):
            print_bytes_hex(initial_vector)
            print("\nEncrypted Key \n") 

        encrypted_key = message_bytes [72:72+encrypted_key_size]
        if (DEBUG_OUTPUT_ENABLE == 1):
            print_bytes_hex(encrypted_key)
            print("\nData CRC \n") 

        data_crc = message_bytes [72+encrypted_key_size:72+encrypted_key_size+4]
        if (DEBUG_OUTPUT_ENABLE == 1):
            print_bytes_hex(data_crc)

        if len(message_bytes) == (72+encrypted_key_size+4) :
            if (DEBUG_OUTPUT_ENABLE == 1):
                print("\nSizes match in processing rkey file \n")
        else: 
            print("\n!!! ERROR : Size mismatch in processing rkey file")

        print("Finished : Processing rkey file.")
        return(message_bytes, encrypted_key_size)

#########################################################
# Boot firmware command demonstrations
#########################################################

# ==================================
# This routine demonstrates "DLM Key Injection" command.
# The supported key types are AL1 and AL2 key.
def command_inject_dlm_key(dlm_key_type, sig_type):
    if (dlm_key_type != b'\x01' and dlm_key_type != b'\x02'):
        print("This key type is not supported")
        # Return with unknown DLM key type
        return UNKNOWN_KEY
    elif (dlm_key_type == b'\x01'):
        if sig_type == 0x03:
            # RA8x1 MCU Group
            text_file = open("./dlm_keys/AL2_KEY_RA8M1.rkey", "r")
        else:
            # RA8P1 MCU
            text_file = open("./dlm_keys/AL2_KEY_RA8P1.rkey", "r")
    elif (dlm_key_type == b'\x02'):
        if sig_type == 0x03:
            # RA8x1 MCU Group
            text_file = open("./dlm_keys/AL1_KEY_RA8M1.rkey", "r")
        else:
            # RA8P1 MCU
            text_file = open("./dlm_keys/AL1_KEY_RA8P1.rkey", "r")

    message_bytes, encrypted_key_size = parse_the_dlm_key_field(text_file, dlm_key_type, sig_type)
    if (message_bytes != 0 and encrypted_key_size != 0):
        # Proceed with the DLM key injection command packet stage
        SOH=b'\x01' # Start of command packet
        LNH=b'\x00'
        LNL=b'\x02'
        CMD=b'\x28' # DLM Key injection command
        KYTY=dlm_key_type
        SUM=calc_sum(LNH + LNL + CMD + KYTY)
        EXT=b'\x03'
        command = SOH + LNH + LNL + CMD + KYTY + SUM + EXT

        print("\nSending DLM key injection command packet: ")
        print_bytes_hex(command)
        ser.write(command)
        time.sleep(LONG_DELAY)

        # Acquire the response of the command packet stage
        return_packet = receive_data_packet()
        # The RES byte is the fourth index
        RES = return_packet[3]
        if RES != 0x28:
            print("DLM key injection command packet - FAIL")
        else:
            # The STS is the fifth index
            STS = return_packet[4]
            if STS != 0x00:
                print("DLM key injection command packet - FAIL")
            # The SUM is the 14th index
            SUM = return_packet[13]
            if SUM == 0xD6:
                print("\nDLM key injection command packet - SUCCESS")
            # Start the data packet stage
            SOD=b'\x81' # Start of data packet
            LNH=b'\x00'
            LNL=b'\x55' # 85 byte: one byte (RES) + shared key ring number (4 bytes) + w-ufpk (32 bytes) 
                        #+ initialize vector (16 bytes) + encrypted dlm key and mac (32 bytes)
            RES=b'\x28' # OK byte
            SKR=b'\x00'*4 #Shared key ring number.
            SUM=calc_sum(LNH + LNL + RES + SKR + message_bytes [24:56] + message_bytes [56:72] + \
                        message_bytes [72:72+encrypted_key_size])
            ETX = b'\x03'
            command = SOD + LNH + LNL + RES + SKR + message_bytes [24:56]  + message_bytes [56:72] + \
                        message_bytes [72:72+encrypted_key_size] + SUM + ETX

            print("\nSending DLM key injection data packet: ")
            print_bytes_hex(command)
            ser.write(command)
            time.sleep(LONG_DELAY)

            # Acquire the response of the data packet stage 
            return_packet = receive_data_packet() 
            # The RES byte is the fourth index
            RES = return_packet[3]
            if RES != 0x28:
                print("DLM key injection data packet failed")
            else:
                # The STS is the fifth index
                STS = return_packet[4]
                if STS != 0x00:
                    print("Injecting DLM key failed")
                elif KYTY == b'\x01':
                    print("\nDLM key injection data packet - SUCCESS\n")
                    print("Injecting AL2 key is successful")
                elif KYTY == b'\x02':
                    print("\nDLM key injection data packet - SUCCESS\n")
                    print("Injecting AL1 key is successful")

    return RES

# ==================================
# This routine demonstrates "DLM Key Verify Command".
# The supported key types are AL1 and AL2 key.    
def command_verify_dlm_key(dlm_key_type): 
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x02'
    CMD=b'\x29' # DLM Key verify command
    KYTY=dlm_key_type # DLM Key type 
    SUM=calc_sum(LNH + LNL + CMD + KYTY)
    ETX=b'\x03'
    command = SOH + LNH + LNL + CMD + KYTY + SUM + ETX

    if KYTY == b'\x01':
        print("Sending DLM AL2 key verify command:")
    elif KYTY == b'\x02':
        print("Sending DLM AL1 key verify command:")
    else:
        print("Unsupported DLM Key type")

    print_bytes_hex(command)
    ser.write(command)
    time.sleep(LONG_DELAY)

    # Acquire the response
    return_packet = receive_data_packet()
    # The RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x29:
        print("DLM key verification - FAIL")
    else:
        # The STS is the fifth index
        STS = return_packet[4]
        if STS != 0x00:
            print("Verifying injected DLM key failed")
        elif KYTY == b'\x01':
            print("Verifying injected AL2 key is successful")
        elif KYTY == b'\x02':
            print("Verifying injected AL1 key is successful")

    return RES

# ==================================
# This routine demonstrates "TrustZone Boundary Request Command". 
def command_trustzone_boundary_request(sig_type):
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    CMD=b'\x4F' # Boundary request command
    SUM=b'\xB0'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT  

    print("Sending read boundary region command:")
    print_bytes_hex(command)
    ser.write(command)
    time.sleep(LONG_DELAY)

    # Acquire the response
    return_packet = receive_data_packet()
    # The RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x4F:
        print("Read boundary region - FAIL")
    else:
        print("Read boundary region - SUCCESS")
    print_boundary_region(return_packet, sig_type)

    return RES

# ==================================
# This routine demonstrates "TrustZone Boundary Setting Command". 
def command_setup_trustzone_boundary(sig_type):
    print("Configuring device with :")
    if sig_type == 0x03:
        # RA8x1 MCU Group
        SOH=b'\x01'
        LNH=b'\x00'
        LNL=b'\x0B'
        CMD=b'\x4E'     # Boundary setting command
        RSV_1=b'\x00'   # Unused code
        RSV_2=b'\x00'
        print(" - 512KB of Code Flash Secure region")
        CFS_1=b'\x02'   # 512KB
        CFS_2=b'\x00'
        print(" - 4KB of Data Flash Secure region")
        DFS_1=b'\x00'
        DFS_2=b'\x04'   # 4KB
        RSV_3=b'\x00'   # Unused code
        RSV_4=b'\x00'
        RSV_5=b'\x00'   # Unused code
        RSV_6=b'\x00'
        SUM=calc_sum(LNH + LNL + CMD + RSV_1 + RSV_2 + CFS_1 + CFS_2 + \
            DFS_1 + DFS_2 + RSV_3 + RSV_4 + RSV_5 + RSV_6)
        EXT=b'\x03'
        command = SOH + LNH + LNL + CMD + RSV_1 + RSV_2 + CFS_1 + CFS_2 + \
            DFS_1 + DFS_2 + RSV_3 + RSV_4 + RSV_5 + RSV_6 + SUM + EXT
    else:
        # RA8P1 MCU
        SOH=b'\x01'
        LNH=b'\x00'
        LNL=b'\x0B'
        CMD=b'\x4E'     # Boundary setting command
        RSV_1=b'\x00'   # Unused code
        RSV_2=b'\x00'
        print(" - 512KB of Code MRAM Secure region")
        CMS_1=b'\x02'   # 512KB
        CMS_2=b'\x00'
        RSV_3=b'\x00'   # Unused code
        RSV_4=b'\x00'  
        RSV_5=b'\x00'   # Unused code
        RSV_6=b'\x00'
        RSV_7=b'\x00'   # Unused code
        RSV_8=b'\x00'
        SUM=calc_sum(LNH + LNL + CMD + RSV_1 + RSV_2 + CMS_1 + CMS_2 + \
            RSV_3 + RSV_4 + RSV_5 + RSV_6 + RSV_7 + RSV_8)
        EXT=b'\x03'
        command = SOH + LNH + LNL + CMD + RSV_1 + RSV_2 + CMS_1 + CMS_2 + \
            RSV_3 + RSV_4 + RSV_5 + RSV_6 + RSV_7 + RSV_8 + SUM + EXT

    print("Sending TrustZone boundary setup command:")
    print_bytes_hex(command)
    ser.write(command)
    time.sleep(LONG_DELAY)

    # Acquire the response
    return_packet = receive_data_packet()
    # The RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x4E:
        print("\n*** ERROR : Set up boundary region - FAIL")
    else:
        print("\nSet up boundary region - SUCCESS") 

    return RES

# ==================================
# This routine demonstrates the "Inquiry Command". 
# This command checks if boot firmware is in "Command acceptable phase" or not.
def command_inquiry():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    CMD=b'\x00'
    SUM=b'\xFF'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT

    print("Sending Inquiry command:")
    print_bytes_hex(command)
    ser.write(command)
    time.sleep(LONG_DELAY)    

# ==================================
# This routine demonstrates the "DLM State Request Command". 
# The command execution result and the DLM state are returned.
def command_DLMstateRequest():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    CMD=b'\x2C' # DLM state request command code
    SUM=b'\xD3'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT   

    print("Sending DLM State Request command:")
    print_bytes_hex(command)
    ser.write(command)
    time.sleep(LONG_DELAY)

    # Acquire the response
    return_packet = receive_data_packet()
    # The RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x2C:
        print("*** ERROR : Read DLM state - FAIL")
        DLM = b'\xFF'
    else:    
        # The DLM byte is the fifth index
        DLM = return_packet[4]
        match DLM:
            case 0x01:
                print("Current DLM state is Chip Manufacturing,") 
                print("User must use the DLM state transition command to transition to OEM state")
            case 0x04:
                print("Current DLM state is OEM") 
            case 0x06:
                print("Current DLM state is LCK_BOOT")
            case 0x07:
                print("Current DLM state is RMA_REQ")
            case 0x08:
                print("Current DLM state is RMA_ACK")
            case 0x09:
                print("Current DLM state is RMA_RET")
            case _:
                print("Unknown DLM state")

    return RES, DLM

# ==================================
# This routine demonstrates the "Authentication Level Request Command". 
# The command execution result and the Authentication level are returned.
def command_AuthenticationLevelRequest():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    CMD=b'\x75' # Authentication level request command code
    SUM=b'\x8A'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT   

    print("Sending Authentication Level Request command:")
    print_bytes_hex(command)
    ser.write(command)
    time.sleep(LONG_DELAY)

    # Acquire the response
    return_packet = receive_data_packet()
    # The RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x75:
        print("*** ERROR : Read Authentication Level - FAIL")
        CAL = b'\xFF'
    else:    
        # The AL byte is the fifth index
        CAL = return_packet[4]
        match CAL:
            case 0x02:
                print("Current Authentication Level (AL) is AL2") 
            case 0x03:
                print("Current Authentication Level (AL) is AL1") 
            case 0x04:
                print("Current Authentication Level (AL) is AL0") 
            case _:
                print("Unknown Authentication Level")

    return RES, CAL

# ==================================
# This routine demonstrates the "Protection Level Request Command". 
# The command execution result and the Protection level are returned.
def command_ProtectionLevelRequest():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    CMD=b'\x73' # Protection level request command code
    SUM=b'\x8C'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT   

    print("Sending Protection Level Request command:")
    print_bytes_hex(command)
    ser.write(command)
    time.sleep(LONG_DELAY)

    # Acquire the response
    return_packet = receive_data_packet()
    # The RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x73:
        print("*** ERROR : Read Protection Level - FAIL")
        CPL = b'\xFF'
    else:    
        # The CPL byte is the fifth index
        CPL = return_packet[4]
        match CPL:
            case 0x02:
                print("Current Protection Level (PL) is PL2") 
            case 0x03:
                print("Current Protection Level (PL) is PL1") 
            case 0x04:
                print("Current Protection Level (PL) is PL0")
            case _:
                print("Unknown Protection Level")

    return RES, CPL

# ==================================
# This routine demonstrates the "DLM State Transit Command". 
def command_DLMstateTransition(source_DLM_state, target_DLM_state):
    RES, SDLM = command_DLMstateRequest()

    if RES != 0x2C:
        print("Read DLM state - FAIL")
    elif SDLM != int.from_bytes(source_DLM_state, "big"):
        print("Adjust the source DLM state!")
    else:   
        if target_DLM_state != source_DLM_state and source_DLM_state != b'\x06' \
            and source_DLM_state != b'\x07' and source_DLM_state != b'\x08' and \
            source_DLM_state != b'\x09': 
            SOH=b'\x01'
            LNH=b'\x00'
            LNL=b'\x03'
            #0x71: DLM state transit command
            CMD=b'\x71'
            SDLM=source_DLM_state
            DDLM=target_DLM_state
            SUM=calc_sum(LNH + LNL + CMD + SDLM + DDLM)
            EXT=b'\x03'
            command = SOH + LNH + LNL + CMD + SDLM + DDLM + SUM + EXT

            print("\nSending DLM State Transition command:")
            print_bytes_hex(command)
            ser.write(command)
            time.sleep(LONG_DELAY)

            # Acquire the response
            return_packet = receive_data_packet()
            # The RES byte is the fourth index
            RES = return_packet[3]
            if RES != 0x71:
                print("DLM Transition command - FAIL")
            elif DDLM == b'\x04':
                print("DLM state changed to OEM.")
                # the STS is the fifth index
                STS = return_packet[4]
                if STS != 0x00:
                    print("DLM state Transition command - FAIL")

    return RES

# ==================================
# This routine demonstrates the "Authentication Level Transit Command". 
def command_AuthenticationLevelTransition(target_AL_state):
    RES, DLM = command_DLMstateRequest()

    if RES != 0x2C:
        print("Read DLM state - FAIL")
        return RES
    elif DLM != 0x04:
        print("DLM state is not OEM, cannot proceed")
        return 0xFF

    RES, CAL = command_AuthenticationLevelRequest()

    if RES != 0x75:
        print("Read Authentication Level - FAIL")
    elif CAL != int.from_bytes(target_AL_state, "big"):
        SOH=b'\x01'
        LNH=b'\x00'
        LNL=b'\x04'
        #0x30: Authentication command
        CMD=b'\x30'
        SDLM=CAL.to_bytes(1, "big")
        DDLM=target_AL_state
        CHCT=b'\x00'
        SUM=calc_sum(LNH + LNL + CMD + SDLM + DDLM + CHCT)
        EXT=b'\x03'
        command = SOH + LNH + LNL + CMD + SDLM + DDLM + CHCT + SUM + EXT

        print("\nSending DLM State Transition command:")
        print_bytes_hex(command)
        ser.write(command)
        time.sleep(LONG_DELAY)

        # Acquire the response
        return_packet = receive_data_packet()
        # The RES byte is the fourth index
        RES = return_packet[3]
        if RES != 0x30:
            print("Authentication Transition command - FAIL")
        else:
            challenge_data = return_packet[4:20]
            print("Challenge data : ")
            print_bytes_hex(challenge_data)
            # Calculate the response value using AES-128 CMAC
            response_data = calc_response_value(challenge_data, b'\x00'*16)
            print("Response data : ")
            print_bytes_hex(response_data)

            # Send Data Packet with Response Value
            SOD=b'\x81'
            LNH=b'\x00'
            LNL=b'\x21'
            RES=b'\x30'
            MAC=response_data
            SUM=calc_sum(LNH + LNL + RES + MAC)
            ETX = b'\x03'
            command = SOD + LNH + LNL + RES + MAC + SUM + ETX
            
            print("\nSending Authentication Response Value:")
            print_bytes_hex(command)
            ser.write(command)
            time.sleep(AL_DATA_RESPONSE_DELAY)

            # Acquire the response
            return_packet = receive_data_packet()
            # The RES byte is the fourth index
            RES = return_packet[3]
            # The STS is the fifth index
            STS = return_packet[4]
            if RES != 0x30 and STS != 0x00:
                print("Authentication Response Value - FAIL")
            else:
                if DDLM == b'\x02':
                    print("AL state changed to AL2.")
                elif DDLM == b'\x03':
                    print("AL state changed to AL1.")

    return RES

# ==================================
# This routine demonstrates the "Protection Level Transit Command". 
def command_ProtectionLevelTransition(target_PL_state):
    RES, DLM = command_DLMstateRequest()

    if RES != 0x2C:
        print("Read DLM state - FAIL")
        return RES
    elif DLM != 0x04:
        print("DLM state is not OEM, cannot proceed")
        return 0xFF

    RES, CPL = command_ProtectionLevelRequest()

    if RES != 0x73:
        print("Read Protection Level - FAIL")
        return RES
    
    target_PL_state = int.from_bytes(target_PL_state, "big")
    if target_PL_state < CPL:
        print("Protection Level upgrade -> Checking Authentication Level")
        RES, CAL = command_AuthenticationLevelRequest()
        if RES != 0x75:
            print("Read Authentication Level - FAIL")
            return RES
        elif CAL != 0x02:
            print("Transiting to AL2")
            RES = command_AuthenticationLevelTransition(b'\x02')
            if RES != 0x30:
                print("Authentication Level Transition - FAIL")
                return RES
    
    # Transition to the target Protection Level
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x03'
    #0x72: Protection Level transition command
    CMD=b'\x72'
    SDLM=CPL.to_bytes(1, "big")
    DDLM=target_PL_state.to_bytes(1, "big")
    SUM=calc_sum(LNH + LNL + CMD + SDLM + DDLM)
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SDLM + DDLM + SUM + EXT

    print("\nSending Protection Level Transition command:")
    print_bytes_hex(command)
    ser.write(command)
    time.sleep(LONG_DELAY)

    # Acquire the response
    return_packet = receive_data_packet()
    # The RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x72:
        print("Protection Level Transition command - FAIL")
    else:
        if DDLM == b'\x02':
            print("Protection Level changed to PL2.")
        elif DDLM == b'\x03':
            print("Protection Level changed to PL1.")
        elif DDLM == b'\x04':
            print("Protection Level changed to PL0.")
    
    return RES

# ==================================
# This routine demonstrates the "Signature Request Command".
def command_signature_request():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    #0x3A: Signature request command
    CMD=b'\x3A'
    SUM=b'\xC5'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT

    print("Sending Signature Request command:")
    print_bytes_hex(command)
    ser.write(command)
    time.sleep(LONG_DELAY)

    # Acquire the response
    return_packet = receive_data_packet()
    # The RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x3A:
        print("Signature Request - FAIL")
    else:
        print("Signature Request - SUCCESS")
        # The TYP byte is the tenth index
        TYP = return_packet[9]
        # The PTN byte starts from the thirtieth index
        PTN = return_packet[29:44]
        product_type_name = PTN.rstrip(b'\x20').decode('ascii')
        print("Product Type Name: " + product_type_name)

    return TYP

# ==================================
# The "Initialize command" can be disabled using the 
# "Parameter setting command". 
#  !!!!! WARNING !!!!!
#  This step is non-reversible, so invoke with caution!
def command_disable_initialize():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x03'
    #0x51: Parameter setting command
    CMD=b'\x51'
    # Parameter ID
    PMID=b'\x01'
    # Parameter Data 0x00 is for disable Initialize
    PRMT=b'\x00'
    SUM=calc_sum(LNH + LNL + CMD + PMID + PRMT)
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + PMID + PRMT + SUM + EXT

    print("\nSending Disable Initialize command:")
    print_bytes_hex(command)
    print("\nWarning : After MCU Initialize is disabled, it can never be re-enabled.")
    ser.write(command)
    time.sleep(LONG_DELAY)

    # Acquire the response
    return_packet = receive_data_packet()
    # The RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x51:
        print("\nDisable Initialize command - FAIL")
    else:
        print("Disable Initialize command - SUCCESS")

# ================================================================
# Main application
# ================================================================
ports = list(serial.tools.list_ports.comports())

raUsbBootFound = False

print_header_message ("Com port selection")

for p in ports:
    print(p)
for p in ports:    
    if 'RA USB Boot' in p.description:
        raComPort = p.name
        raUsbBootFound = True
        print("\nAuto-selected " + raComPort + " for RA Boot mode USB CDC connection")

if not(raUsbBootFound):
    raComPort = input ("\nPlease enter com port to use : ")

print_header_message ("Opening com port")

ser = serial.Serial(raComPort, 9600, timeout=0, write_timeout=0)
time.sleep(LONG_DELAY)

ser.reset_input_buffer()
ser.reset_output_buffer()

print("Com port opened : " + ser.name)  

print_header_message ("Connecting to RA Boot Mode")

loopcount = communication_setting()

if loopcount > 0:
    # Initial communication is established
    ser.reset_input_buffer()
    time.sleep(SHORT_DELAY)
    # Reference the section "Communication Setting Phase" in R01AN7140
    print("Sending GENERIC code to target : 0x55 ")
    ser.write(b'\x55')

    print("Checking for the Boot code sent back from target")
    time.sleep(SHORT_DELAY)
    h = ser.read()
    print("Received :" + h.hex())

    match h:
        case b'\xc6':
            print("CM85/CM33 boot code received")
            bootcode = 0xc6
        case b'\xc3':
            print("CM4/CM23 boot code received")
            print("They are not supported by this demonstration")
            terminate_execution()
        case _:
            print("*** ERROR : Unknown code received, closing down")
            terminate_execution()

else:  
    # No ack received, so its possible we have a previous boot mode connection
    # still active, so send a boot mode inquiry command to see.
    print("No ack received") 
    print("Seeing if boot mode already active") 
    command_inquiry()
    dataPacket = receive_data_packet()
    print_bytes_hex(dataPacket)
    if dataPacket != b'\x81\x00\x0a\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\xfe\x03':
        # No previous boot mode connection detected, so give up
        print('*** ERROR : Connection failed, closing down')
        terminate_execution()
    else:
        print('Already active boot mode connection found and will be used')
        # bootcode cannot be read, so setting to 0
        bootcode = 0x0
        # Will assume that CM85 is in use

print_header_message("Requesting Product Type Name")
# Check the device signature to see if it is supported by this demonstration
deviceSignature = command_signature_request()
if deviceSignature != 0x03 and deviceSignature != 0x07:
    print("The device is not supported by this demonstration")
    terminate_execution()

# =============================

print_header_message("Requesting the DLM State")

response, current_dlm_state = command_DLMstateRequest()

if response != 0x2C:
    print("\n*** ERROR : Request current DLM state failed")
else:
    match current_dlm_state:
        case 0x01:
            source_DLM_state = b'\x01'
            target_DLM_state = b'\x04'
            # Transition from CM to OEM
            command_DLMstateTransition(source_DLM_state, target_DLM_state)
        case 0x06:
            print("This demonstration can not be performed in LCK_BOOT state. \n")
        case 0x07:
            print("This demonstration can not be performed in RMA_REQ state. \n")
        case 0x08:
            print("This demonstration can not be performed in RMA_ACK state. \n")
        case 0x09:
            print("This demonstration can not be performed in RMA_RET state. \n")
        case 0x04:
            print_header_message("Requesting the Protection and Authentication Level")
            response_pl, current_pl = command_ProtectionLevelRequest()
            response_al, current_al = command_AuthenticationLevelRequest()
            if current_pl != 0x02:
                print_header_message("Transitioning Protection Level to PL2")
                target_PL_state = b'\x02'
                response = command_ProtectionLevelTransition(target_PL_state)

            print_header_message("Requesting current TrustZone Boundary information")
            response = command_trustzone_boundary_request(deviceSignature)
            if response != 0x4F:
                print("\n*** ERROR : Read TrustZone boundary failed")
                terminate_execution()
            
            print_header_message("Configuring TrustZone Boundaries")
            response = command_setup_trustzone_boundary(deviceSignature)
            if response != 0x4E:
                print("\n*** ERROR : Setting TrustZone boundary failed")
                terminate_execution()
            
            print_header_message("Requesting updated TrustZone Boundary information")
            response = command_trustzone_boundary_request(deviceSignature)
            if response != 0x4F:
                print("\n*** ERROR : Reading TrustZone boundary failed")
                terminate_execution()
            
            print_header_message("Injecting DLM - AL2 Key")
            al2_key_type = b'\x01'
            response = command_inject_dlm_key(al2_key_type, deviceSignature)
            if response != 0x28:
                print("\n*** ERROR : Injecting AL2 key failed")
                terminate_execution()

            print_header_message("Verifying the AL2 Key")
            response = command_verify_dlm_key(al2_key_type)
            if response != 0x29:
                print("\n*** ERROR : Verify AL2 key failed")
                terminate_execution()

            print_header_message("Injecting DLM - AL1 Key")
            al1_key_type = b'\x02'
            response = command_inject_dlm_key(al1_key_type, deviceSignature)
            if response != 0x28:
                print("\n*** ERROR : Injecting AL1 key failed")
                terminate_execution()
            
            print_header_message("Verifying the AL1 Key")
            response = command_verify_dlm_key(al1_key_type)
            if response != 0x29:
                print("\n*** ERROR : Verify AL1 key failed")
                terminate_execution()
            
            print_header_message("Transitioning Protection Level from PL2 to PL1")
            target_PL_state = b'\x03'
            response = command_ProtectionLevelTransition(target_PL_state)
            if response != 0x72:
                print("\n*** ERROR : Transitioning from PL2 to PL1 failed")
                terminate_execution()
            
            print_header_message("Transitioning Protection Level from PL1 to PL0")
            target_PL_state = b'\x04'
            response = command_ProtectionLevelTransition(target_PL_state)
            if response != 0x72:
                print("\n*** ERROR : Transitioning from PL1 to PL0 failed")
                terminate_execution()
            
            if INVOKE_DISABLE_INITIALIZE_COMMAND == 1:
                print_header_message("Disabling ability to invoke Initialize command")
                print("!!! ======================== WARNING ========================= !!!")
                print("!!! Executing 'command_disable_initialize ()' will prevent the !!!")
                print("!!! Initialize command from working in the future.             !!!")
                check = input("\nEnter YES if you are really sure you want to do this: ")
                if check == "YES":
                    print("You entered YES - calling function")
                    command_disable_initialize()
                else:
                    print("You did not enter YES - skipping call to 'command_disable_initialize ()'")

terminate_execution()
