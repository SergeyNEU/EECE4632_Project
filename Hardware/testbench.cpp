#include "project.cpp"

// Testbench
int main()
{
    hls::stream<input_t> INPUT;
    hls::stream<output_t> OUTPUT;

 uint8_t test_input[BLOCK_SIZE * 2] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                                          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

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
