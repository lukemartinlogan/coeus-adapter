#!/bin/bash

# Define variables
L_values=(2 4 8 16 32 64 128 256 512)
nprocs_values=(1 2 4 8 16 32 64 128 256)
location="ssd"
report="case3_3_256"
for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  jarvis cd gray_scott_hermes
  mkdir -p ~/${report}/${nprocs}process
  jarvis pkg config adios2_gray_scott L=512 steps=$L out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out1.bp nprocs=${nprocs} ppn=20  checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case3/ckpt.bp
  jarvis pkg config adios2_gray_scott_2 L=512 steps=$L out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out2.bp nprocs=${nprocs} ppn=20 checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case3/ckpt.bp
   jarvis pkg config adios2_gray_scott_3 L=512 steps=$L out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out3.bp nprocs=${nprocs} ppn=20 checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case3/ckpt.bp
  jarvis ppl run > ~/${report}/${nprocs}process/result.txt
    jarvis ppl clean
    jarvis ppl kill
    jarvis ppl update
done