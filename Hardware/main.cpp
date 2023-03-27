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

#include "common.cpp"

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
