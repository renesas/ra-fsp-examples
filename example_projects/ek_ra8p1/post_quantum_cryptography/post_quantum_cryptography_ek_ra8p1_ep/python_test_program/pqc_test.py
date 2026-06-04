from quantcrypt.kem     import MLKEM_512
from quantcrypt.dss     import MLDSA_44
import binascii
import serial
import sys
import argparse
from time import sleep
from rich import print

comport = ""
ack     = 1
nak     = 0

def main(argv):
    global comport
    global ack
    global nak

    parser = argparse.ArgumentParser(description="PQC Test Tool.")
    parser.add_argument('-c', '--comport', type=str, help='COM port for the serial connection to the MCU')
    args = parser.parse_args()

    if (args.comport):
        comport = args.comport
    else:
        print("COM port must be specified on the command line. Use -c COMx")
        sys.exit()

    # Attempt to open the COM port
    try:
        ser = serial.Serial()
        ser.port = comport
        ser.timeout = 5
        ser.write_timeout = 5
        ser.baudrate = 115200
        ser.open()
    except:
        print("ERROR: Failed to open " + comport)
        sys.exit()

    print("{} open".format(comport))
    
    # First, we create an instance of a ML-KEM algorithm.
    kem = MLKEM_512()

    # Send ACK to the MCU to start the scenario
    # 1
    print("Sending ACK to MCU...")
    packet = bytearray()
    packet.append(ack)
    ser.write(packet)

    # Receive the MCU public key length
    # 2
    print("Waiting for public key length from MCU...")
    ser_data = ser.read(2)

    mcu_pub_key_len = ser_data[0] * 256
    mcu_pub_key_len += ser_data[1]

    print("Remote public length received: {} bytes".format(mcu_pub_key_len))

    # Send ACK to the MCU
    # 3
    print("Sending ACK to MCU...")
    packet = bytearray()
    packet.append(ack)
    ser.write(packet)

    # Receive the MCU public key
    # 4
    print("Waiting for MCU public key...")
    mcu_public_key = ser.read(mcu_pub_key_len)
    print("Received {} bytes".format(len(mcu_public_key)))

    sleep(0.25)

    # Send ACK to the MCU
    # 5
    print("Sending ACK to MCU...")
    packet = bytearray()
    packet.append(ack)
    ser.write(packet)

    # Then, we use the Public Key to encapsulate the internally 
    # generated shared_secret bytes into cipher_text bytes.
    cipher_text, shared_secret = kem.encaps(mcu_public_key)

    # Send the ciphertext size
    # 6
    print("Sending size of ciphertext to the MCU...")
    packet = bytearray()
    packet.append(len(cipher_text) >> 8)
    packet.append(len(cipher_text) & 0xFF)
    ser.write(packet)
    
    print("Ciphertext length: {} bytes".format(len(cipher_text)))

    # Receive ACK/NAK from MCU
    # 7
    print("Waiting for ACK from the MCU")
    ack_nak = ser.read(1)

    if b'\x01' == ack_nak:
        print("Received ACK")
    else:
        print("ERROR: received NAK({}). Quitting".format(ack_nak))
        ser.close()
        sys.exit()
    
    # Send the ciphertext to the MCU
    print("Sending ciphertext to the MCU...")
    # 8
    ser.write(cipher_text)

    # Receive ACK/NAK from MCU
    # 9
    print("Waiting for ACK from the MCU")
    ack_nak = ser.read(1)

    if b'\x01' == ack_nak:
        print("Received ACK")
    else:
        print("ERROR: received NAK({}). Quitting".format(ack_nak))
        ser.close()
        sys.exit()

    # Finally, the secret_key is used to decapsulate the
    # original shared_secret from the cipher_text bytes.

    # Receive the shared secret length
    # 10
    print("Waiting for shared secret length from MCU...")
    ser_data = ser.read(2)

    mcu_shared_secret_len = ser_data[0] * 256
    mcu_shared_secret_len += ser_data[1]

    # Send ACK to the MCU
    # 11
    print("Sending ACK to MCU...")
    packet = bytearray()
    packet.append(ack)
    ser.write(packet)

    print("Waiting for shared secret from MCU (to compare)...")
    mcu_shared_secret = ser.read(mcu_shared_secret_len)
    if len(mcu_shared_secret) == mcu_shared_secret_len:
        print("Received {} bytes from the MCU".format(mcu_shared_secret_len))
    else:
        print("ERROR: Didn't receive MCU shared secret")

    print("Length of shared secret: {} bytes".format(mcu_shared_secret_len))
    print("Calculated (Python) shared secret:")
    print(binascii.hexlify(shared_secret))

    print("Received (MCU) shared secret:")
    print(binascii.hexlify(mcu_shared_secret))

    # Check that the shared secrets match
    if shared_secret == mcu_shared_secret:
        # Send ACK to the MCU
        packet = bytearray()
        packet.append(ack)
        ser.write(packet)

        print("[bold magenta]MATCH![/bold magenta]")
    else:
        # Send NAK to the MCU
        packet = bytearray()
        packet.append(nak)
        ser.write(packet)

        print("[bold red]ERROR: Shared secrets DO NOT MATCH![/bold red]")

    # Demonstrate the ML-DSA algorithm by verifying the signed shared secrets

    # Send ACK to the MCU
    # 12
    print("Sending ACK to MCU...")
    packet = bytearray()
    packet.append(ack)
    ser.write(packet)
    
    # Receive the MCU public key length
    # 13
    print("Waiting for public key length from MCU...")
    ser_data = ser.read(2)

    mcu_pub_key_len = ser_data[0] * 256
    mcu_pub_key_len += ser_data[1]

    print("Remote public key length received: {} bytes".format(mcu_pub_key_len))

    # Send ACK to the MCU
    # 14
    print("Sending ACK to MCU...")
    packet = bytearray()
    packet.append(ack)
    ser.write(packet)

    # Receive the MCU public key
    # 15
    print("Waiting for MCU public key...")
    mcu_public_key = ser.read(mcu_pub_key_len)
    print("Received {} bytes".format(len(mcu_public_key)))

    sleep(0.25)

    # Send ACK to the MCU
    # 16
    print("Sending ACK to MCU...")
    packet = bytearray()
    packet.append(ack)
    ser.write(packet)

    # Receive the MCU public key length
    # 17
    print("Waiting for signature length from MCU...")
    ser_data = ser.read(2)

    signature_len = ser_data[0] * 256
    signature_len += ser_data[1]

    print("Remote signature length received: {} bytes".format(signature_len))

    # Send ACK to the MCU
    # 18
    print("Sending ACK to MCU...")
    packet = bytearray()
    packet.append(ack)
    ser.write(packet)

    # Receive the signature from the MCU
    # 19
    signature = ser.read(signature_len)

    print("Signature received")

    # Send ACK to the MCU
    # 20
    print("Sending ACK to MCU...")
    packet = bytearray()
    packet.append(ack)
    ser.write(packet)

    # Verify the message
    dss = MLDSA_44()
    is_valid = dss.verify(mcu_public_key, shared_secret, signature, raises=False)
    if True == is_valid:
        # Send ACK to the MCU
        packet = bytearray()
        packet.append(ack)
        ser.write(packet)

        print("[bold magenta]SUCCESS! Signature verified.[/bold magenta]")
    else:
        # Send NAK to the MCU
        packet = bytearray()
        packet.append(nak)
        ser.write(packet)

        print("[bold red]FAIL! Signature failed verification.[/bold red]")

if __name__ == "__main__":
    main(sys.argv[1:])