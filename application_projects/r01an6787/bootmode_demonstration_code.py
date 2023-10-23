#/***********************************************************************************************************************
# * Copyright [2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
# *
# * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
# * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
# * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
# * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
# * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
# * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
# * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
# * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
# * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
# * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
# * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
# * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
# * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
# * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
# * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
# **********************************************************************************************************************/

#/**********************************************************************************************************************
# * File: bootmode_demonstration_copy.py
# * This file demonstates below functionalities
# *     Read current DLM state
# *     Set up TrustZone boundary
# *     Read TrustZone boundary
# *     Inject DLM SECDBG and NSECDBG keys
# *     Transition the DLM state from SSD to NSECSD
# *     Transition the DLM state from NSECSD to DPL
# ***********************************************************************************************************************/


# both serial and USB COM port can be used to communicate with the Boot firmware
import serial
import time
import serial.tools.list_ports
#DLM .rkey file is base64 encoded
import base64

# Set DEBUG_OUTPUT_ENABLE to 1 to enable more debug output
DEBUG_OUTPUT_ENABLE = 0

# Set INVOKE_DISABLE_INITIALISE_COMMAND to 1 to disable initialise command
# !!!!! WARNING !!!!!
# This is non-reversable, so invoke with caution !!!
INVOKE_DISABLE_INITIALISE_COMMAND = 0

# Adjust these delays for your hardware system
SHORT_DELAY = 0.05
LONG_DELAY = 0.5

#Unknown DLM Key Type
UNKNOWN_KEY = 0xFF

#the key type number for DLM keys is 0x00
#the other user key type number is defined in R01AN5562 section User key list. 
#the meaning of each entry is defined in key_type_string
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
                "Key update key" ]

#########################################################
# Establish communication with the boot firmware
#########################################################

# ==================================
# This routine demonstrates the communication setting phase handling
def communication_setting():
    loopcount = 20

    print ('Sending three 0x00 to target to start Communication Setting Phase')
    while loopcount != 0:
        # Try this a few times
        ser.write(b'\x00')
        # This sleeping time can change based on what source clock is used for 
        # the MCU. Reference the Secttion "Communication setting phase" in 
        # R01AN5562 for more details".
        time.sleep(SHORT_DELAY)
        ser.write(b'\x00')
        time.sleep(SHORT_DELAY)
        ser.write(b'\x00')
        time.sleep(SHORT_DELAY)
        h = ser.read()
        if h == b'\x00':
            print ("Success: ACK received")
            break
        loopcount -= 1
        time.sleep(SHORT_DELAY)
    return loopcount
    
#########################################################
# boot firmware command supporting subroutines
#########################################################

# ==================================
# This routine prints a message wrapped by lines above & below, to
# indicate a section in output being displayed
def print_header_message (message):
    mlen = len (message)
    wrap_string = "=" * mlen
    print ("\n" + wrap_string + "\n" + message + "\n" + wrap_string + "\n")

#===================================
# This routine cleans up the system on exit with error
def terminate_execution():
    print_header_message ("Demonstration finished.")
    ser.close()
    quit()

# ==================================
# This routine print the type of key bssed on the key index. 
def print_key_type_string (number):
    if number in key_type_num:
        elem_num = key_type_num.index(number)
        print ("Key Type = '",key_type_string[elem_num],"'")
    else:
      print ("!*** ERROR : Key type unknown : ", number)
      terminate_execution()

# ==================================
# This routine calculates the check sum. The low byte is the check sum.
def calc_sum (lotsofbytes):
    count = 0
    for index, value in enumerate(lotsofbytes):
        count = count + value

    count = count % 256
    
    summation = (256-count) % 256
    h = summation.to_bytes(1,'big')
    
    return h

# ==================================
# This routine print the array in hex format
def print_bytes_hex (lotsofbytes):
    bstring = r"b'"
    for index, value in enumerate(lotsofbytes):
        bstring = bstring + r"\x" + format(value,'02x')
    bstring = bstring + r"'"
    print (bstring)
 
