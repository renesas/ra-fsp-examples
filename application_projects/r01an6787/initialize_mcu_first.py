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
# * File: initialize_mcu_first.py
# * This file demonstates below functionalities
# *     Read current DLM state
# *     Check whether the Initialize command is disabled or enabled
# *     Disable the Initialize command (by default, this function is disabled)
# *     Initialze the MCU
# ***********************************************************************************************************************/

#both serial and USB COM port can be used to communicate with the Boot firmware
import serial
import time
import serial.tools.list_ports

# Adjust these delays for your hardware system
SHORT_DELAY = 0.05
LONG_DELAY = 0.5
INITIALIZE_DELAY = 1.5

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
        # This sleeping time can change based on what source clock is used for the MCU. 
        # Reference the Secttion "Command setting phase" in R01AN5562 for details".
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

# ==================================
# This routine calculate the check sum. The low byte is the check sum.
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
# This routine receives the entire response data packet 
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
    
#===================================
# This routine cleans up the system on exit with error
def terminate_execution():
    print_header_message ("Demonstration Finished.")
    ser.close()
    quit()
    
#########################################################
# boot firmware command demonstrations
#########################################################

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
# Reference section "Parameter request command" 
# for the definition of the parameters"
def command_check_whether_initialize_is_disabled ():
    
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x02'
    #0x51: Parameter request command
    CMD=b'\x52'
    # Parameter ID: enable/disable of the Initialization
    PMID=b'\x01'
    SUM=calc_sum(LNH + LNL + CMD + PMID)
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + PMID + SUM + EXT

    print ("Sending MCU check whether Initialize command is disabled command:")
    print_bytes_hex (command)
    
    ser.write(command)
    time.sleep(LONG_DELAY)

    return_packet = receive_data_packet()
    #RES is the fourth byte
    RES = return_packet[3]
    if (RES == 0x52):
        #whether Initialize command is disabled is indicated by the fifth byte
        PRMT = return_packet[4]
        if PRMT == 0x00:
            print('Initialization is disabled')
            print ("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
            print ("Warning : If the Initialization is disabled and the DLM state")
            print ("          is not SSD, then the bootmode_demonstration_code.py")
            print ("          can not run succesfully as the TrustZone boundary")
            print ("          can only be set up in SSD state.")
            print ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        elif PRMT == 0x07:
            print ('Initialization is enabled') 
    else:
        print ('Check Whether Initialize Command is disabled or not command failed')
    
    return PRMT
    
# ==================================
# This routine demonstrates the "DLM state request command". 
# The command execution result and the DLM state are returned.
def command_DLMstateRequest():
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x01'
    CMD=b'\x2c'
    SUM=b'\xd3'
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SUM + EXT   

    print ("Sending DLM State Request command:")
    print_bytes_hex (command)

    ser.write(command)
    time.sleep(LONG_DELAY)
    #acquire the response
    return_packet = receive_data_packet()
    # the RES byte is the fourth index
    RES = return_packet[3]
    if RES != 0x2C:
        print('Read DLM state - FAIL')
        DLM = b'\xFF'
    else:    
        # the DLM byte is the fifth index
        DLM = return_packet[4]
        if DLM == 0x01:
            print('Current DLM state is Chip Manufacturing,') 
            print('User must use the DLM state transition command to transition to SSD state')
        elif DLM == 0x02:
            print('Current DLM state is SSD.')
        elif DLM == 0x03:
            print('Current DLM state is NSECSD.')
        elif DLM == 0x04:
            print('Current DLM state is DPL') 
        elif DLM == 0x05:
            print('Current DLM state is LCK_DBG')
        elif DLM == 0x06:
            print('Current DLM state is LCK_BOOT')
        elif DLM == 0x07:
            print('Current DLM state is RMA_REQ')
        elif DLM == 0x08:
            print('Current DLM state is RMA_ACK')
    return RES, DLM
    
# ==================================
# This routine demonstrates the "DLM state transition command". 
def command_DLMstateTransition(source_DLM_state, target_DLM_state):
    
    RES, SDLM = command_DLMstateRequest()
    if RES != 0x2C:
        print(' Read DLM state - FAIL')
    elif SDLM != int.from_bytes(source_DLM_state,"big"):
        print(' Adjust the source DLM state! ')
    else:   
    
        if target_DLM_state != source_DLM_state and source_DLM_state != b'\x05' \
            and source_DLM_state != b'\x06' and source_DLM_state != b'\x07' and \
            source_DLM_state != b'\x08': 
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
                    print('DLM state changed to SSD.')
                elif DDLM == b'\x03':
                    print('DLM state changed to NSECSD.')
                elif DDLM == b'\x04':
                    print('DLM state changed to DPL.')
                # the STS is the fifth index
                STS = return_packet[4]
                if STS != 0x00:
                    print('DLM state Transition command - FAIL')
                
    return RES  


# ==================================
# The "Initialize command" clears User area, Data area, Config area, 
# Boundary setting, and Key index (Wrapped key). In addition, 
# the DLM state transitions to SSD from the NSECSD or DPL.

def command_initialize (current_DLM_state):
    
    SOH=b'\x01'
    LNH=b'\x00'
    LNL=b'\x03'
    #0x50: Initialie command
    CMD=b'\x50'
    # Source DLM state code
    SDLM=current_DLM_state
    # Destination DLM state code
    DDLM=b'\x02'
    SUM=calc_sum(LNH + LNL + CMD + SDLM + DDLM)
    EXT=b'\x03'
    command = SOH + LNH + LNL + CMD + SDLM + DDLM + SUM + EXT

    print ("Sending MCU Initialize command:")
    print_bytes_hex (command)
        
    ser.write(command)
    time.sleep(INITIALIZE_DELAY)

    #acquire the response
    return_packet = receive_data_packet()
    # the RES byte is the fourth index
    RES = return_packet[3]
        
    if RES != 0x50:
        print('Initialize - FAIL')
    else:
        print ('Initialize - SUCCESS')
    
    print ("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    print ("Warning : After MCU Initialize is run, an MCU reset is")
    print ("          required before further boot mode operations.")
    print ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
 

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

# =============================

print_header_message ("Requesting the DLM State")

# If the MCU is not in SSD, either initialize the MCU or transition CM to SSD

response, dlm_state = command_DLMstateRequest()

if response == 0x2C and dlm_state == 0x01:
    print_header_message ("Transitioning DLM state from CM to SSD")
    source_DLM_state = b'\x01'
    target_DLM_state = b'\x02'
    response = command_DLMstateTransition(source_DLM_state, target_DLM_state)
    if response == 0x71:
        # Initialize from source state SSD
        mcommand_initialize(b'\x02')
elif response == 0x2C:
    if dlm_state == 0x02 or dlm_state == 0x03 or dlm_state == 0x04:
        print_header_message ("Checking whether Initialize command is disabled")
        DISABLE = command_check_whether_initialize_is_disabled()
        if DISABLE == 0x07:
            print_header_message ("Initializing the MCU")
            bstring = dlm_state.to_bytes(1, 'big')
            # update the current state definition in your operations.
            command_initialize(bstring)
    else:
        print_header_message ("Initialize command is not available in this DLM state")

        
terminate_execution()


