from pynq import Overlay
import numpy as np
from pynq import allocate

stream_overlay = Overlay('./aes128.bit')

dma = stream_overlay.sergeyAES128.axi_dma_0

# Allocate input and output buffers for 32 uint8s
input_buffer = allocate(shape=(32,), dtype=np.uint8)
output_buffer = allocate(shape=(16,), dtype=np.uint8)

test_input = [
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
]

# Assign test_input values to input_buffer
for i, value in enumerate(test_input):
    input_buffer[i] = value

print("INPUT:")
for i in range(0, 32, 16):
    print(''.join(format(x, '02X') for x in input_buffer[i:i+16]))

def run_kernel():
    dma.sendchannel.transfer(input_buffer)
    dma.recvchannel.transfer(output_buffer)
    dma.sendchannel.wait()
    dma.recvchannel.wait()

run_kernel()

print("OUTPUT:")
print(''.join(format(x, '02X') for x in output_buffer))