# ==================================
# This routine reads out the current TrustZone boundary setup.
def print_boundary_region (return_packet):
    
    CFS1_1 = return_packet[4]
    CFS1_2 = return_packet[5]
    h = CFS1_1*256 + CFS1_2
    secure_code_flash_no_nsc = f' - The secure code flash region size without NSC is {h} KB'
    print (secure_code_flash_no_nsc)
    
    CFS2_1 = return_packet[6]
    CFS2_2 = return_packet[7]
    h = CFS2_1*256 + CFS2_2
    secure_code_flash_with_nsc = f' - The secure code flash region size with NSC is {h} KB'
    print (secure_code_flash_with_nsc)
           
    DFS1_1 = return_packet[8]
    DFS1_2 = return_packet[9]
    h = DFS1_1*256 + DFS1_2
    secure_data_flash = f' - The secure data flash region size is {h} KB'
    print (secure_data_flash)
        
    SRS1_1 = return_packet[10]
    SRS1_2 = return_packet[11]
    h = SRS1_1*256 + SRS1_2       
    secure_sram_region_without_nsc = f' - The secure SRAM region size without NSC is {h} KB'
    print (secure_sram_region_without_nsc)
   
    SRS2_1 = return_packet[12]
    SRS2_2 = return_packet[13]
    h = SRS2_1*256 + SRS2_2
    secure_sram_region_with_nsc = f' - The secure SRAM region size with NSC is {h} KB'
    print (secure_sram_region_with_nsc)
    
# ==================================
# This routine receives the entire response data packet.    
def receive_data_packet ():
    packet = bytearray()
    SOD = ser.read()
    
    if SOD != b'\x81':
        print('\n!!! ERROR : Unexpected SOD byte value received in data packet : ', SOD, ' (', int.from_bytes(SOD,"big"),')')
        print('!!!         Try resetting board with MD pulled low, then running again.') 
        terminate_execution()

    packet = packet + SOD
    LNH = ser.read()
    packet = packet + LNH
    LNL = ser.read()
    packet = packet + LNL

    # +2 is so that get SUM and ETX bytes too
    for loop in range(0, ((int.from_bytes(LNH,"big") * 256) + int.from_bytes(LNL,"big") + 2)):
        x = ser.read()
        packet = packet + x
    
    return packet

