#!/bin/bash

# Define variables
L_values=(1280 640 320 160 80 40 20 10 5)
nprocs_values=(256 128 64 32 16 8 4 2 1)
location="ssd"
report="case3_3_256"
for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  jarvis cd gray_scott_hermes
  mkdir -p ~/${report}/${nprocs}process
  jarvis pkg config adios2_gray_scott L=512 steps=$L out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out1.bp nprocs=${nprocs} ppn=20  checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case3/ckpt.bp db_path=/mnt/${location}/hxu40/ofs-mount/case3/benchmark_metadata.db
  jarvis pkg config adios2_gray_scott_2 L=512 steps=$L out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out2.bp nprocs=${nprocs} ppn=20 checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case3/ckpt.bp db_path=/mnt/${location}/hxu40/ofs-mount/case3/benchmark_metadata.db
  jarvis ppl run > ~/${report}/${nprocs}process/result.txt
    jarvis ppl clean
    jarvis ppl kill
    jarvis ppl update

done