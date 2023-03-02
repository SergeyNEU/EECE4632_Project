# Sergey Petrushkevich
# Running using Python 2.7, run using the command in terminal "python2.7 software.py"
# Crypto library: "pip install pycrypto"

from Crypto.Cipher import AES
import time
import os

# Define the encryption and decryption functions
def encrypt(key, plaintext):
    
    # Encrypts the given plaintext using AES-128 encryption algorithm with the given key.
    ## Args:
    # key: The encryption key to use.
    # plaintext: The plaintext to encrypt.
    ## Returns:
    # The encrypted ciphertext.
    
    # Create an instance of the AES cipher with the given key
    cipher = AES.new(key, AES.MODE_ECB)

    # Pad the plaintext to the nearest multiple of 16 bytes
    padded_plaintext = pad(plaintext)

    # Encrypt the plaintext
    ciphertext = cipher.encrypt(padded_plaintext)

    # Return the ciphertext
    return ciphertext

def decrypt(key, ciphertext):

    # Decrypts the given ciphertext using AES-128 decryption algorithm with the given key.
    ## Args:
    # key: The decryption key to use.
    # ciphertext: The ciphertext to decrypt.
    ## Returns:
    # The decrypted plaintext.
    
    # Create an instance of the AES cipher with the given key
    cipher = AES.new(key, AES.MODE_ECB)

    # Decrypt the ciphertext
    padded_plaintext = cipher.decrypt(ciphertext)

    # Remove the padding from the plaintext
    plaintext = unpad(padded_plaintext)

    # Return the plaintext
    return plaintext

# Define padding and unpadding functions
def pad(plaintext):

    # Pads the given plaintext to the nearest multiple of 16 bytes using the null character.

    ## Args:
    # plaintext: The plaintext to pad.

    ## Returns:
    # The padded plaintext.

    # Pad the plaintext to the nearest multiple of 16 bytes
    return plaintext + b"\0" * (16 - len(plaintext) % 16)

def unpad(padded_plaintext):
    
    # Removes the padding from the given padded plaintext.
    ## Args:
    # padded_plaintext: The padded plaintext to unpad.
    ## Returns:
    # The unpadded plaintext.
    
    # Remove the padding from the plaintext
    return padded_plaintext.rstrip(b"\0")

# Define number of iterations
num_iterations = 100000

# Lets generate random plaintext and keys
plaintext_list = [os.urandom(16) for i in range(num_iterations)]
key_list = [os.urandom(16) for i in range(num_iterations)]

# Time the implementation to measure the time it takes to encrypt and decrypt the test data
start_time = time.time()

for i in range(num_iterations):
    ciphertext = encrypt(key_list[i], plaintext_list[i])
    decrypted_plaintext = decrypt(key_list[i], ciphertext)

end_time = time.time()
total_time = (end_time - start_time) * 1000000 # convert to microseconds
avg_time = total_time / num_iterations

# Format the time
formatted_time = '{:.2f}'.format(avg_time)

# Output the results
print("AES-128 Encryption and Decryption")
print("Number of iterations: ", num_iterations)
print("Average time taken: ", formatted_time, " microseconds")
