#include "project.h"

#ifdef _WIN32
#else
// Helper function to convert a block_t into a hexadecimal string for display
std::string blockToHexString(const block_t &block)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        ss << std::setw(2) << static_cast<unsigned>(block.data[i]);
    }
    return ss.str();
}

void print_state(uint8_t state[4][4])
{
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)state[row][col] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::dec << std::endl;
}

void print_round_keys(uint8_t round_keys[11][4][4])
{
    for (int round = 0; round < 11; ++round)
    {
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)round_keys[round][row][col] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::dec << std::endl;
    }
}
#endif

uint8_t hex_multiplication(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    uint8_t high_bit_mask = 0x80;
    uint8_t high_bit = 0;
    uint8_t modulo = 0x1B;

    for (int i = 0; i < 8; ++i)
    {
        if (b & 1)
        {
            p ^= a;
        }

        high_bit = a & high_bit_mask;
        a <<= 1;
        if (high_bit)
        {
            a ^= modulo;
        }
        b >>= 1;
    }
    return p;
}

void add_round_key(uint8_t state[4][4], uint8_t round_key[4][4])
{
#ifdef _WIN32
#pragma HLS ARRAY_PARTITION variable=round_key complete dim=2
#endif
    add_round_key_label2:for (int row = 0; row < 4; ++row)
    {
#ifdef _WIN32
#pragma HLS PIPELINE II=8
#endif
        for (int col = 0; col < 4; ++col)
        {
            state[row][col] ^= round_key[row][col];
        }
    }
}

void key_expansion(const uint8_t key[BLOCK_SIZE], uint8_t round_keys[11][4][4])
{
#ifdef _WIN32
#pragma HLS PIPELINE II=25
#endif
    for (int i = 0; i < 4; ++i)
    {
        round_keys[0][0][i] = key[i];
        round_keys[0][1][i] = key[4 + i];
        round_keys[0][2][i] = key[8 + i];
        round_keys[0][3][i] = key[12 + i];
    }
#ifdef _WIN32
#pragma HLS DEPENDENCE variable=round_keys inter true
#endif
    for (int round = 1; round <= 10; ++round)
    {
        uint8_t temp[4];
        temp[0] = s_box[round_keys[round - 1][1][3]] ^ rcon[round];
        temp[1] = s_box[round_keys[round - 1][2][3]];
        temp[2] = s_box[round_keys[round - 1][3][3]];
        temp[3] = s_box[round_keys[round - 1][0][3]];
        for (int j = 0; j < 4; ++j)
        {
            round_keys[round][0][j] = round_keys[round - 1][0][j] ^ temp[j];
        }
        for (int j = 0; j < 4; ++j)
        {
            round_keys[round][1][j] = round_keys[round - 1][1][j] ^ round_keys[round][0][j];
        }
        for (int j = 0; j < 4; ++j)
        {
            round_keys[round][2][j] = round_keys[round - 1][2][j] ^ round_keys[round][1][j];
        }

        for (int j = 0; j < 4; ++j)
        {
            round_keys[round][3][j] = round_keys[round - 1][3][j] ^ round_keys[round][2][j];
        }
    }
}

void inv_sub_bytes(uint8_t state[4][4])
{
#ifdef _WIN32
#pragma HLS PIPELINE II=18
#endif
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            state[row][col] = inv_s_box[state[row][col]];
        }
    }
}

void inv_shift_rows(uint8_t state[4][4])
{
    uint8_t temp;
    // Row 1: right circular shift by 1
    temp = state[1][3];
    for (int col = 3; col > 0; --col)
    {
        state[1][col] = state[1][col - 1];
    }
    state[1][0] = temp;

    // Row 2: right circular shift by 2
    temp = state[2][2];
    state[2][2] = state[2][0];
    state[2][0] = temp;
    temp = state[2][3];
    state[2][3] = state[2][1];
    state[2][1] = temp;
    // Row 3: right circular shift by 3
    temp = state[3][0];
    for (int col = 0; col < 3; ++col)
    {
        state[3][col] = state[3][col + 1];
    }
    state[3][3] = temp;
}

