#!/bin/bash

# Define variables
L_values=(832 640 512 408 324 256 203)
nprocs_values=(64 32 16 8 4 2 1)

for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  jarvis cd gray_scott_hermes
  mkdir -p ~/case3_1/${nprocs}process
  jarvis pkg config adios2_gray_scott L=$L steps=40 out_file=/mnt/ssd/hxu40/ofs-mount/case3/${nprocs}process/out1.bp nprocs=${nprocs}
  jarvis pkg config adios2_gray_scott_2 L=$L steps=40 out_file=/mnt/ssd/hxu40/ofs-mount/case3/${nprocs}process/out2.bp nprocs=${nprocs}
  jarvis ppl run >> ~/case3_1/${nprocs}process/result.txt

done