#!/bin/bash

# Define variables
steps=(2 4 8 16 32 64 128 256 512)
#(2 4 8 16 32 64 128 256 512)
#(10 20 40 80 160 320 640 1280 2560)
#(1 2 4 8 16 32 64 128 256)
nprocs_values=(1 2 4 8 16 32 64 128 256)
location="hdd"
report="eva2_case1"
for i in ${!steps[@]}; do
  step=${steps[$i]}
  nprocs=${nprocs_values[$i]}
  jarvis cd gray_scott_bp5
  # First configuration and run
  mkdir -p ~/${report}/${nprocs}process
  jarvis pkg config adios2_gray_scott out_file=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp ppn=20 nprocs=$nprocs steps=$step L=512
  jarvis pkg config adios_hashing in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp ppn=20 nprocs=$nprocs out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy1.bp
  jarvis ppl run > ~/${report}/${nprocs}process/result1.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp

  # Second configuration and run
  jarvis pkg config adios2_gray_scott out_file=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp ppn=20 nprocs=$nprocs steps=$step L=512
  jarvis pkg config adios_hashing in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp ppn=20 nprocs=$nprocs out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy2.bp
  jarvis ppl run > ~/${report}/${nprocs}process/result2.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp

  # Hashing compare
  jarvis cd hashing_compare
  jarvis pkg config hashing_compare ppn=20 nprocs=$nprocs in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy1.bp out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy2.bp
  jarvis ppl run > ~/${report}/${nprocs}process/result3.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process

done