# ==================================
# This routine parses the DLM key file
# The decode DLM key data buffer and the size of the encrypted key are returned.
def parse_the_dlm_key_field(text_file, dlm_key_type):
    #read entire file to a string
    data = text_file.read()
     
    #close file
    text_file.close()

    if(dlm_key_type == b'\x01'):
        print ("Start Processing SECDBG rkey file") 
    elif (dlm_key_type == b'\x02'):
        print ("Start Processing NSECDBG rkey file") 
        
    if (DEBUG_OUTPUT_ENABLE == 1):
        print(data)

    if not (data.startswith("-----BEGIN RENESAS KEY-----\n")) or not (data.endswith("-----END RENESAS KEY-----\n")):
        print ("!*** ERROR : Incorrect rkey file header and/or footer")
        return 0, 0
    else:
        if (DEBUG_OUTPUT_ENABLE == 1):
            print ("\nCorrect rkey file header and footer found")

        data = data.removeprefix("-----BEGIN RENESAS KEY-----\n")
        data = data.removesuffix("-----END RENESAS KEY-----\n")

        if (DEBUG_OUTPUT_ENABLE == 1):
            print ("Header & Footer removed \n") 
            print(data)

        print ("Base64 Decoding key") 
        message_bytes = base64.b64decode(data)

        if (DEBUG_OUTPUT_ENABLE == 1):
            print ("\nBase64 decoded. Here is the decoded data: \n") 
            print_bytes_hex (message_bytes)

        if (DEBUG_OUTPUT_ENABLE == 1):
            print ("\nrkey file structure \n") 
        
        magic_code = message_bytes [0:4]
        if magic_code == b'REK1' :
            if (DEBUG_OUTPUT_ENABLE == 1):
                print ("Correct Magic Code found : ", magic_code)
        else:
            print ("!!! ERROR : Incorrect Magic Code found : ", magic_code)

        suite_version = int.from_bytes(message_bytes [4:8], byteorder='big', signed = False)
        if suite_version == 1 :
            if (DEBUG_OUTPUT_ENABLE == 1):
                print ("Correct Suite Version found : ", suite_version)
        else:
            print ("!!! ERROR : Incorrect Suite Version : ", suite_version)

        reserved_field7 = int.from_bytes(message_bytes [8:15], byteorder='big', signed = False)
        if reserved_field7 == 0 :
            if (DEBUG_OUTPUT_ENABLE == 1):
                print ("Correct Reserved field[7] found : ", reserved_field7)
        else:
            print ("!!! ERROR : Incorrect Reserved field[7] : ", reserved_field7)

        key_type = int.from_bytes(message_bytes [15:16], byteorder='big', signed = False)
        if (DEBUG_OUTPUT_ENABLE == 1):
            print ("Key Type = ", key_type)
        print_key_type_string (key_type)

        encrypted_key_size = int.from_bytes(message_bytes [16:20], byteorder='big', signed = False)
        if (DEBUG_OUTPUT_ENABLE == 1):
            print ("Encrypted Key Size  = ", encrypted_key_size)
            print ("\nW_UFPK \n") 
        w_ufpk = message_bytes [20:56]
        if (DEBUG_OUTPUT_ENABLE == 1):
            print_bytes_hex (w_ufpk)
            print ('Note that when injecting the key, user needs to remove the first 4 bytes')
            print ("\nInitalize Vector \n") 
        initial_vector = message_bytes [56:72]
        if (DEBUG_OUTPUT_ENABLE == 1):
            print_bytes_hex (initial_vector)
            print ("\nEncrypted Key \n") 
        encrypted_key = message_bytes [72:72+encrypted_key_size]
        if (DEBUG_OUTPUT_ENABLE == 1):
            print_bytes_hex (encrypted_key)
            print ("\nData CRC \n") 
        data_crc = message_bytes [72+encrypted_key_size:72+encrypted_key_size+4]
        if (DEBUG_OUTPUT_ENABLE == 1):
            print_bytes_hex (data_crc)

        if len(message_bytes) == (72+encrypted_key_size+4) :
            if (DEBUG_OUTPUT_ENABLE == 1):
                print ("\nSizes match in processing rkey file \n")
        else : 
            print ("\n!!! ERROR : Size mismatch in processing rkey file")

        print ("Finished : Processing rkey file.")
        return (message_bytes, encrypted_key_size)
        
    
#########################################################
# boot firmware command demonstrations
#########################################################
 
