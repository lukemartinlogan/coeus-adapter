#!/bin/bash

# Define variables
L_values=(51200 25600 12800 6400 3200 1600 800 400 200)
nprocs_values=(256 128 64 32 16 8 4 2 1)
location="nvme"
report="case2_2_256"
for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  mkdir -p ~/${report}/${nprocs}process
  jarvis cd gray_scott
  jarvis pkg config adios2_gray_scott out_file=/mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process/out1.bp ppn=20 nprocs=${nprocs} steps=$L L=128
  jarvis pkg config adios2_gray_scott_2 out_file=/mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process/out2.bp ppn=20 nprocs=${nprocs} steps=$L L=128
  jarvis ppl run &> ~/${report}/${nprocs}process/result.txt

  jarvis cd hashing_compare
  jarvis pkg config hashing_compare ppn=20 nprocs=${nprocs} in_filename=/mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process/out1.bp out_filename=/mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process/out2.bp
  jarvis ppl run &> ~/${report}/${nprocs}process/result2.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process


done