void inv_mix_columns(uint8_t state[4][4])
{
#ifdef _WIN32
#pragma HLS PIPELINE II=16
#endif
    uint8_t temp[4][4];
#ifdef _WIN32
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=temp
#endif
    for (int col = 0; col < 4; ++col)
    {
        temp[0][col] = hex_multiplication(0x0e, state[0][col]) ^ hex_multiplication(0x0b, state[1][col]) ^ hex_multiplication(0x0d, state[2][col]) ^ hex_multiplication(0x09, state[3][col]);
        temp[1][col] = hex_multiplication(0x09, state[0][col]) ^ hex_multiplication(0x0e, state[1][col]) ^ hex_multiplication(0x0b, state[2][col]) ^ hex_multiplication(0x0d, state[3][col]);
        temp[2][col] = hex_multiplication(0x0d, state[0][col]) ^ hex_multiplication(0x09, state[1][col]) ^ hex_multiplication(0x0e, state[2][col]) ^ hex_multiplication(0x0b, state[3][col]);
        temp[3][col] = hex_multiplication(0x0b, state[0][col]) ^ hex_multiplication(0x0d, state[1][col]) ^ hex_multiplication(0x09, state[2][col]) ^ hex_multiplication(0x0e, state[3][col]);
    }

    memcpy(state, temp, 16);
}

void aes_128_decrypt(const uint8_t ciphertext[BLOCK_SIZE], const uint8_t key[4 * 4], uint8_t *plaintext)
{
    uint8_t state[4][4];
    uint8_t round_keys[11][4][4];
    // Initialize state and perform key expansion
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            state[row][col] = ciphertext[row + 4 * col];
        }
    }
    key_expansion(key, round_keys);

    // Initial round
    add_round_key(state, round_keys[10]);

    // Main rounds
    for (int round = 9; round > 0; --round)
    {
        inv_shift_rows(state);
        inv_sub_bytes(state);
        add_round_key(state, round_keys[round]);
        inv_mix_columns(state);
    }

    // Final round
    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, round_keys[0]);

    // Store the result
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            plaintext[row + 4 * col] = state[row][col];
        }
    }
}

void sub_bytes(uint8_t state[4][4])
{
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            state[row][col] = s_box[state[row][col]];
        }
    }
}

void shift_rows(uint8_t state[4][4])
{
    shift_rows_label0:for (int row = 1; row < 4; ++row)
    {
#ifdef _WIN32
#pragma HLS PIPELINE II=8
#endif
        uint8_t temp_row[4];
        for (int col = 0; col < 4; ++col)
        {
            temp_row[col] = state[row][(col + row) % 4];
        }
        memcpy(state[row], temp_row, 4);
    }
}

void mix_columns(uint8_t state[4][4])
{
    uint8_t temp_state[4][4];
    mix_columns_label1:for (int col = 0; col < 4; ++col)
    {
#ifdef _WIN32
#pragma HLS PIPELINE II=15
#endif
        temp_state[0][col] = hex_multiplication(0x02, state[0][col]) ^ hex_multiplication(0x03, state[1][col]) ^ state[2][col] ^ state[3][col];
        temp_state[1][col] = state[0][col] ^ hex_multiplication(0x02, state[1][col]) ^ hex_multiplication(0x03, state[2][col]) ^ state[3][col];
        temp_state[2][col] = state[0][col] ^ state[1][col] ^ hex_multiplication(0x02, state[2][col]) ^ hex_multiplication(0x03, state[3][col]);
        temp_state[3][col] = hex_multiplication(0x03, state[0][col]) ^ state[1][col] ^ state[2][col] ^ hex_multiplication(0x02, state[3][col]);
    }
    memcpy(state, temp_state, 16);
}