# ==================================
# This routine demonstrates "DLM key injection" command.
# The supported key types are SECDBG and NSECDBG key
def command_inject_dlm_key(dlm_key_type):
    if (dlm_key_type != b'\x01' and dlm_key_type != b'\x02'):
        print('this key type is not supported')
        # return with unknown DLM key type
        return UNKNOWN_KEY
    elif (dlm_key_type == b'\x01'):
        text_file = open("./dlm_keys/SECDBG.rkey", "r")
        
    elif (dlm_key_type == b'\x02'):
        text_file = open("./dlm_keys/NON-SECDBG.rkey", "r")
        
    message_bytes, encrypted_key_size = parse_the_dlm_key_field(text_file, dlm_key_type)
    if(message_bytes != 0 and encrypted_key_size != 0):
        # Proceed with the DLM key injection command packet stage
        SOH=b'\x01' #start of command packet
        LNH=b'\x00'
        LNL=b'\x02'
        CMD=b'\x28' #DLM Key injection command
        KYTY = dlm_key_type
        SUM=calc_sum(LNH + LNL + CMD + KYTY)
        EXT=b'\x03'
        command = SOH + LNH + LNL + CMD + KYTY + SUM + EXT  
        print ("\nSending DLM key injection command packet: ")
        print_bytes_hex (command)
        ser.write(command)
        time.sleep(LONG_DELAY)
        return_packet = receive_data_packet()
        # the RES byte is the fourth index
        RES = return_packet[3]
        # Acquire the response of the command packet stage
        if RES != 0x28:
            print('DLM key injection command packet - FAIL')
        else:
            # the STS is the fifth index
            STS = return_packet[4]
            if STS != 0x00:
                print('DLM key injection command packet - FAIL')
            #the SUM is the 14th index
            SUM = return_packet[13]
            if SUM == 0xD6:
                print('\nDLM key injection command packet - SUCCESS')
            # Start the data packet stage
            SOD=b'\x81' #start of data packet
            LNH=b'\x00'
            LNL=b'\x51' #81 byte: one byte (RES) + w-ufpk (32byte) + initialize vector (16 byte) 
                        #+ encrypted dlm key and mac (32 byte)
            RES=b'\x28' #OK byte
            SUM=calc_sum(LNH+LNL+RES + message_bytes [24:56] + message_bytes [56:72] + \
                        message_bytes [72:72+encrypted_key_size])
            ETX = b'\x03'
            command = SOD + LNH + LNL + RES + message_bytes [24:56]  + message_bytes [56:72] + \
                        message_bytes [72:72+encrypted_key_size] + SUM + ETX
            print ("\nSending DLM key injection data packet: ")
            print_bytes_hex (command)
            ser.write(command)
            time.sleep(LONG_DELAY)
            # Acquire the response of the data packet stage 
            return_packet = receive_data_packet() 
            # the RES byte is the fourth index
            RES = return_packet[3]
            if RES != 0x28:
                print('DLM key injection data packet failed')
            else:
                # the STS is the fifth index
                STS = return_packet[4]
                if STS != 0x00:
                    print('Injecting DLM key failed')
                elif KYTY == b'\x01':
                    print('\nDLM key injection data packet - SUCCESS\n')
                    print("Injecting SECDBG key is successful")
                elif KYTY == b'\x02':
                    print('\nDLM key injection data packet - SUCCESS\n')
                    print('Injecting NSECDBG key is successful')
    return RES

# ==================================
# This routine demonstrates "DLM Key verification command".
# The supported key types are SECDBG and NSECDBG key     
def command_verify_dlm_key(dlm_key_type): 
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x02'
    CMD=b'\x29' #DLM Key verify command
    KYTY = dlm_key_type #DLM Key type 
    
    SUM=calc_sum(LNH+LNL+CMD + KYTY)
    ETX=b'\x03'
    command = SOH + LNH + LNL + CMD + KYTY + SUM + ETX 
    if KYTY == b'\x01':
        print ("Sending DLM SECDBG key verify command:")
    elif KYTY == b'\x02':
        print ("Sending DLM NSECDBG key verify command:")
    else :
        print ("Unsupported DLM Key type")
    print_bytes_hex (command)
    ser.write(command)
    time.sleep(LONG_DELAY)
    #acquire the response
    return_packet = receive_data_packet()
    # the RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x29:
        print(' DLM key verification - FAIL')
    else:
        # the STS is the fifth index
        STS = return_packet[4]
        if STS != 0x00:
            print('Verifying injected DLM key failed')
        elif KYTY == b'\x01':
            print ("Verifying injected SECDBG key is successful")
        elif KYTY == b'\x02':
            print('Verifying injected NSECDBG key is successful')
    return RES
    
# ==================================
# This routine demonstrates "TrustZone boundary request command". 
def command_trustzone_boundary_request():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    CMD=b'\x4F' #Boundary request command
    SUM=b'\xB0'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT  
    
    print ("Sending read boundary region command:")
    print_bytes_hex (command)
    ser.write(command)
    time.sleep(LONG_DELAY)
    #acquire the response
    return_packet = receive_data_packet()
    # the RES byte is the fourth index
    RES = return_packet[3]
    
    if RES != 0x4F:
        print('Read boundary region - FAIL')
    else:
        print ('Read boundary region - SUCCESS')
    print_boundary_region(return_packet)
        
    return RES
      
