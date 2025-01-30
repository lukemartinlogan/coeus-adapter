#!/bin/bash

# Define variables
L_values=(1296 1024 832 640 512 408 324 256 203)
nprocs_values=(256 128 64 32 16 8 4 2 1)
location="hdd"
report="case3_eva_1_1"
for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  jarvis cd gray_scott_hermes
  mkdir -p ~/${report}/${nprocs}process
  jarvis pkg config adios2_gray_scott L=$L steps=40 out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out1.bp nprocs=${nprocs} ppn=20 checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case3/ckpt.bp engine=hermes_derived db_path=benchmark_metadata.db
  jarvis pkg config adios2_gray_scott_2 L=$L steps=40 out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out2.bp nprocs=${nprocs} ppn=20 checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case3/ckpt.bp engine=hermes_derived db_path=benchmark_metadata.db
  jarvis pkg config adios2_gray_scott_3 L=$L steps=40 out_file=/mnt/${location}/hxu40/ofs-mount/case3/${nprocs}process/out3.bp nprocs=${nprocs} ppn=20 checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case3/ckpt.bp engine=hermes_derived db_path=benchmark_metadata.db
  jarvis ppl run > ~/${report}/${nprocs}process/result.txt
  jarvis ppl clean
  jarvis ppl kill
  jarvis ppl update

done