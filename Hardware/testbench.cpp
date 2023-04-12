#include "project.h"

// Testbench
int main()
{
    hls::stream<axis256_t> INPUT;
    hls::stream<axis128_t> OUTPUT;

    uint8_t test_input[BLOCK_SIZE * 2] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                                          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t test_output[BLOCK_SIZE];

    // Write input data to INPUT stream
    axis256_t input_data;
    std::cout << "INPUT: ";
    for (int i = 0; i < BLOCK_SIZE * 2; i += 32)
    {
        for (int j = 0; j < 32; j++)
        {
            input_data.data.range(8 * (j + 1) - 1, 8 * j) = test_input[i + j];
        }
        INPUT.write(input_data);

        for (int j = 0; j < 32; j++)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(test_input[i + j]) << " ";
        }
        std::cout << std::endl;
    }

    // Call the project function
    project(INPUT, OUTPUT);

    // Read output data from OUTPUT stream and print it
    axis128_t output_data = OUTPUT.read();
    std::cout << "OUTPUT: ";
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        uint8_t value = output_data.data.range(8 * (i + 1) - 1, 8 * i).to_uint();
        test_output[i] = value;
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value) << " ";
    }
    std::cout << std::endl;

    return 0;
}