# ==================================
# This routine demonstrates "TrustZone boundary setting command". 
def command_setup_trustzone_boundary():
    print ("Configuring device with :")
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x0B'
    CMD=b'\x4E' #Boundary setting command
    print(' - 8kB of secure code flash region without NSC')
    CFS1_1=b'\x00'
    CFS1_2=b'\x08'  #8KB
    print(' - 32kB of secure code flash region with NSC')
    CFS2_1=b'\x00'
    CFS2_2=b'\x20'  #32KB
    print(' - 4kB of secure data flash region')
    DFS1_1=b'\x00'
    DFS1_2=b'\x04'  #4KB
    print(' - 2kB of secure sram region without NSC')
    SRS1_1=b'\x00'
    SRS1_2=b'\x02'  #2KB
    print(' - 32kB of secure sram region with NSC')
    SRS2_1=b'\x00'
    SRS2_2=b'\x20'  #32KB #Size of SRAM Secure region (without NSC) [KB]
    
    SUM=calc_sum(LNH + LNL + CMD + CFS1_1 + CFS1_2 + CFS2_1 + CFS2_2 + \
        DFS1_1 + DFS1_2 + SRS1_1 + SRS1_2 + SRS2_1 + SRS2_2)
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + CFS1_1 + CFS1_2 + CFS2_1 + CFS2_2 + \
        DFS1_1 + DFS1_2 + SRS1_1 + SRS1_2 + SRS2_1 + SRS2_2 + SUM + EXT
    print ("Sending TrustZone boundary setup command:")
    print_bytes_hex (command)
    
    ser.write(command)
    time.sleep(LONG_DELAY)

    #acquire the response
    return_packet = receive_data_packet()
    # the RES byte is the fourth index
    RES = return_packet[3]
    
    if RES != 0x4E:
        print('\n*** ERROR : Set up boundary region - FAIL')
    else:
        print ('\nSet up boundary region - SUCCESS') 
    
    return RES
        
# ==================================
# This routine demonstrates the "Inquiry command". 
# This command checks if boot firmware is in ‘Command acceptable phase’ or not.

def command_inquiry ():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    CMD=b'\x00'
    SUM=b'\xFF'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT

    print ("Sending Inquiry command:")
    print_bytes_hex (command)

    ser.write(command)
    time.sleep(LONG_DELAY)    
    
# ==================================
# This routine demonstrates the "DLM state request command". 
# The command execution result and the DLM state are returned.
def command_DLMstateRequest():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    CMD=b'\x2c'         # DLM state request command code
    SUM=b'\xd3'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT   

    print ("Sending DLM State Request command:")
    print_bytes_hex (command)

    ser.write(command)
    time.sleep(SHORT_DELAY)
    #acquire the response
    return_packet = receive_data_packet()
    # the RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x2C:
        print('*** ERROR : Read DLM state - FAIL')
        DLM = b'\xFF'
    else:    
        # the DLM byte is the fifth index
        DLM = return_packet[4]
        match DLM:
            case 0x01:
                print('Current DLM state is Chip Manufacturing,') 
                print('Use the DLM state transition command to transition to SSD state!')
            case 0x02:
                print('Current DLM state is SSD.')
            case 0x03:
                print('Current DLM state is NSECSD')
            case 0x04:
                print('Current DLM state is DPL') 
            case 0x05:
                print('Current DLM state is LCK_DBG')
            case 0x06:
                print('Current DLM state is LCK_BOOT')
            case 0x07:
                print('Current DLM state is RMA_REQ')
            case 0x08:
                print('Current DLM state is RMA_ACK')
            case _:
                print('Unknown DLM state')
    return RES, DLM

