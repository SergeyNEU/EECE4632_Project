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
#include <cstdint>
#include <string.h>

#include "encrypt.cpp"
#include "decrypt.cpp"

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
    block_t ciphertext; // Change this line: use block_t instead of block_t*
} output_t;

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

void main(hls::stream<input_t> &input, hls::stream<output_t> &output, uint32_t num_iterations)
{
    // Read input
    input_t in = input.read();

    // Pad the plaintext
    uint8_t padding_size = BLOCK_SIZE - (strlen((const char *)in.plaintext->data) % BLOCK_SIZE);
    pad(in.plaintext, padding_size);

    // Encrypt the plaintext
    encrypt(in.plaintext, in.key, &in.ciphertext);

    // Decrypt the ciphertext
    block_t decrypted_plaintext;
    decrypt(&in.ciphertext, in.key, &decrypted_plaintext);

    // Remove the padding from the plaintext
    uint8_t unpadded_size = unpad(&decrypted_plaintext);

    // Write output
    output_t out;
    memcpy(out.ciphertext.data, in.ciphertext.data, BLOCK_SIZE);
    memcpy(out.ciphertext.data + BLOCK_SIZE, &unpadded_size, 1);
    memcpy(out.ciphertext.data + BLOCK_SIZE + 1, decrypted_plaintext.data, unpadded_size);
    output.write(out);
}