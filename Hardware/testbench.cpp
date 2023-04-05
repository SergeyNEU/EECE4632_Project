#include "project.cpp"

// Testbench
int main()
{
    hls::stream<input_t> INPUT;
    hls::stream<output_t> OUTPUT;

    input_t test_input;
    output_t test_output;

    // Fill test_input with data
    uint8_t sample_plaintext[BLOCK_SIZE] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t sample_key[BLOCK_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        test_input.plaintext.data[i] = sample_plaintext[i];
        test_input.key.data[i] = sample_key[i];
    }

    // Write input data to INPUT stream
    INPUT.write(test_input);

    // Call the project function
    project(INPUT, OUTPUT);

    // Read output data from OUTPUT stream
    test_output = OUTPUT.read();

    // Check the output and compare with the expected result
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        assert(test_output.decryptedtext.data[i] == test_input.plaintext.data[i]);
    }

    std::cout << "Test passed!" << std::endl;
    return 0;
}
