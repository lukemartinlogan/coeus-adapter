#!/bin/bash

# Define variables
L_values=(51200 25600 12800 6400 3200 1600 800 400 200)
nprocs_values=(256 128 64 32 16 8 4 2 1)
location="ssd"
report="case3_3_256"
for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  jarvis cd gray_scott_hermes
  mkdir -p ~/${report}/${nprocs}process
  jarvis pkg config adios2_gray_scott L=128 steps=$L out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out1.bp nprocs=${nprocs}
  jarvis pkg config adios2_gray_scott_2 L=128 steps=$L out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out2.bp nprocs=${nprocs}
  jarvis ppl run >> ~/${report}/${nprocs}process/result.txt

done