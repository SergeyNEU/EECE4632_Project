#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "ap_fixed.h"
#include "ap_int.h"
#include "ap_shift_reg.h"
#include "ap_utils.h"
#include "ap_common.h"
#include "ap_int.h"
#include "ap_fixed_base.h"
#include "ap_int_base.h"
#include "ap_shift_reg.h"
#include "ap_utils.h"
#include "hls_math.h"
#include "hls_half.h"
#include "hls_fpo.h"
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "ap_fixed.h"
#include <string.h>

#define BLOCK_SIZE 16

typedef struct
{
    uint8_t data[BLOCK_SIZE];
} block_t;

typedef struct
{
    uint8_t data[BLOCK_SIZE];
} key_t;

typedef struct
{
    block_t *plaintext;
    key_t *key;
} input_t;

typedef struct
{
    block_t *ciphertext;
} output_t;

// Encrypts the given plaintext using AES-128 encryption algorithm with the given key.
void encrypt(block_t *plaintext, key_t *key, block_t *ciphertext)
{
    // Create an instance of the AES cipher with the given key
    AES_KEY aes_key;
    AES_set_encrypt_key((const unsigned char *)key->data, 128, &aes_key);

    // Encrypt the plaintext
    AES_encrypt((const unsigned char *)plaintext->data, (unsigned char *)ciphertext->data, &aes_key);
}

// Decrypts the given ciphertext using AES-128 decryption algorithm with the given key.
void decrypt(block_t *ciphertext, key_t *key, block_t *plaintext)
{
    // Create an instance of the AES cipher with the given key
    AES_KEY aes_key;
    AES_set_decrypt_key((const unsigned char *)key->data, 128, &aes_key);

    // Decrypt the ciphertext
    AES_decrypt((const unsigned char *)ciphertext->data, (unsigned char *)plaintext->data, &aes_key);
}

// Pads the given plaintext to the nearest multiple of 16 bytes using the null character.
void pad(block_t *plaintext, uint8_t padding_size)
{
    // Pad the plaintext to the nearest multiple of 16 bytes
    memset(plaintext->data + BLOCK_SIZE - padding_size, padding_size, padding_size);
}

// Removes the padding from the given padded plaintext.
uint8_t unpad(block_t *padded_plaintext)
{
    // Remove the padding from the plaintext
    uint8_t padding_size = padded_plaintext->data[BLOCK_SIZE - 1];
    memset(padded_plaintext->data + BLOCK_SIZE - padding_size, 0, padding_size);
    return padding_size;
}

void software(hls::stream<input_t> &input, hls::stream<output_t> &output, uint32_t num_iterations)
{
	// Read input
	input_t in = input.read();

	// Pad the plaintext
	uint8_t padding_size = BLOCK_SIZE - (strlen((const char *)in.plaintext->data) % BLOCK_SIZE);
	pad(in.plaintext, padding_size);

	// Encrypt the plaintext
	encrypt(in.plaintext, in.key, in.ciphertext);

	// Decrypt the ciphertext
	block_t decrypted_plaintext;
	decrypt(in.ciphertext, in.key, &decrypted_plaintext);

	// Remove the padding from the plaintext
	uint8_t unpadded_size = unpad(&decrypted_plaintext);

	// Write output
	output_t out;
	memcpy(out.ciphertext->data, in.ciphertext->data, BLOCK_SIZE);
	memcpy(out.ciphertext->data + BLOCK_SIZE, &unpadded_size, 1);
	memcpy(out.ciphertext->data + BLOCK_SIZE + 1, decrypted_plaintext.data, unpadded_size);
	output.write(out);
}

//ERROR: [HLS 207-3801] unknown type name 'AES_KEY' (EECE4632_Github_Project/Hardware/vitis.cpp:49:5)
//ERROR: [HLS 207-3801] unknown type name 'AES_KEY' (EECE4632_Github_Project/Hardware/vitis.cpp:60:5)
//ERROR: [HLS 207-2972] no member named 'ciphertext' in 'input_t' (EECE4632_Github_Project/Hardware/vitis.cpp:93:35)
//ERROR: [HLS 207-2972] no member named 'ciphertext' in 'input_t' (EECE4632_Github_Project/Hardware/vitis.cpp:97:13)
//ERROR: [HLS 207-2972] no member named 'ciphertext' in 'input_t' (EECE4632_Github_Project/Hardware/vitis.cpp:104:34)
