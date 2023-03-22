from pynq import Overlay, MMIO
import os
import random
import struct

# Load the bitstream and HWH files
overlay = Overlay('/path/to/aes_overlay.bit')

# Define MMIO addresses and size
MMIO_BASE_ADDR = overlay.ip_dict['aes_encrypt_decrypt_0']['phys_addr']
MMIO_SIZE = overlay.ip_dict['aes_encrypt_decrypt_0']['addr_range']

# Initialize MMIO instance
mmio = MMIO(MMIO_BASE_ADDR, MMIO_SIZE)

# Function to generate a random 128-bit key
def generate_key():
    return [random.randint(0, 255) for _ in range(16)]

# Function to send data to FPGA
def send_data(data, address):
    for i in range(len(data)):
        mmio.write(address + i, data[i])

# Function to read data from FPGA
def read_data(address, length):
    return [mmio.read(address + i) for i in range(length)]

# Function to encrypt and decrypt the input string
def encrypt_decrypt(input_str):
    # Pad input_str to a multiple of 16 bytes
    padded_input = input_str.encode('utf-8')
    padding_size = 16 - len(padded_input) % 16
    padded_input += bytes([0] * padding_size)

    # Generate a random 128-bit key
    key = generate_key()

    # Send data to FPGA
    send_data(key, 0x10)
    send_data(padded_input, 0x20)

    # Start the encrypt and decrypt operation
    mmio.write(0x00, 0x01)

    # Wait for the operation to complete
    while mmio.read(0x00) & 0x02 == 0:
        pass

    # Read the encrypted and decrypted data from FPGA
    encrypted_data = read_data(0x30, 16)
    decrypted_data = read_data(0x40, len(padded_input))

    # Convert the data back to strings
    encrypted_str = ''.join([chr(c) for c in encrypted_data])
    decrypted_str = ''.join([chr(c) for c in decrypted_data if c != 0])

    return encrypted_str, decrypted_str

# Get user input
input_str = input("Enter a string to encrypt and decrypt: ")

# Encrypt and decrypt the input string
encrypted_str, decrypted_str = encrypt_decrypt(input_str)

# Print results
print(f"Input string: {input_str}")
print(f"Encrypted string: {encrypted_str}")
print(f"Decrypted string: {decrypted_str}")
