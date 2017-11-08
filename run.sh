#!/bin/bash

shopt -s extglob
cd  ./hellfireos-master/platform/noc_3x2/
make images
cd ../../../
rm ./hellfireos-master/usr/sim/mpsoc_sim/objects -rf
rm ./hellfireos-master/usr/sim/mpsoc_sim/reports -rf
mkdir ./hellfireos-master/usr/sim/mpsoc_sim/objects
mkdir ./hellfireos-master/usr/sim/mpsoc_sim/reports
mv ./hellfireos-master/platform/noc_3x2/!(makefile) ./hellfireos-master/usr/sim/mpsoc_sim/objects/
cd ./hellfireos-master/usr/sim/mpsoc_sim/
make noc_3x2
./mpsoc_sim 5 s
cd ../../../../
#rm img_matrix_bmp.h
#python parse_output.py ./hellfireos-master/usr/sim/mpsoc_sim/reports/out0.txt
#gcc create_bmp.c -o crbmp
#./crbmp
#display output.bmp