# ==================================
# This routine demonstrates the "DLM state transition command". 
def command_DLMstateTransition(source_DLM_state, target_DLM_state):
    
    RES, SDLM = command_DLMstateRequest()
    if RES != 0x2C:
        print(' Read DLM state - FAIL')
    elif SDLM != int.from_bytes(source_DLM_state,"big"):
        print(' adjust the source DLM state! ')
    else:   
        if target_DLM_state != source_DLM_state and source_DLM_state != b'\x05' \
            and source_DLM_state != b'\x06' and source_DLM_state != b'\x07' \
            and source_DLM_state != b'\x08': 
            SOH=b'\x01'
            LNH=b'\x00'
            LNL=b'\x03'
            #0x71: DLM State Transition Command
            CMD=b'\x71' 
            #SDLM = source_DLM_state
            DDLM=target_DLM_state
            SUM=calc_sum(LNH + LNL + CMD + source_DLM_state + DDLM)
            EXT=b'\x03'
            command = SOH + LNH + LNL + CMD + source_DLM_state + DDLM + SUM + EXT
            print ("\nSending DLM State Transition command:")
            print_bytes_hex (command)
            ser.write(command)
            time.sleep(LONG_DELAY)
            #acquire the response
            return_packet = receive_data_packet()
            # the RES byte is the fourth index
            RES = return_packet[3]
            if RES != 0x71:
                print('DLM Transition command - FAIL')
            else:
                if DDLM == b'\x02':
                    print('DLM state changed to SSD')
                elif DDLM == b'\x03':
                    print('DLM state changed to NSECSD')
                elif DDLM == b'\x04':
                    print('DLM state changed to DPL')
                # the STS is the fifth index
                STS = return_packet[4]
                if STS != 0x00:
                    print('*** ERROR : DLM state Transition command - FAIL')
    return RES  

# ==================================
# The "Initialize command" can be disabled using the 
# "Parameter setting command". 
#  !!!!! WARNING !!!!!
#  This step is non-reversable, so invoke with caution!
def command_disable_initialize ():
    
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x03'
    #0x51: Parameter setting command
    CMD=b'\x51'
    # Parameter ID
    PMID=b'\x01'
    # Parameter Data 0x00 is for disable Initialze
    PRMT=b'\x00'
    SUM=calc_sum(LNH + LNL + CMD + PMID + PRMT)
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + PMID + PRMT + SUM + EXT

    print ("\nSending Disable Initialize command:")
    print_bytes_hex (command)
    print ("\nWarning : After MCU Initialize is disabled, it can never be re-enabled. ")
    
    ser.write(command)
    time.sleep(LONG_DELAY)

    #acquire the response
    return_packet = receive_data_packet()
    # the RES byte is the fourth index
    RES = return_packet[3]
       
    if RES != 0x51:
        print('\nDisable Initialize command - FAIL')
    else:
        print ('Disable Initialize command - SUCCESS')
    
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
    # Initial communcation is established
    ser.reset_input_buffer()
    time.sleep(SHORT_DELAY)
    # Reference the section "Communication setting phase" in R01AN5562
    print ('Sending GENERIC code to target : 0x55 ')
    ser.write(b'\x55')
    print ('Checking for the Boot code sent back from target')
    time.sleep(SHORT_DELAY)
    h = ser.read()
    print ("Received :" + h.hex())
   
    match h:
        case b'\xc6':
            print ("CM33 boot code received")
            bootcode = 0xc6
        case b'\xc3':
            print ("CM4/CM23 boot code received")
            print ("They are not supported by this demonstration")
            terminate_execution()
        case _:
            print ("*** ERROR : Unknown code received, closing down")
            terminate_execution()
else:  
    # No ack received, so its possible we have a previous boot mode connection
    # still active, so send a boot mode inquiry command to see
    print ('No ack received') 
    print ('Seeing if boot mode already active') 
    command_inquiry()
    dataPacket = receive_data_packet()
    print_bytes_hex (dataPacket)
    if dataPacket != b'\x81\x00\x0a\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\xfe\x03':
        # No previous boot mode connection detected, so give up
        print ('*** ERROR : Connection failed, closing down')
        terminate_execution()
    else:
        print ('Already active boot mode connection found and will be used')
        #bootcode cannot be read, so setting to 0
        bootcode = 0x0
        # Will assume that CM33 is in use

print_header_message ("Requesting the DLM State")

