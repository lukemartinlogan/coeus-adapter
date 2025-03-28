#!/bin/bash

# Define variables
L_values=(848 680 540 512 428 340 268 214 170)
nprocs_values=(256 128 64 32 16 8 4 2 1)
location="hdd"
report="case1_fixed_step"

for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  jarvis cd gray_scott_bp5
  # First configuration and run
  mkdir -p ~/${report}/${nprocs}process
  jarvis pkg config adios2_gray_scott plotgap=1 out_file=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp ppn=20 nprocs=$nprocs steps=40 L=$L checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case1/ckpt.bp
  jarvis pkg config adios_hashing in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp ppn=20 nprocs=$nprocs out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy1.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result1.txt


  # Second configuration and run
  jarvis pkg config adios2_gray_scott plotgap=1 out_file=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp ppn=20 nprocs=$nprocs steps=40 L=$L checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case1/ckpt.bp
  jarvis pkg config adios_hashing in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp ppn=20 nprocs=$nprocs out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy2.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result2.txt


  # Hashing compare
  jarvis cd hashing_compare
  jarvis pkg config hashing_compare nprocs=$nprocs ppn=20 in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy1.bp out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy2.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result3.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process

done