void aes_128_encrypt(const uint8_t plaintext[BLOCK_SIZE], const uint8_t key[BLOCK_SIZE], uint8_t *ciphertext)
{
    uint8_t state[4][4];
    uint8_t round_keys[11][4][4];
    // Initialize state and perform key expansion
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            state[row][col] = plaintext[row + 4 * col];
        }
    }

    key_expansion(key, round_keys);

    // Initial round
    add_round_key(state, round_keys[0]);

    // Main rounds
    for (int round = 1; round < 10; ++round)
    {
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, round_keys[round]);
    }

    // Final round
    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, round_keys[10]);

    // Store the result
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            ciphertext[row + 4 * col] = state[row][col];
        }
    }
}

#ifdef _WIN32
void project(hls::stream<input_t> &INPUT, hls::stream<output_t> &OUTPUT)
{
#pragma HLS TOP name=project
#pragma HLS INTERFACE axis port = INPUT
#pragma HLS INTERFACE axis port = OUTPUT
#pragma HLS INTERFACE s_axilite port = return

    while (1)
    {
        // Read INPUT
        input_t in = INPUT.read();

        // Encrypt the plaintext
        uint8_t ciphertext[BLOCK_SIZE];
        aes_128_encrypt(in.plaintext.data, in.key.data, ciphertext);

        // Decrypt the ciphertext
        uint8_t decrypted_ciphertext[BLOCK_SIZE];
        aes_128_decrypt(ciphertext, in.key.data, decrypted_ciphertext);

        // Write OUTPUT
        output_t out;

        // Assign array values
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            out.decryptedtext.data[i] = decrypted_ciphertext[i];
        }

        OUTPUT.write(out);
    }
}
#else

// Function to convert a byte array to a hex string
std::string to_hex_string(const uint8_t *data, size_t size)
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < size; ++i)
    {
        oss << std::setw(2) << static_cast<int>(data[i]);
    }
    return oss.str();
}

int main()
{
    srand(time(NULL));

    // Generate random INPUT
    input_t in;

    // Generate random data for the KEY
    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        in.key.data[i] = rand() % 100;
    }

    // Our text
    in.plaintext.data[0] = 'T';
    in.plaintext.data[1] = 'e';
    in.plaintext.data[2] = 's';
    in.plaintext.data[3] = 't';
    in.plaintext.data[4] = 'i';
    in.plaintext.data[5] = 'n';
    in.plaintext.data[6] = 'g';
    in.plaintext.data[7] = 'H';
    in.plaintext.data[8] = 'e';
    in.plaintext.data[9] = 'l';
    in.plaintext.data[10] = 'l';
    in.plaintext.data[11] = 'o';
    in.plaintext.data[12] = 'o';
    in.plaintext.data[13] = 'o';
    in.plaintext.data[14] = 'o';
    in.plaintext.data[15] = 'o';

    std::cout << "(str) Plaintext: ";
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        std::cout << static_cast<char>(in.plaintext.data[i]);
    }
    std::cout << std::endl;

    std::cout << "(hex) Plaintext: " << to_hex_string(in.plaintext.data, BLOCK_SIZE) << std::endl;
    std::cout << "(hex) Random key: " << to_hex_string(in.key.data, BLOCK_SIZE) << std::endl;

    // Encrypt the plaintext
    uint8_t ciphertext[BLOCK_SIZE];
    aes_128_encrypt(in.plaintext.data, in.key.data, ciphertext);
    std::cout << "(hex) Encrypted ciphertext: " << to_hex_string(ciphertext, BLOCK_SIZE) << std::endl;

    // Decrypt the ciphertext
    uint8_t decrypted_ciphertext[BLOCK_SIZE];
    aes_128_decrypt(ciphertext, in.key.data, decrypted_ciphertext);
    std::cout << "(hex) Decrypted plaintext: " << to_hex_string(decrypted_ciphertext, BLOCK_SIZE) << std::endl;

    std::cout << "(str) Decrypted Plaintext: ";
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        std::cout << static_cast<char>(decrypted_ciphertext[i]);
    }
    std::cout << std::endl;

    return 0;
}
#endif