response, current_dlm_state = command_DLMstateRequest()

if response != 0x2C:
    print("\n*** ERROR : Request current DLM state failed")
else:
    match current_dlm_state:
        case 0x01:
            source_DLM_state = b'\x01'
            target_DLM_state = b'\x02'
            #Transition from CM to SSD
            command_DLMstateTransition(source_DLM_state, target_DLM_state)
        case 0x03:
            print ("Run initialize_mcu_first.py to try to transition to SSD first!")
        case 0x04:
            print ("Run initialize_mcu_first.py to try to transition to SSD first!")
        case 0x05:
            print ("This demonstration can not be performed in LCK_DEBUG state. \n")
        case 0x06:
            print ("This demonstration can not be performed in LCK_BOOT state. \n")
        case 0x07:
            print ("This demonstration can not be performed in RMA_REQ state. \n")
        case 0x08:
            print ("This demonstration can not be performed in RMA_ACK state. \n")
        case _:
            print_header_message ("Requesting current TrustZone Boundary information")
            response = command_trustzone_boundary_request()
            if response != 0x4F:
                print("\n*** ERROR : Read TrustZone boundary failed")
                terminate_execution()
            print_header_message ("Configuring TrustZone Boundaries")
            response = command_setup_trustzone_boundary()
            if response != 0x4E:
                print("\n*** ERROR : Setting TrustZone boundary failed")
                terminate_execution()

            print_header_message ("Requesting updated TrustZone Boundary information")

            response = command_trustzone_boundary_request()
            if response != 0x4F:
                print("\n*** ERROR : Reading TrustZone boundary failed")
                terminate_execution()
            print_header_message ("Injecting DLM - SECDBG Key")

            secdbg_key_type = b'\x01'
            response = command_inject_dlm_key(secdbg_key_type)
            if response != 0x28:
                print("\n*** ERROR : Injecting SECDBG key failed")
                terminate_execution()

            print_header_message ("Verifying the SECDBG Key")

            response = command_verify_dlm_key(secdbg_key_type)
            if response != 0x29:
                print("\n*** ERROR : Verify SECDBG key failed")
                terminate_execution()

            print_header_message ("Injecting DLM - NSECDBG Key")

            nsecdbg_key_type = b'\x02'
            response = command_inject_dlm_key(nsecdbg_key_type)
            if response != 0x28:
                print("\n*** ERROR : Injecting NSECDBG key failed")
                terminate_execution()
            print_header_message ("Verifying the NSECDBG Key")

            response = command_verify_dlm_key(nsecdbg_key_type)
            if response != 0x29:
                print("\n*** ERROR : Verify NSECDBG key failed")
                terminate_execution()

            print_header_message ("Transitioning DLM state from SSD to NSECSD")

            source_DLM_state = b'\x02'
            target_DLM_state = b'\x03'
            response = command_DLMstateTransition(source_DLM_state, target_DLM_state)
            if response != 0x71:
                print("\n*** ERROR : Transitioning from SSD to NSECSD state failed")
                terminate_execution()
            
            print_header_message ("Transitioning DLM state from NSECSD to DPL")
            
            source_DLM_state = b'\x03'
            target_DLM_state = b'\x04'
            response = command_DLMstateTransition(source_DLM_state, target_DLM_state)
            if response != 0x71:
                print ("\n*** ERROR : Transitioning from NSECSD to DPL state failed.\n")
                terminate_execution()

            if (INVOKE_DISABLE_INITIALISE_COMMAND == 1):
                print_header_message ("Disabling ability to invoke Initialize command")
                print ("!!! ======================== WARNING ========================= !!!")
                print ("!!! Executing 'command_disable_initialize ()' will prevent the !!!")
                print ("!!! Initialize command from working in the future.             !!!")
                check = input ("\nEnter YES if you are really sure you want to do this: ")
                if check == "YES":
                    print ("You entered YES - calling function")
                    command_disable_initialize ()
                else:
                    print ("You did not enter YES - skipping call to 'command_disable_initialize ()'")

terminate_execution()


