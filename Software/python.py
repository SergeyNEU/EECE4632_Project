# Sergey Petrushkevich
# Running using Python 2.7, run using the command in terminal "python2.7 software.py"
# Crypto library: "pip install pycrypto"

from Crypto.Cipher import AES
import time
import os

# Encrypts the given plaintext using AES-128 encryption algorithm with the given key.
def encrypt(key, plaintext):
    # Create an instance of the AES cipher with the given key
    cipher = AES.new(key, AES.MODE_ECB)

    # # Pad the plaintext to the nearest multiple of 16 bytes
    # padded_plaintext = pad(plaintext)

    # Encrypt the plaintext
    ciphertext = cipher.encrypt(plaintext)

    # Return the ciphertext
    return ciphertext

# Decrypts the given ciphertext using AES-128 decryption algorithm with the given key.
def decrypt(key, ciphertext):
    # Create an instance of the AES cipher with the given key
    cipher = AES.new(key, AES.MODE_ECB)

    # Decrypt the ciphertext
    padded_plaintext = cipher.decrypt(ciphertext)

    # Remove the padding from the plaintext
    plaintext = unpad(padded_plaintext)

    # Return the plaintext
    return plaintext

 # Pads the given plaintext to the nearest multiple of 16 bytes using the null character.
def pad(plaintext):
    # Pad the plaintext to the nearest multiple of 16 bytes
    return plaintext + b"\0" * (16 - len(plaintext) % 16)

# Removes the padding from the given padded plaintext.
def unpad(padded_plaintext):
    # Remove the padding from the plaintext
    return padded_plaintext.rstrip(b"\0")

# Define number of iterations
num_iterations = 100000

# Hardcode the plaintext and key from the Vitis testbench
plaintext = bytearray([0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF])
key = bytearray([0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF])

# Time the implementation to measure the time it takes to encrypt and decrypt the test data
start_time = time.time()

for i in range(num_iterations):
    ciphertext = encrypt(key, plaintext)
    decrypted_plaintext = decrypt(key, ciphertext)


# Print the ciphertext and decrypted_plaintext as strings of hexadecimal values
print(ciphertext.encode('hex'))
print(decrypted_plaintext.encode('hex'))

end_time = time.time()
total_time = (end_time - start_time) * 1000000 # convert to microseconds
avg_time = total_time / num_iterations

# Format the time
formatted_time = '{:.2f}'.format(avg_time)

# Output the results
print("AES-128 Encryption and Decryption")
print("Number of iterations: ", num_iterations)
print("Average time taken: ", formatted_time, " microseconds")