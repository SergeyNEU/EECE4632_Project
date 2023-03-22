#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "ap_fixed.h"
#include "ap_int.h"
#include "ap_cint.h"
#include "ap_shift_reg.h"
#include "ap_utils.h"
#include "ap_common.h"
#include "ap_int.h"
#include "ap_fixed_base.h"
#include "ap_int_base.h"
#include "ap_cint.h"
#include "ap_complex.h"
#include "ap_shift_reg.h"
#include "ap_utils.h"
#include "hls_math.h"
#include "hls_half.h"
#include "hls_fpo.h"
#include "hls_video.h"
#include "hls_image.h"
#include "hls_histogram.h"
#include "hls_bayer.h"
#include "ap_resource_dflt.h"

#include "ap_resource.h"
#include "ap_resource_extended.h"
#include "ap_resource_lut.h"
#include "ap_resource_dsp.h"
#include "ap_hls_stream.h"
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "hls_video.h"
#include "ap_fixed.h"
#include <string.h>

#define BLOCK_SIZE 16

typedef ap_uint<8> uint8_t;
typedef ap_uint<128> uint128_t;

typedef struct
{
    uint8_t data[16];
} block_t;

typedef struct
{
    uint8_t data[16];
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

void encrypt(block_t *plaintext, key_t *key, block_t *ciphertext)
{

    // Encrypts the given plaintext using AES-128 encryption algorithm with the given key.
    // Args:
    // key: The encryption key to use.
    // plaintext: The plaintext to encrypt.
    // Returns:
    // The encrypted ciphertext.

    // Create an instance of the AES cipher with the given key
    AES_KEY aes_key;
    AES_set_encrypt_key((const unsigned char *)key->data, 128, &aes_key);

    // Encrypt the plaintext
    AES_encrypt((const unsigned char *)plaintext->data, (unsigned char *)ciphertext->data, &aes_key);
}

void decrypt(block_t *ciphertext, key_t *key, block_t *plaintext)
{

    // Decrypts the given ciphertext using AES-128 decryption algorithm with the given key.
    // Args:
    // key: The decryption key to use.
    // ciphertext: The ciphertext to decrypt.
    // Returns:
    // The decrypted plaintext.

    // Create an instance of the AES cipher with the given key
    AES_KEY aes_key;
    AES_set_decrypt_key((const unsigned char *)key->data, 128, &aes_key);

    // Decrypt the ciphertext
    AES_decrypt((const unsigned char *)ciphertext->data, (unsigned char *)plaintext->data, &aes_key);
}

void pad(block_t *plaintext, uint8_t padding_size)
{

    // Pads the given plaintext to the nearest multiple of 16 bytes using the null character.

    // Args:
    // plaintext: The plaintext to pad.

    // Pad the plaintext to the nearest multiple of 16 bytes
    memset(plaintext->data + BLOCK_SIZE - padding_size, padding_size, padding_size);
}

uint8_t unpad(block_t *padded_plaintext)
{

    // Removes the padding from the given padded plaintext.
    // Args:
    // padded_plaintext: The padded plaintext to unpad.
    // Returns:
    // The unpadded plaintext.

    // Remove the padding from the plaintext
    uint8_t padding_size = padded_plaintext->data[BLOCK_SIZE - 1];
    memset(padded_plaintext->data + BLOCK_SIZE - padding_size, 0, padding_size);
    return padding_size;
}

void software(hls::stream<input_t> &input, hls::stream<output_t> &output, uint32_t num_iterations)
{

    // Define number of iterations
    uint32_t i = 0;

    // Time the implementation to measure the time it takes to encrypt and decrypt the test data
    uint64_t start_time = hls::timestamp_us();

    for (i = 0; i < num_iterations; i++)
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

    uint64_t end_time = hls::timestamp_us();
    uint64_t total_time = end_time - start_time;
    float avg_time = (float)total_time / num_iterations;

    // Output the results
    printf("AES-128 Encryption and Decryption\n");
    printf("Number of iterations: %d\n", num_iterations);
    printf("Average time taken: %.2f microseconds\n", avg_time);
}