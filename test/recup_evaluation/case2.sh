#!/bin/bash

# Define variables
L_values=(1296 1024 832 640 512 408 324 256 203)
nprocs_values=(256 128 64 32 16 8 4 2 1)
location="hdd"
report="case2_hdd"
for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  mkdir -p ~/${report}/${nprocs}process
  jarvis cd gray_scott
  jarvis pkg config adios2_gray_scott out_file=/mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process/out1.bp ppn=20 nprocs=${nprocs} steps=40 L=$L checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case2/ckpt.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result.txt

  jarvis pkg config adios2_gray_scott out_file=/mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process/out2.bp ppn=20 nprocs=${nprocs} steps=40 L=$L checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case2/ckpt.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result1.txt

  jarvis cd hashing_compare
  jarvis pkg config hashing_compare ppn=20 nprocs=${nprocs} in_filename=/mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process/out1.bp out_filename=/mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process/out2.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result2.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case2/${nprocs}process


done