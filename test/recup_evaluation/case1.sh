#!/bin/bash

# Define variables
L_values=(832 640 512 408 324 256 203)
nprocs_values=(64 32 16 8 4 2 1)
location="ssd"
report="case1_1"
for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  jarvis cd gray_scott_bp5
  # First configuration and run
  mkdir -p ~/${report}/${nprocs}process
  jarvis pkg config adios2_gray_scott out_file=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp ppn=16 nprocs=$nprocs steps=40 L=$L
  jarvis pkg config adios2_gray_scott_post in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp ppn=16 nprocs=$nprocs out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy1.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result1.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp

  # Second configuration and run
  jarvis pkg config adios2_gray_scott out_file=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp ppn=16 nprocs=$nprocs steps=70 L=$L
  jarvis pkg config adios2_gray_scott_post in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp ppn=16 nprocs=$nprocs out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy2.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result2.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp

  # Hashing compare
  jarvis cd hashing_compare
  jarvis pkg config hashing_compare nprocs=$nprocs in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy1.bp out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy2.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result3.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process

done