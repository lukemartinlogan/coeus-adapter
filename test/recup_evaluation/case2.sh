#!/bin/bash

# Define variables
L_values=(832 640 512 408 324 256 203)
nprocs_values=(64 32 16 8 4 2 1)

for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  mkdir -p ~/case2_1/${nprocs}process
  jarvis cd gray_scott
  jarvis pkg config adios2_gray_scott out_file=/mnt/ssd/hxu40/ofs-mount/case2/${nprocs}process/out1.bp ppn=16 nprocs=${nprocs} steps=40 L=$L
  jarvis pkg config adios2_gray_scott_2 out_file=/mnt/ssd/hxu40/ofs-mount/case2/${nprocs}process/out2.bp ppn=16 nprocs=${nprocs} steps=40 L=$L
  jarvis ppl run >> ~/case2_1/${nprocs}process/result.txt

  jarvis cd hashing_compare
  jarvis pkg config hashing_compare nprocs=${nprocs} in_filename=/mnt/ssd/hxu40/ofs-mount/case2/${nprocs}process/out1.bp out_filename=/mnt/ssd/hxu40/ofs-mount/case2/${nprocs}process/out2.bp
  jarvis ppl run >> ~/case2_1/${nprocs}process/result2.txt
  rm -r /mnt/ssd/hxu40/ofs-mount/case2/${nprocs}process


done