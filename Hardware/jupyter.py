from pynq import Overlay
import numpy as np
from pynq import allocate

stream_overlay = Overlay('./aes128.bit')

dma = stream_overlay.sergeyAES128.axi_dma_0
input_buffer = allocate(shape=(2,), dtype=np.uint64)
output_buffer = allocate(shape=(1,), dtype=np.uint64)

test_input = [
    0x00112233445566778899AABBCCDDEEFF,
    0xFFFFFFFFFFFFFFFF8899AABBCCDDEEFF
]

input_buffer[0] = test_input[0]
input_buffer[1] = test_input[1]

print("INPUT:")
for value in input_buffer:
    print(format(value, "016X"))

def run_kernel():
    dma.sendchannel.transfer(input_buffer)
    dma.recvchannel.transfer(output_buffer)
    dma.sendchannel.wait()
    dma.recvchannel.wait()

run_kernel()

print("OUTPUT:")
for value in output_buffer:
    print(format(value, "016X"))
