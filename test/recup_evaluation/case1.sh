#!/bin/bash

# Define variables
L_values=(1296 1024 832 640 512 408 324 256 203)
nprocs_values=(256 128 64 32 16 8 4 2 1)
location="hdd"
report="case1_3_hdd"
jarvis repo add ~/coeus/
jarvis repo create case1_eva_1 app
jarvis cd case1_eva_1
jarvis ppl append adios2_gray_scott engine=bp5
jarvis ppl append adios_hashing engine=bp5
jarvis repo create hashing_compare
jarvis ppl append hashing_compare
for i in ${!L_values[@]}; do
  L=${L_values[$i]}
  nprocs=${nprocs_values[$i]}
  jarvis cd case1_eva_1
  # First configuration and run
  mkdir -p ~/${report}/${nprocs}process
  jarvis pkg config adios2_gray_scott out_file=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp ppn=20 nprocs=$nprocs steps=40 L=$L checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case1/ckpt.bp
  jarvis pkg config adios_hashing in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp ppn=20 nprocs=$nprocs out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy1.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result1.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out1.bp

  # Second configuration and run
  jarvis pkg config adios2_gray_scott out_file=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp ppn=20 nprocs=$nprocs steps=40 L=$L checkpoint_output=/mnt/${location}/hxu40/ofs-mount/case1/ckpt.bp
  jarvis pkg config adios_hashing in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp ppn=20 nprocs=$nprocs out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy2.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result2.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/out2.bp

  # Hashing compare
  jarvis cd hashing_compare
  jarvis pkg config hashing_compare nprocs=$nprocs ppn=20 in_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy1.bp out_filename=/mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process/copy2.bp
  jarvis ppl run >> ~/${report}/${nprocs}process/result3.txt
  rm -r /mnt/${location}/hxu40/ofs-mount/case1/${nprocs}process

done