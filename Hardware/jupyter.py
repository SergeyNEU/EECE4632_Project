from pynq import Overlay
import numpy as np
import random
from pynq import allocate

stream_overlay = Overlay('./aes128.bit')

dma = stream_overlay.sergeyAES128.axi_dma_0
input_buffer = allocate(shape=(11,), dtype=np.int32)
output_buffer = allocate(shape=(11,), dtype=np.int32)

input_buffer[:]=[random.randint(0,100) for k in range(11)]
print(input_buffer)

def run_kernel():
    dma.sendchannel.transfer(input_buffer)
    dma.recvchannel.transfer(output_buffer)
    dma.sendchannel.wait()
    dma.recvchannel.wait()

run_kernel()

print(output_buffer)