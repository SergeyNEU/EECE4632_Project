#include "common.cpp"

void project(hls::stream<input_t> &INPUT, hls::stream<output_t> &OUTPUT)
{
	#pragma HLS INTERFACE axis port=INPUT
	#pragma HLS INTERFACE axis port=OUTPUT
	#pragma HLS INTERFACE s_axilite port=return

	while(1)
	{
		// Read INPUT
		input_t in = INPUT.read();

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

		// Write OUTPUT
		output_t out;
		memcpy(out.decryptedtext.data, decrypted_plaintext.data, unpadded_size);
		OUTPUT.write(out);
	}
}


